#include <cmath>
#include <vector>
#include <algorithm>
#include <random>
#include <limits>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <cstdlib>
#include <clocale>
#include <locale>
#include <sstream>

#include "headers.h"
#include "sen_distribution.h"

using namespace std;

void test_sen_class();

// Обработка одной выборки
void process_sample(const std::vector<double>& sample, int n,
    double mu, double la, double v,
    double mu1, double la1, double v1,
    double mu2, double la2, double v2,
    double p,
    bool is_new_sample = false) {

    std::cout << "  Обработка " << (is_new_sample ? "новой" : "классической")
        << " выборки размером n = " << n << std::endl;

    // Эмпирическая плотность
    EmpiricalDensity ed;
    build_empirical_density_c(sample.data(), (int)sample.size(), -1, &ed);

    // Имя CSV
    std::stringstream filename;
    filename << n;
    if (is_new_sample) filename << "_new";
    filename << "sen_density_data.csv";
    std::string csv_name = filename.str();

    // Запись CSV
    {
        double xmin_csv = ed.a - 0.5 * (ed.b - ed.a);
        double xmax_csv = ed.b + 0.5 * (ed.b - ed.a);
        int    M_csv = 800;
        double h_csv = (xmax_csv - xmin_csv) / (M_csv - 1);

        std::ofstream out(csv_name);
        out.imbue(std::locale::classic());
        out << std::fixed << std::setprecision(12);
        out << "x;f_main;f_mix;f_emp\n";

        SENDistribution d_main(mu, la, v);
        for (int i = 0; i < M_csv; ++i) {
            double x = xmin_csv + i * h_csv;
            double f_main = d_main.pdf(x);
            double f_mix = mix_pdf(x, mu1, la1, v1, mu2, la2, v2, p);
            double f_emp = empirical_pdf_at_c(&ed, x, n);
            out << x << ';' << f_main << ';' << f_mix << ';' << f_emp << '\n';
        }
        out.close();
        std::cout << "  Файл " << csv_name << " создан" << std::endl;
    }

    // Теоретические моменты компонентов смеси
    Moments m1 = SENDistribution(mu1, la1, v1).moments();
    Moments m2 = SENDistribution(mu2, la2, v2).moments();
    Moments mm_theor = mix_moments(m1, m2, p);

    // Эмпирические моменты
    double Mx = 0, D2x = 0;
    for (double x : sample) Mx += x;
    Mx /= sample.size();
    for (double x : sample) D2x += (x - Mx) * (x - Mx);
    D2x /= sample.size();
    double D = std::sqrt(D2x);
    double m3 = 0, m4 = 0;
    for (double x : sample) {
        double d = x - Mx;
        m3 += d * d * d;
        m4 += d * d * d * d;
    }
    m3 /= sample.size();
    m4 /= sample.size();
    double g1 = m3 / (D2x * D);
    double g2 = m4 / (D2x * D2x) - 3.0;

    // Вывод сводки
    std::cout << "  Теория: M=" << mm_theor.M << ", D=" << mm_theor.D2
        << ", g1=" << mm_theor.g1 << ", g2=" << mm_theor.g2 << "\n";
    std::cout << "  Эмпирика: M=" << Mx << ", D=" << D2x
        << ", g1=" << g1 << ", g2=" << g2 << "\n\n";

    free_empirical(&ed);
}

// Генерация новой выборки с другим сидом
std::vector<double> generate_new_sample(int n,
    double mu1, double la1, double v1,
    double mu2, double la2, double v2,
    double p) {

    std::mt19937_64 old_rng = rng;
    rng.seed(987654321);

    std::vector<double> new_sample;
    new_sample.reserve(n);
    for (int i = 0; i < n; ++i) {
        double u = randU();
        double x = (u < p) ? sen_sample(mu2, la2, v2) : sen_sample(mu1, la1, v1);
        new_sample.push_back(x);
    }
    rng = old_rng;
    return new_sample;
}

int main() {
    std::setlocale(LC_ALL, "ru");
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    // Тесты
    test_against_table();
    run_required_tests();

    // Параметры
    double mu = 1.0, la = 2.0, v = 1.0;
    double mu1 = 0.0, la1 = 1.0, v1 = 1.0;
    double mu2 = 2.0, la2 = 1.5, v2 = 2.0;
    double p = 0.4;

    vector<int> sample_sizes = { 200, 1000, 5000, 20000, 100000 };

    for (int n : sample_sizes) {
        vector<double> sample;
        sample.reserve(n);
        for (int i = 0; i < n; ++i) {
            double u = randU();
            double x = (u < p) ? sen_sample(mu2, la2, v2) : sen_sample(mu1, la1, v1);
            sample.push_back(x);
        }

        process_sample(sample, n, mu, la, v, mu1, la1, v1, mu2, la2, v2, p, false);

        if (n == 1000) {
            std::cout << "  ==============================\n";
            std::cout << "  === ВЫПОЛНЕНИЕ ТЕСТА 3.3.2 ===\n";
            vector<double> new_sample = generate_new_sample(n, mu1, la1, v1, mu2, la2, v2, p);
            process_sample(new_sample, n, mu, la, v, mu1, la1, v1, mu2, la2, v2, p, true);

            std::cout << "  СРАВНЕНИЕ ВЫБОРОК РАЗМЕРОМ 1000:" << std::endl;
            double Mx1 = 0, D2x1 = 0;
            for (double x : sample) Mx1 += x;
            Mx1 /= sample.size();
            for (double x : sample) D2x1 += (x - Mx1) * (x - Mx1);
            D2x1 /= sample.size();

            double Mx2 = 0, D2x2 = 0;
            for (double x : new_sample) Mx2 += x;
            Mx2 /= new_sample.size();
            for (double x : new_sample) D2x2 += (x - Mx2) * (x - Mx2);
            D2x2 /= new_sample.size();

            std::cout << "  Классическая выборка: M=" << Mx1 << ", D=" << D2x1 << std::endl;
            std::cout << "  Новая выборка: M=" << Mx2 << ", D=" << D2x2 << std::endl;
            std::cout << "  Разница: ΔM=" << std::abs(Mx1 - Mx2) << ", ΔD=" << std::abs(D2x1 - D2x2) << "\n";
            std::cout << "  ==============================\n\n";
        }

        sample.clear();
    }

    // Вызов тестов класса
     test_sen_class();

    return 0;
}

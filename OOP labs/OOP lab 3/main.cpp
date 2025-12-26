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
#include "sen_distribution.h"
#include "mixture_distribution.h"
#include "empirical_distribution.h"
#include "tests.h"

using namespace std;

// Генерация новой выборки с фиксированным seed для воспроизводимости
std::vector<double> generate_new_sample(int n,
    double mu1, double la1, double v1,
    double mu2, double la2, double v2,
    double p);

// Обработка одной выборки: вычисление плотностей, моментов и сохранение в CSV
void process_sample(const std::vector<double>& sample, int n,
    double mu, double la, double v,
    double mu1, double la1, double v1,
    double mu2, double la2, double v2,
    double p, bool is_new_sample) {

    cout << "  Обработка " << (is_new_sample ? "новой" : "классической")
        << " выборки размером n = " << n << endl;

    // Создание эмпирического распределения из переданной выборки
    EmpiricalDistribution ed(sample);

    // Формирование имени CSV файла в зависимости от размера и типа выборки
    stringstream filename;
    filename << n;
    if (is_new_sample) filename << "_new";
    filename << "sen_density_data.csv";
    string csv_name = filename.str();


    // Блок вычисления и записи плотностей в CSV
    {
        // Определение границ графика с запасом
        const auto& ed_sample = ed.get_sample();
        double xmin_csv = *min_element(ed_sample.begin(), ed_sample.end());
        double xmax_csv = *max_element(ed_sample.begin(), ed_sample.end());
        xmin_csv -= 0.5 * (xmax_csv - xmin_csv); // Расширяем левую границу
        xmax_csv += 0.5 * (xmax_csv - xmin_csv); // Расширяем правую границу

        int M_csv = 800;
        double h_csv = (xmax_csv - xmin_csv) / (M_csv - 1);

        ofstream out(csv_name);
        out.imbue(locale::classic());
        out << fixed << setprecision(12);
        out << "x;f_main;f_mix;f_emp\n";

        // Создание трех распределений для сравнения (эмпирческое выше):
        SENDistribution d_main(mu, la, v); // - основное SEN распределение

        SENDistribution d1(mu1, la1, v1); // - смесь распределений
        SENDistribution d2(mu2, la2, v2);
        MixtureDistribution d_mix(d1, d2, p);

        // Запись плотностей в CSV для построения графиков
        for (int i = 0; i < M_csv; ++i) {
            double x = xmin_csv + i * h_csv;
            double f_main = d_main.pdf(x); // Плотность основного распределения
            double f_mix = d_mix.pdf(x); // Плотность смеси
            double f_emp = ed.pdf(x); // Эмпирическая плотность
            out << x << ';' << f_main << ';' << f_mix << ';' << f_emp << '\n';
        }
        out.close();
        cout << "  Файл " << csv_name << " создан" << endl;
    }

    // Теоретические моменты через класс MixtureDistribution
    SENDistribution d1_theor(mu1, la1, v1);
    SENDistribution d2_theor(mu2, la2, v2);
    MixtureDistribution mix_dist(d1_theor, d2_theor, p);
    Moments mm_theor = mix_dist.moments();

    // Эмпирические моменты через класс EmpiricalDistribution
    Moments m_emp = ed.moments();

    // Вывод сводки
    cout << "  Теория: M=" << mm_theor.M << ", D=" << mm_theor.D2
        << ", g1=" << mm_theor.g1 << ", g2=" << mm_theor.g2 << "\n";
    cout << "  Эмпирика: M=" << m_emp.M << ", D=" << m_emp.D2
        << ", g1=" << m_emp.g1 << ", g2=" << m_emp.g2 << "\n\n";
}

// Генерация новой выборки с другим сидом
vector<double> generate_new_sample(int n,
    double mu1, double la1, double v1,
    double mu2, double la2, double v2,
    double p) {

    // Сохранение текущего состояния генератора случайных чисел
    mt19937_64 old_rng = rng;
    rng.seed(987654321);

    // Используем класс MixtureDistribution для генерации
    SENDistribution d1(mu1, la1, v1);
    SENDistribution d2(mu2, la2, v2);
    MixtureDistribution mix_dist(d1, d2, p);

    vector<double> new_sample;
    new_sample.reserve(n);
    for (int i = 0; i < n; ++i) {
        new_sample.push_back(mix_dist.sample());
    }

    rng = old_rng;
    return new_sample;
}

int main() {
    setlocale(LC_ALL, "ru");
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    // Тесты
    test_against_table();
    run_required_tests();

    // Параметры распределений:
    // - основное SEN распределение (mu, la, v)
    // - два компонента смеси (mu1, la1, v1) и (mu2, la2, v2)
    // - вероятность p для смеси
    double mu = 1.0, la = 2.0, v = 1.0;
    double mu1 = 0.0, la1 = 1.0, v1 = 1.0;
    double mu2 = 2.0, la2 = 1.5, v2 = 2.0;
    double p = 0.4;

    // Размеры выборок для исследования
    vector<int> sample_sizes = { 200, 1000, 5000, 20000, 100000 };


    // Основной цикл по размерам выборок
    for (int n : sample_sizes) {
        // Создаем объекты распределений для генерации
        SENDistribution d1(mu1, la1, v1);
        SENDistribution d2(mu2, la2, v2);
        MixtureDistribution mix_gen(d1, d2, p);

        // Генерируем выборку через класс
        vector<double> sample;
        sample.reserve(n);
        for (int i = 0; i < n; ++i) {
            sample.push_back(mix_gen.sample());
        }

        process_sample(sample, n, mu, la, v, mu1, la1, v1, mu2, la2, v2, p, false);

        if (n == 1000) {
            cout << "  ==============================\n";
            cout << "  === ВЫПОЛНЕНИЕ ТЕСТА 3.3.2 ===\n";
            vector<double> new_sample = generate_new_sample(n, mu1, la1, v1, mu2, la2, v2, p);
            process_sample(new_sample, n, mu, la, v, mu1, la1, v1, mu2, la2, v2, p, true);

            // Сравнение моментов через класс EmpiricalDistribution
            EmpiricalDistribution ed1(sample);
            EmpiricalDistribution ed2(new_sample);

            Moments m1 = ed1.moments();
            Moments m2 = ed2.moments();

            cout << "  СРАВНЕНИЕ ВЫБОРОК РАЗМЕРОМ 1000:" << endl;
            cout << "  Классическая выборка: M=" << m1.M << ", D=" << m1.D2 << endl;
            cout << "  Новая выборка: M=" << m2.M << ", D=" << m2.D2 << endl;
            cout << "  Разница: ΔM=" << abs(m1.M - m2.M) << ", ΔD=" << abs(m1.D2 - m2.D2) << "\n";
            cout << "  ==============================\n\n";
        }

        sample.clear();
    }

    // Вызов тестов классов
    test_sen_class();
    test_new_classes();

    return 0;
}
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

using namespace std;


// Функция для обработки одной выборки
void process_sample(const std::vector<double>& sample, int n,
    double mu, double la, double v,
    double mu1, double la1, double v1,
    double mu2, double la2, double v2,
    double p,
    bool is_new_sample = false) {

    std::cout << "  Обработка " << (is_new_sample ? "новой" : "классической")
        << " выборки размером n = " << n << std::endl;

    // Построение эмпирической плотности по сгенерированной выборке
    EmpiricalDensity ed;
    build_empirical_density_c(sample.data(), (int)sample.size(), -1, &ed);

    // Создание имени файла с размерностью выборки и меткой новой выборки
    std::stringstream filename;
    filename << n;
    if (is_new_sample) {
        filename << "_new";
    }
    filename << "sen_density_data.csv";
    std::string csv_name = filename.str();

    // Запись данных в CSV-файл для последующего построения графиков
    {
        // Расширяем диапазоны, выставляем число точек и вычисляем шаг для записи
        double xmin_csv = ed.a - 0.5 * (ed.b - ed.a);
        double xmax_csv = ed.b + 0.5 * (ed.b - ed.a);
        int    M_csv = 800;
        double h_csv = (xmax_csv - xmin_csv) / (M_csv - 1);

        // Создание файла и запись
        std::ofstream out(csv_name);
        out.imbue(std::locale::classic());
        out << std::fixed << std::setprecision(12);
        out << "x;f_main;f_mix;f_emp\n";

        for (int i = 0; i < M_csv; ++i) {
            double x = xmin_csv + i * h_csv;
            double f_main = sen_pdf(x, mu, la, v);
            double f_mix = mix_pdf(x, mu1, la1, v1, mu2, la2, v2, p);
            double f_emp = empirical_pdf_at_c(&ed, x, n);
            out << x << ';' << f_main << ';' << f_mix << ';' << f_emp << '\n';
        }
        out.close();
        std::cout << "  Файл " << csv_name << " создан" << std::endl;
    }

    // Расчет теоретических моментов для смеси
    Moments m1, m2;
    sen_moments_c(mu1, la1, v1, &m1);
    sen_moments_c(mu2, la2, v2, &m2);
    Moments mm_theor = mix_moments(m1, m2, p);

    // Расчет эмпирических моментов по сгенерированной выборке
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

    // Вывод сводки по смеси (для проверки)
    std::cout << "  Теория: M=" << mm_theor.M << ", D=" << mm_theor.D2
        << ", g1=" << mm_theor.g1 << ", g2=" << mm_theor.g2 << "\n";
    std::cout << "  Эмпирика: M=" << Mx << ", D=" << D2x
        << ", g1=" << g1 << ", g2=" << g2 << "\n\n";

    // Освобождение памяти
    free_empirical(&ed);
}

// Функция для генерации новой выборки с другим сидом
std::vector<double> generate_new_sample(int n,
    double mu1, double la1, double v1,
    double mu2, double la2, double v2,
    double p) {
    // Сохраняем текущее состояние генератора
    std::mt19937_64 old_rng = rng;

    // Устанавливаем новый сид для генерации новой выборки
    rng.seed(987654321); // Другой сид для новой выборки

    std::vector<double> new_sample;
    new_sample.reserve(n);
    for (int i = 0; i < n; ++i) {
        double u = randU();
        double x = (u < p) ? sen_sample(mu2, la2, v2) : sen_sample(mu1, la1, v1);
        new_sample.push_back(x);
    }

    // Восстанавливаем исходное состояние генератора
    rng = old_rng;

    return new_sample;
}


int main() {
    std::setlocale(LC_ALL, "ru");
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    // Запуск тестов
    test_against_table(); // Сравнение с табличными значениями
    run_required_tests(); // Обязательные тесты по заданию 3.1 3.2

    // Параметры распределений для построения графиков
    double mu = 1.0, la = 2.0, v = 1.0; // Параметры основного SEN
    double mu1 = 0.0, la1 = 1.0, v1 = 1.0; // Параметры первой компоненты смеси
    double mu2 = 4.0, la2 = 3.0, v2 = 2.0; // Параметры второй компоненты смеси
    double p = 0.7; // Вес второй компоненты

    // Массив размерностей выборок
    vector<int> sample_sizes = { 200, 1000, 5000, 20000, 100000 };

    // Обработка каждой размерности выборки
    for (int n : sample_sizes) {
        // Генерация классической выборки объема n из смеси двух SEN-распределений
        vector<double> sample;
        sample.reserve(n);
        for (int i = 0; i < n; ++i) {
            double u = randU();
            double x = (u < p) ? sen_sample(mu2, la2, v2) : sen_sample(mu1, la1, v1);
            sample.push_back(x);
        }

        // Обработка классической выборки
        process_sample(sample, n, mu, la, v, mu1, la1, v1, mu2, la2, v2, p, false);

        // Для выборки размером 1000 выполняем тест 3.3.2
        if (n == 1000) {
            std::cout << "  ==============================\n";
            std::cout << "  === ВЫПОЛНЕНИЕ ТЕСТА 3.3.2 ===\n";

            // Генерация новой выборки с другим сидом
            vector<double> new_sample = generate_new_sample(n, mu1, la1, v1, mu2, la2, v2, p);

            // Обработка новой выборки
            process_sample(new_sample, n, mu, la, v, mu1, la1, v1, mu2, la2, v2, p, true);

            // Сравнение эмпирических характеристик двух выборок
            std::cout << "  СРАВНЕНИЕ ВЫБОРОК РАЗМЕРОМ 1000:" << std::endl;

            // Вычисляем моменты для классической выборки
            double Mx1 = 0, D2x1 = 0;
            for (double x : sample) Mx1 += x;
            Mx1 /= sample.size();
            for (double x : sample) D2x1 += (x - Mx1) * (x - Mx1);
            D2x1 /= sample.size();

            // Вычисляем моменты для новой выборки  
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

        // Очистка выборки для следующей итерации
        sample.clear();
    }

    return 0;
}
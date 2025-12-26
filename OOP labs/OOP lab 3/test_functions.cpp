#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>
#include <cmath>
#include "tests.h"

using namespace std;

// Тестовая таблица с эталонными значениями для разных v
const TableRow TAB[] = {
    {0.10, 0.201, 2.902, 1.208},
    {0.25, 0.335, 1.437, 0.834},
    {0.50, 0.461, 0.794, 0.661},
    {1.00, 0.596, 0.405, 0.550},
    {2.00, 0.723, 0.186, 0.483},
    {3.00, 0.786, 0.112, 0.458},
    {4.00, 0.825, 0.076, 0.444},
    {5.00, 0.852, 0.055, 0.436},
    {10.0, 0.916, 0.019, 0.418}
};

// Сверяет реализацию с эталоном
void test_against_table() {
    cout << "Тесты стандартного распределения (mu = 0, la = 1)\n";
    cout << "v\t\t sigma2(thr)\t\t sigma2(calc)\t gamma2(thr)\t\t gamma2(calc)\t f0(thr)\t f0(calc)\n";
    cout.setf(ios::fixed);
    cout << setprecision(6);

    for (TableRow t : TAB) {
        SENDistribution d_std(0.0, 1.0, t.v);
        Moments mm = d_std.moments();
        double s2 = mm.D2;
        double g2 = mm.g2;
        double f0 = d_std.pdf(0.0);

        cout << t.v << "\t " << t.sigma2 << "\t\t " << s2
            << "\t " << t.gamma2 << "\t\t " << g2
            << "\t " << t.f0 << "\t " << f0 << "\n";
    }
    cout << "==========================================================\n";
}

// Запускает серию «обязательных» тестов из первой лабораторной работы
void run_required_tests() {
    cout << "\n3.1. Масштаб/сдвиг\n";
    double v = 1.0;

    {
        double mu = 0, la = 1;
        cout << "3.1.1: mu=0, la=1, v=1\n";
        SENDistribution d(mu, la, v);
        Moments m = d.moments();
        cout << "M=" << mu << ", D=" << m.D2 << ", g1=0, g2=" << m.g2
            << ", f(0)=" << d.pdf(0) << "\n\n";
    }

    {
        double mu = 0, la = 2;
        cout << "3.1.2: mu=0, la=2, v=1\n";
        SENDistribution d(mu, la, v);
        Moments m = d.moments();
        cout << "M=" << mu << ", D=" << m.D2 << ", g1=0, g2=" << m.g2
            << ", f(0)=" << d.pdf(0) << "\n\n";
    }

    {
        double mu = 1.5, la = 2;
        cout << "3.1.3: mu=1.5, la=2, v=1\n";
        SENDistribution d(mu, la, v);
        Moments m = d.moments();
        cout << "M=" << mu << ", D=" << m.D2 << ", g1=0, g2=" << m.g2
            << ", f(0)=" << d.pdf(mu) << "\n";
    }

    cout << "\n==========================================================\n";
    cout << "\n3.2. Смеси\n";
    double vv = 1.0;

    {
        double mu = 1.7, la = 2, p = 0.37;
        SENDistribution d1(mu, la, vv);
        SENDistribution d2(mu, la, vv);
        MixtureDistribution mix(d1, d2, p);
        Moments mm = mix.moments();
        cout << "3.2.1: mu1=mu2=1.7, la1=la2=2, v1=v2=1, p=0.37\n";
        cout << "M=" << mm.M << ", D=" << mm.D2 << ", g1=" << mm.g1 << ", g2=" << mm.g2 << "\n\n";
    }

    {
        double mu1 = 0, mu2 = 2, la1 = 1, la2 = 1, p = 0.75;
        SENDistribution d1(mu1, la1, vv);
        SENDistribution d2(mu2, la2, vv);
        MixtureDistribution mix(d1, d2, p);
        Moments mm = mix.moments();
        cout << "3.2.2: mu1=0, mu2=2, la1=la2=1, v1=v2=1, p=0.75\n";
        cout << "M=" << mm.M << ", D=" << mm.D2 << ", g1=" << mm.g1 << ", g2=" << mm.g2 << "\n\n";
    }

    {
        double mu1 = 0, mu2 = 0, la1 = 1, la2 = 3, p = 0.5;
        SENDistribution d1(mu1, la1, vv);
        SENDistribution d2(mu2, la2, vv);
        MixtureDistribution mix(d1, d2, p);
        Moments mm = mix.moments();
        cout << "3.2.3: mu1=mu2=0, la1=1, la2=3, v1=v2=1, p=0.5\n";
        cout << "M=" << mm.M << ", D=" << mm.D2 << ", g1=" << mm.g1 << ", г2=" << mm.g2 << "\n\n";
    }

    {
        double mu1 = 0, mu2 = 0, la1 = 1, la2 = 1, v1 = 0.5, v2 = 2.0, p = 0.5;
        SENDistribution d1(mu1, la1, v1);
        SENDistribution d2(mu2, la2, v2);
        MixtureDistribution mix(d1, d2, p);
        Moments mm = mix.moments();
        cout << "3.2.4: mu1=mu2=0, la1=la2=1, v1=0.5, v2=2, p=0.5\n";
        cout << "M=" << mm.M << ", D=" << mm.D2 << ", g1=" << mm.g1 << ", г2=" << mm.g2 << "\n";
    }

    cout << "\n==========================================================\n";
}

// Комплексное тестирование всех возможностей класса SENDistribution
void test_sen_class() {
    using std::cout;
    using std::endl;

    cout << "\n=== TEST: SENDistribution class ===\n";
    cout.setf(ios::fixed);
    cout << setprecision(6);

    // 1) Конструктор по умолчанию + базовые вычисления
    try {
        SENDistribution d0; // mu=0, la=1, v=1
        double x = d0.mu();
        Moments m = d0.moments();
        cout << "\n[Default distribution]\n"
            << "mu=" << d0.mu() << "  lambda=" << d0.lambda() << "  v=" << d0.v() << '\n'
            << "pdf(mu)=" << d0.pdf(x) << '\n'
            << "M=" << m.M << "  D=" << m.D2 << "  g1=" << m.g1 << "  g2=" << m.g2 << '\n';
    }
    catch (const std::exception& e) {
        cout << "[Default distribution] exception: " << e.what() << '\n';
    }

    // 2) Параметрический конструктор + базовые вычисления
    SENDistribution d(0.0, 2.0, 1.0);
    try {
        double x = d.mu();
        Moments m = d.moments();
        cout << "\n[Param distribution]\n"
            << "mu=" << d.mu() << "  lambda=" << d.lambda() << "  v=" << d.v() << '\n'
            << "pdf(mu)=" << d.pdf(x) << '\n'
            << "M=" << m.M << "  D=" << m.D2 << "  g1=" << m.g1 << "  g2=" << m.g2 << '\n';
    }
    catch (const std::exception& e) {
        cout << "[Param distribution] exception: " << e.what() << '\n';
    }

    // 3) Персистентность (save → load)
    try {
        const char* path = "sen_params.txt";
        d.save(path);
        auto d2 = SENDistribution::load(path);
        cout << "\n[Persistence]\n"
            << "reloaded: mu=" << d2.mu() << "  lambda=" << d2.lambda() << "  v=" << d2.v() << '\n';
        Moments m1 = d2.moments();
        cout << "\n[Before setters]\n"
            << "mu=" << d2.mu() << "  lambda=" << d2.lambda() << "  v=" << d2.v() << '\n'
            << "pdf(mu)=" << d2.pdf(d2.mu()) << '\n'
            << "M=" << m1.M << "  D=" << m1.D2 << "  g1=" << m1.g1 << "  g2=" << m1.g2 << '\n';
    }
    catch (const std::exception& e) {
        cout << "[Persistence] exception: " << e.what() << '\n';
    }

    // 4) Setters + проверка pdf/moments после изменения
    try {
        SENDistribution d2 = d;
        d2.set_mu(1.5);
        d2.set_lambda(2.5);
        d2.set_v(3.0);
        Moments m2 = d2.moments();
        cout << "\n[After setters]\n"
            << "mu=" << d2.mu() << "  lambda=" << d2.lambda() << "  v=" << d2.v() << '\n'
            << "pdf(mu)=" << d2.pdf(d2.mu()) << '\n'
            << "M=" << m2.M << "  D=" << m2.D2 << "  g1=" << m2.g1 << "  g2=" << m2.g2 << '\n';
    }
    catch (const std::exception& e) {
        cout << "[After setters] exception: " << e.what() << '\n';
    }

    // 5) Негативные тесты (ожидаемые исключения)
    try {
        SENDistribution d2 = d;
        d2.set_lambda(0.0);
        cout << "\n[Negative lambda] ERROR: исключение не получено\n";
    }
    catch (const std::exception& e) {
        cout << "\n[Negative lambda] OK: " << e.what() << '\n';
    }

    try {
        SENDistribution bad(0.0, 1.0, -1.0);
        cout << "[Negative v] ERROR: исключение не получено\n";
    }
    catch (const std::exception& e) {
        cout << "[Negative v] OK: " << e.what() << "\n";
    }

    cout << "=== END TEST ===\n";
}

// Комплексное тестирование всех возможностей классов MixtureDistribution и EmpiricalDistribution
void test_new_classes() {
    cout << "\n=== ТЕСТИРОВАНИЕ НОВЫХ КЛАССОВ ===\n";

    // 1. Тест MixtureDistribution с новыми сеттерами
    cout << "\n--- 1. Тест MixtureDistribution (с сеттерами) ---\n";
    try {
        SENDistribution dist1(0.0, 1.0, 1.0);
        SENDistribution dist2(2.0, 1.5, 2.0);
        MixtureDistribution mix(dist1, dist2, 0.4);

        cout << "Исходная смесь:\n";
        cout << "Компонент 1: mu=" << mix.first_component().mu() << ", lambda=" << mix.first_component().lambda() << ", v=" << mix.first_component().v() << "\n";
        cout << "Компонент 2: mu=" << mix.second_component().mu() << ", lambda=" << mix.second_component().lambda() << ", v=" << mix.second_component().v() << "\n";
        cout << "Параметр смеси p=" << mix.mixture_parameter() << "\n";

        // Тестирование сеттеров
        SENDistribution new_dist1(1.0, 2.0, 0.5);
        SENDistribution new_dist2(3.0, 1.0, 1.5);

        mix.set_first_component(new_dist1);
        mix.set_second_component(new_dist2);
        mix.set_mixture_parameter(0.7);

        cout << "\nПосле изменения через сеттеры:\n";
        cout << "Компонент 1: mu=" << mix.first_component().mu() << ", lambda=" << mix.first_component().lambda() << ", v=" << mix.first_component().v() << "\n";
        cout << "Компонент 2: mu=" << mix.second_component().mu() << ", lambda=" << mix.second_component().lambda() << ", v=" << mix.second_component().v() << "\n";
        cout << "Параметр смеси p=" << mix.mixture_parameter() << "\n";

        double x = 1.0;
        cout << "PDF(" << x << ") = " << mix.pdf(x) << "\n";

        Moments m = mix.moments();
        cout << "Моменты смеси: M=" << m.M << ", D=" << m.D2 << ", g1=" << m.g1 << ", g2=" << m.g2 << "\n";

        cout << "Сгенерированный сэмпл: " << mix.sample() << "\n";

        mix.save("test_mixture.txt");
        auto loaded_mix = MixtureDistribution::load("test_mixture.txt");
        cout << "Загруженная смесь: p=" << loaded_mix.mixture_parameter() << "\n";
    }
    catch (const std::exception& e) {
        cout << "Ошибка: " << e.what() << "\n";
    }

    // 2. Тест EmpiricalDistribution с улучшенным управлением памятью
    cout << "\n--- 2. Тест EmpiricalDistribution (с улучшенным управлением памятью) ---\n";
    try {
        // Создаем эмпирическое распределение из смеси
        SENDistribution dist1(0.0, 1.0, 1.0);
        SENDistribution dist2(2.0, 1.5, 2.0);
        MixtureDistribution mix(dist1, dist2, 0.4);
        EmpiricalDistribution emp_from_mix(mix, 1000); // 1000 сэмплов

        cout << "Размер выборки: " << emp_from_mix.get_sample().size() << "\n";
        cout << "Число бинов: " << emp_from_mix.get_bins_count() << "\n";

        double x = 1.0;
        cout << "Empirical PDF(" << x << ") = " << emp_from_mix.pdf(x) << "\n";

        Moments m_emp = emp_from_mix.moments();
        cout << "Эмпирические моменты: M=" << m_emp.M << ", D=" << m_emp.D2 << ", g1=" << m_emp.g1 << ", g2=" << m_emp.g2 << "\n";

        cout << "Случайный сэмпл из эмпирической выборки: " << emp_from_mix.sample() << "\n";

        emp_from_mix.save("test_empirical.txt");
        auto loaded_emp = EmpiricalDistribution::load("test_empirical.txt");
        cout << "Загруженная эмпирическая выборка, размер: " << loaded_emp.get_sample().size() << "\n";

        // Тест конструктора копирования и присваивания
        EmpiricalDistribution copy = emp_from_mix;
        cout << "Конструктор копирования: размер = " << copy.get_sample().size() << "\n";

        EmpiricalDistribution assigned;
        assigned = emp_from_mix;
        cout << "Оператор присваивания: размер = " << assigned.get_sample().size() << "\n";

        // Тест управления памятью через множественные присваивания
        cout << "Тест управления памятью (множественные присваивания):\n";
        for (int i = 0; i < 5; ++i) {
            EmpiricalDistribution temp = emp_from_mix;
            assigned = temp;
            cout << "Цикл " << i + 1 << ": OK\n";
        }
    }
    catch (const std::exception& e) {
        cout << "Ошибка: " << e.what() << "\n";
    }

    cout << "\n=== ТЕСТИРОВАНИЕ ЗАВЕРШЕНО ===\n";
}
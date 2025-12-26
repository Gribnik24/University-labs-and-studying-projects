#include <iostream>
#include <iomanip>
#include <fstream>
#include <cmath>

#include "tests.h"
#include "sen_distribution.h"
#include "empirical_distribution.h"
#include "mixture_distribution.h"

using namespace std;

using SenMixture = MixtureDistribution<SENDistribution, SENDistribution>;

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

void test_against_table() {
    cout << "Тесты стандартного распределения (mu = 0, la = 1)\n";
    cout << "v\t\t sigma2(thr)\t\t sigma2(calc)\t gamma2(thr)\t\t gamma2(calc)\t f0(thr)\t f0(calc)\n";
    cout.setf(ios::fixed);
    cout << setprecision(6);

    for (const TableRow& t : TAB) {
        try {
            SENDistribution d_std(0.0, 1.0, t.v);
            Moments mm = d_std.moments();
            double s2 = mm.D2;
            double g2 = mm.g2;
            double f0 = d_std.pdf(0.0);

            cout << t.v << "\t " << t.sigma2 << "\t\t " << s2
                << "\t " << t.gamma2 << "\t\t " << g2
                << "\t " << t.f0 << "\t " << f0 << "\n";
        }
        catch (const std::exception& e) {
            cout << t.v << "\t [ОШИБКА] test_against_table: " << e.what() << "\n";
        }
    }
    cout << "==========================================================\n";
}

void run_required_tests() {
    cout << "\n3.1. Масштаб/сдвиг\n";
    double v = 1.0;

    // 3.1.1
    {
        double mu = 0, la = -1;
        cout << "3.1.1: mu=0, la=-1, v=1\n";
        try {
            SENDistribution d(mu, la, v);
            Moments m = d.moments();
            cout << "M=" << mu << ", D=" << m.D2 << ", g1=0, g2=" << m.g2
                << ", f(0)=" << d.pdf(0) << "\n\n";
        }
        catch (const std::exception& e) {
            cout << "  [ОШИБКА] run_required_tests 3.1.1: " << e.what() << "\n\n";
        }
    }

    // 3.1.2
    {
        double mu = 0, la = 2;
        cout << "3.1.2: mu=0, la=2, v=1\n";
        try {
            SENDistribution d(mu, la, v);
            Moments m = d.moments();
            cout << "M=" << mu << ", D=" << m.D2 << ", g1=0, g2=" << m.g2
                << ", f(0)=" << d.pdf(0) << "\n\n";
        }
        catch (const std::exception& e) {
            cout << "  [ОШИБКА] run_required_tests 3.1.2: " << e.what() << "\n\n";
        }
    }

    // 3.1.3
    {
        double mu = 1.5, la = 2;
        cout << "3.1.3: mu=1.5, la=2, v=1\n";
        try {
            SENDistribution d(mu, la, v);
            Moments m = d.moments();
            cout << "M=" << mu << ", D=" << m.D2 << ", g1=0, g2=" << m.g2
                << ", f(0)=" << d.pdf(mu) << "\n\n";
        }
        catch (const std::exception& e) {
            cout << "  [ОШИБКА] run_required_tests 3.1.3: " << e.what() << "\n\n";
        }
    }

    cout << "\n==========================================================\n";
    cout << "\n3.2. Смеси\n";
    double vv = 1.0;

    // 3.2.1
    {
        double mu = 1.7, la = 2, p = 0.37;
        cout << "3.2.1: mu1=mu2=1.7, la1=la2=2, v1=v2=1, p=0.37\n";
        try {
            SENDistribution d1(mu, la, vv);
            SENDistribution d2(mu, la, vv);
            SenMixture mix(d1, d2, p);
            Moments mm = mix.moments();
            cout << "M=" << mm.M << ", D=" << mm.D2
                << ", g1=" << mm.g1 << ", g2=" << mm.g2 << "\n\n";
        }
        catch (const std::exception& e) {
            cout << "  [ОК, ПОЛУЧЕНА ОШИБКА] run_required_tests 3.2.1: " << e.what() << "\n\n";
        }
    }

    // 3.2.2
    {
        double mu1 = 0, mu2 = 2, la1 = 1, la2 = 1, p = 0.75;
        cout << "3.2.2: mu1=0, mu2=2, la1=la2=1, v1=v2=1, p=0.75\n";
        try {
            SENDistribution d1(mu1, la1, vv);
            SENDistribution d2(mu2, la2, vv);
            SenMixture mix(d1, d2, p);
            Moments mm = mix.moments();
            cout << "M=" << mm.M << ", D=" << mm.D2
                << ", g1=" << mm.g1 << ", g2=" << mm.g2 << "\n\n";
        }
        catch (const std::exception& e) {
            cout << "  [ОШИБКА] run_required_tests 3.2.2: " << e.what() << "\n\n";
        }
    }

    // 3.2.3
    {
        double mu1 = 0, mu2 = 0, la1 = 1, la2 = 3, p = 0.5;
        cout << "3.2.3: mu1=mu2=0, la1=1, la2=3, v1=v2=1, p=0.5\n";
        try {
            SENDistribution d1(mu1, la1, vv);
            SENDistribution d2(mu2, la2, vv);
            SenMixture mix(d1, d2, p);
            Moments mm = mix.moments();
            cout << "M=" << mm.M << ", D=" << mm.D2
                << ", g1=" << mm.g1 << ", g2=" << mm.g2 << "\n\n";
        }
        catch (const std::exception& e) {
            cout << "  [ОШИБКА] run_required_tests 3.2.3: " << e.what() << "\n\n";
        }
    }

    // 3.2.4
    {
        double mu1 = 0, mu2 = 0, la1 = 1, la2 = 1, v1 = 0.5, v2 = 2.0, p = 0.5;
        cout << "3.2.4: mu1=mu2=0, la1=la2=1, v1=0.5, v2=2, p=0.5\n";
        try {
            SENDistribution d1(mu1, la1, v1);
            SENDistribution d2(mu2, la2, v2);
            SenMixture mix(d1, d2, p);
            Moments mm = mix.moments();
            cout << "M=" << mm.M << ", D=" << mm.D2
                << ", g1=" << mm.g1 << ", g2=" << mm.g2 << "\n";
        }
        catch (const std::exception& e) {
            cout << "  [ОШИБКА] run_required_tests 3.2.4: " << e.what() << "\n";
        }
    }

    cout << "\n==========================================================\n";
}



// тестирование класса SENDistribution
void test_sen_class() {
    using std::cout;
    using std::endl;

    cout << "\n=== TEST: SENDistribution class ===\n";
    cout.setf(ios::fixed);
    cout << setprecision(6);

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
        cout << "[ОШИБКА] test_sen_class default ctor: " << e.what() << '\n';
    }

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
        cout << "[ОШИБКА] test_sen_class param ctor: " << e.what() << '\n';
    }

    try {
        const char* path = "sen_params.txt";
        save_sen_to_file(d, path);
        SENDistribution d2 = load_sen_from_file(path);
        cout << "\n[Persistence]\n"
            << "reloaded: mu=" << d2.mu() << "  lambda=" << d2.lambda() << "  v=" << d2.v() << '\n';
        Moments m1 = d2.moments();
        cout << "\n[Before setters]\n"
            << "mu=" << d2.mu() << "  lambda=" << d2.lambda() << "  v=" << d2.v() << '\n'
            << "pdf(mu)=" << d2.pdf(d2.mu()) << '\n'
            << "M=" << m1.M << "  D=" << m1.D2 << "  g1=" << m1.g1 << "  g2=" << m1.g2 << '\n';
    }
    catch (const std::exception& e) {
        cout << "[ОШИБКА] test_sen_class persistence: " << e.what() << '\n';
    }

    // 4) Setters
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
        cout << "[ОШИБКА] test_sen_class setters: " << e.what() << '\n';
    }

    try {
        SENDistribution d2 = d;
        d2.set_lambda(0.0);
        cout << "\n[Negative lambda] ОШИБКА: исключение не получено\n";
    }
    catch (const std::exception& e) {
        cout << "\n[Negative lambda] OK: " << e.what() << '\n';
    }

    try {
        SENDistribution bad(0.0, 1.0, -1.0);
        cout << "[Negative v] ОШИБКА: исключение не получено\n";
    }
    catch (const std::exception& e) {
        cout << "[Negative v] OK: " << e.what() << "\n";
    }

    cout << "=== END TEST ===\n";
}



// тестирование классов MixtureDistribution и EmpiricalDistribution
void test_new_classes() {
    cout << "\n=== ТЕСТИРОВАНИЕ НОВЫХ КЛАССОВ ===\n";

    cout << "\n--- 1. Тест MixtureDistribution (с сеттерами) ---\n";
    try {
        SENDistribution dist1(0.0, 1.0, 1.0);
        SENDistribution dist2(2.0, 1.5, 2.0);
        SenMixture mix(dist1, dist2, 0.4);

        cout << "Исходная смесь:\n";
        cout << "Компонент 1: mu=" << mix.first_component().mu()
            << ", lambda=" << mix.first_component().lambda()
            << ", v=" << mix.first_component().v() << "\n";
        cout << "Компонент 2: mu=" << mix.second_component().mu()
            << ", lambda=" << mix.second_component().lambda()
            << ", v=" << mix.second_component().v() << "\n";
        cout << "Параметр смеси p=" << mix.mixture_parameter() << "\n";

        SENDistribution new_dist1(1.0, 2.0, 0.5);
        SENDistribution new_dist2(3.0, 1.0, 1.5);

        mix.set_first_component(new_dist1);
        mix.set_second_component(new_dist2);
        mix.set_mixture_parameter(0.7);

        cout << "\nПосле изменения через сеттеры:\n";
        cout << "Компонент 1: mu=" << mix.first_component().mu()
            << ", lambda=" << mix.first_component().lambda()
            << ", v=" << mix.first_component().v() << "\n";
        cout << "Компонент 2: mu=" << mix.second_component().mu()
            << ", lambda=" << mix.second_component().lambda()
            << ", v=" << mix.second_component().v() << "\n";
        cout << "Параметр смеси p=" << mix.mixture_parameter() << "\n";

        double x = 1.0;
        cout << "PDF(" << x << ") = " << mix.pdf(x) << "\n";

        Moments m = mix.moments();
        cout << "Моменты смеси: M=" << m.M << ", D=" << m.D2
            << ", g1=" << m.g1 << ", g2=" << m.g2 << "\n";

        cout << "Сгенерированный сэмпл: " << mix.sample() << "\n";

        const char* mix_path = "test_mixture.txt";
        {
            ofstream out(mix_path);
            if (!out) throw runtime_error("cannot open test_mixture.txt for write");
            mix.save(out);
        }

        SenMixture loaded(dist1, dist2, 0.5);
        {
            ifstream in(mix_path);
            if (!in) throw runtime_error("cannot open test_mixture.txt for read");
            loaded.load(in);
        }
        cout << "Загруженная смесь: p=" << loaded.mixture_parameter() << "\n";
    }
    catch (const std::exception& e) {
        cout << "[ОШИБКА] test_new_classes (MixtureDistribution): " << e.what() << "\n";
    }

    cout << "\n--- 2. Тест EmpiricalDistribution (через интерфейс распределения) ---\n";
    try {
        SENDistribution dist1(0.0, 1.0, 1.0);
        SENDistribution dist2(2.0, 1.5, 2.0);
        SenMixture mix(dist1, dist2, 0.4);

        EmpiricalDistribution emp_from_mix(mix, 1000);

        cout << "Размер выборки: " << emp_from_mix.size() << "\n";
        cout << "Число бинов: " << emp_from_mix.get_bins_count() << "\n";

        double x = 1.0;
        cout << "Empirical PDF(" << x << ") = " << emp_from_mix.pdf(x) << "\n";

        Moments m_emp = emp_from_mix.moments();
        cout << "Эмпирические моменты: M=" << m_emp.M
            << ", D=" << m_emp.D2
            << ", g1=" << m_emp.g1
            << ", g2=" << m_emp.g2 << "\n";

        cout << "Случайный сэмпл из эмпирической выборки: "
            << emp_from_mix.sample() << "\n";

        const char* emp_path = "test_empirical.txt";
        save_empirical_to_file(emp_from_mix, emp_path);
        auto loaded_emp = load_empirical_from_file("test_empirical.txt");
        cout << "Загруженная эмпирическая выборка, размер: " << loaded_emp.size() << "\n";

        EmpiricalDistribution copy = emp_from_mix;
        cout << "Конструктор копирования: размер = " << copy.size() << "\n";

        EmpiricalDistribution assigned;
        assigned = emp_from_mix;
        cout << "Оператор присваивания: размер = " << assigned.size() << "\n";

        cout << "Тест управления памятью (множественные присваивания):\n";
        for (int i = 0; i < 5; ++i) {
            EmpiricalDistribution temp = emp_from_mix;
            assigned = temp;
            cout << "Цикл " << i + 1 << ": OK\n";
        }
    }
    catch (const std::exception& e) {
        cout << "[ОШИБКА] test_new_classes (EmpiricalDistribution): " << e.what() << "\n";
    }

    cout << "\n=== ТЕСТИРОВАНИЕ ЗАВЕРШЕНО ===\n";
}



//тесты последней лабораторной
void test_polymorphism() {
    cout << "\n=== TEST: polymorphic behavior via IDistribution/IPersistent ===\n";
    cout.setf(ios::fixed);
    cout << setprecision(6);

    try {
        SENDistribution sen(0.0, 1.0, 1.0);
        SENDistribution sen1(-1.0, 0.5, 1.0);
        SENDistribution sen2(2.0, 1.5, 2.0);
        SenMixture mix(sen1, sen2, 0.4);

        EmpiricalDistribution emp_from_sen(sen, 1000);
        EmpiricalDistribution emp_from_mix(mix, 1000);

        struct DistItem {
            std::string name;
            const IDistribution* dist;
        };

        std::vector<DistItem> dists = {
            {"SEN(0,1,1)",          &sen},
            {"Mixture(SEN1,SEN2)",  &mix},
            {"Empirical(SEN)",      &emp_from_sen},
            {"Empirical(Mixture)",  &emp_from_mix}
        };

        double x = 0.5;
        cout << "\n--- Полиморфные вызовы через IDistribution* ---\n";
        for (const auto& it : dists) {
            Moments m = it.dist->moments();
            double f = it.dist->pdf(x);
            double s = it.dist->sample();

            cout << "[" << it.name << "]\n";
            cout << "  pdf(" << x << ") = " << f << "\n";
            cout << "  M=" << m.M << ", D=" << m.D2
                << ", g1=" << m.g1 << ", g2=" << m.g2 << "\n";
            cout << "  sample() = " << s << "\n\n";
        }

        cout << "--- Полиморфная персистентность через IPersistent* ---\n";

        struct PersItem {
            std::string name;
            const IPersistent* obj;
        };

        std::vector<PersItem> pers = {
            {"SEN(0,1,1)",         &sen},
            {"Mixture(SEN1,SEN2)", &mix},
            {"Empirical(SEN)",     &emp_from_sen},
            {"Empirical(Mixture)", &emp_from_mix}
        };

        const char* path = "polymorphic_save.txt";
        try {
            std::ofstream out(path);
            if (!out) {
                throw std::runtime_error("cannot open polymorphic_save.txt for write");
            }
            for (const auto& it : pers) {
                out << "# " << it.name << "\n";
                it.obj->save(out);
            }
            cout << "Все объекты записаны в файл " << path << " через IPersistent*.\n";
        }
        catch (const std::exception& e) {
            cout << "  [ОШИБКА] test_polymorphism save: " << e.what() << "\n";
        }

        cout << "=== ТЕСТИРОВАНИЕ ЗАВЕРШЕНО ===\n";
    }
    catch (const std::exception& e) {
        cout << "[ОШИБКА] test_polymorphism: " << e.what() << "\n";
    }
}

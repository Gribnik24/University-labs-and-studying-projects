#include <iostream>
#include <iomanip>
#include <fstream>
#include <cmath>
#include <vector>

#include "tests.h"
#include "sen_distribution.h"
#include "empirical_distribution.h"
#include "mixture_distribution.h"
#include "estimate.h"

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

// Лабораторная работа #1 - Теоретическая таблица SEN-распределения
void test_against_table() {
    std::cout << "Тесты стандартного распределения (mu = 0, la = 1)\n";
    std::cout << "v\t\t sigma2(thr)\t\t sigma2(calc)\t gamma2(thr)\t\t gamma2(calc)\t f0(thr)\t f0(calc)\n";
    std::cout.setf(ios::fixed);
    std::cout << setprecision(6);

    for (const TableRow& t : TAB) {
        try {
            SENDistribution d_std(0.0, 1.0, t.v);
            Moments mm = d_std.moments();
            double s2 = mm.D2;
            double g2 = mm.g2;
            double f0 = d_std.pdf(0.0);

            std::cout << t.v << "\t " << t.sigma2 << "\t\t " << s2
                << "\t " << t.gamma2 << "\t\t " << g2
                << "\t " << t.f0 << "\t " << f0 << "\n";
        }
        catch (const std::exception& e) {
            std::cout << t.v << "\t [ОШИБКА] test_against_table: " << e.what() << "\n";
        }
    }
    std::cout << "==========================================================\n";
}

// Лабораторная работа #1 - Набор тестов 3.1-3.3
void run_required_tests() {
    std::cout << "\n3.1. Масштаб/сдвиг\n";
    double v = 1.0;

    // 3.1.1
    {
        double mu = 0, la = 1;
        std::cout << "3.1.1: mu=0, la=1, v=1\n";
        try {
            SENDistribution d(mu, la, v);
            Moments m = d.moments();
            std::cout << "M=" << mu << ", D=" << m.D2 << ", g1=0, g2=" << m.g2
                << ", f(0)=" << d.pdf(0) << "\n\n";
        }
        catch (const std::exception& e) {
            std::cout << "  [ОШИБКА] run_required_tests 3.1.1: " << e.what() << "\n\n";
        }
    }

    // 3.1.2
    {
        double mu = 0, la = 2;
        std::cout << "3.1.2: mu=0, la=2, v=1\n";
        try {
            SENDistribution d(mu, la, v);
            Moments m = d.moments();
            std::cout << "M=" << mu << ", D=" << m.D2 << ", g1=0, g2=" << m.g2
                << ", f(0)=" << d.pdf(0) << "\n\n";
        }
        catch (const std::exception& e) {
            std::cout << "  [ОШИБКА] run_required_tests 3.1.2: " << e.what() << "\n\n";
        }
    }

    // 3.1.3
    {
        double mu = 1.5, la = 2;
        std::cout << "3.1.3: mu=1.5, la=2, v=1\n";
        try {
            SENDistribution d(mu, la, v);
            Moments m = d.moments();
            std::cout << "M=" << mu << ", D=" << m.D2 << ", g1=0, g2=" << m.g2
                << ", f(0)=" << d.pdf(mu) << "\n\n";
        }
        catch (const std::exception& e) {
            std::cout << "  [ОШИБКА] run_required_tests 3.1.3: " << e.what() << "\n\n";
        }
    }

    std::cout << "\n==========================================================\n";
    std::cout << "\n3.2. Смеси\n";
    double vv = 1.0;

    // 3.2.1
    {
        double mu = 1.7, la = 2, p = 0.37;
        std::cout << "3.2.1: mu1=mu2=1.7, la1=la2=2, v1=v2=1, p=0.37\n";
        try {
            SENDistribution d1(mu, la, vv);
            SENDistribution d2(mu, la, vv);
            SenMixture mix(d1, d2, p);
            Moments mm = mix.moments();
            std::cout << "M=" << mm.M << ", D=" << mm.D2
                << ", g1=" << mm.g1 << ", g2=" << mm.g2 << "\n\n";
        }
        catch (const std::exception& e) {
            std::cout << "  [ОК, ПОЛУЧЕНА ОШИБКА] run_required_tests 3.2.1: " << e.what() << "\n\n";
        }
    }

    // 3.2.2
    {
        double mu1 = 0, mu2 = 2, la1 = 1, la2 = 1, p = 0.75;
        std::cout << "3.2.2: mu1=0, mu2=2, la1=la2=1, v1=v2=1, p=0.75\n";
        try {
            SENDistribution d1(mu1, la1, vv);
            SENDistribution d2(mu2, la2, vv);
            SenMixture mix(d1, d2, p);
            Moments mm = mix.moments();
            std::cout << "M=" << mm.M << ", D=" << mm.D2
                << ", g1=" << mm.g1 << ", g2=" << mm.g2 << "\n\n";
        }
        catch (const std::exception& e) {
            std::cout << "  [ОШИБКА] run_required_tests 3.2.2: " << e.what() << "\n\n";
        }
    }

    // 3.2.3
    {
        double mu1 = 0, mu2 = 0, la1 = 1, la2 = 3, p = 0.5;
        std::cout << "3.2.3: mu1=mu2=0, la1=1, la2=3, v1=v2=1, p=0.5\n";
        try {
            SENDistribution d1(mu1, la1, vv);
            SENDistribution d2(mu2, la2, vv);
            SenMixture mix(d1, d2, p);
            Moments mm = mix.moments();
            std::cout << "M=" << mm.M << ", D=" << mm.D2
                << ", g1=" << mm.g1 << ", g2=" << mm.g2 << "\n\n";
        }
        catch (const std::exception& e) {
            std::cout << "  [ОШИБКА] run_required_tests 3.2.3: " << e.what() << "\n\n";
        }
    }

    // 3.2.4
    {
        double mu1 = 0, mu2 = 0, la1 = 1, la2 = 1, v1 = 0.5, v2 = 2.0, p = 0.5;
        std::cout << "3.2.4: mu1=mu2=0, la1=la2=1, v1=0.5, v2=2, p=0.5\n";
        try {
            SENDistribution d1(mu1, la1, v1);
            SENDistribution d2(mu2, la2, v2);
            SenMixture mix(d1, d2, p);
            Moments mm = mix.moments();
            std::cout << "M=" << mm.M << ", D=" << mm.D2
                << ", g1=" << mm.g1 << ", g2=" << mm.g2 << "\n";
        }
        catch (const std::exception& e) {
            std::cout << "  [ОШИБКА] run_required_tests 3.2.4: " << e.what() << "\n";
        }
    }

    std::cout << "\n==========================================================\n";
}

// Лабораторная работа #2 - Тесты для класса SEN-распределения
void test_sen_class() {
    using std::cout;
    using std::endl;

    std::cout << "\n=== TEST: SENDistribution class ===\n";
    std::cout.setf(ios::fixed);
    std::cout << setprecision(6);

    try {
        SENDistribution d0; // mu=0, la=1, v=1
        double x = d0.mu();
        Moments m = d0.moments();
        std::cout << "\n[Default distribution]\n"
            << "mu=" << d0.mu() << "  lambda=" << d0.lambda() << "  v=" << d0.v() << '\n'
            << "pdf(mu)=" << d0.pdf(x) << '\n'
            << "M=" << m.M << "  D=" << m.D2 << "  g1=" << m.g1 << "  g2=" << m.g2 << '\n';
    }
    catch (const std::exception& e) {
        std::cout << "[ОШИБКА] test_sen_class default ctor: " << e.what() << '\n';
    }

    SENDistribution d(0.0, 2.0, 1.0);
    try {
        double x = d.mu();
        Moments m = d.moments();
        std::cout << "\n[Param distribution]\n"
            << "mu=" << d.mu() << "  lambda=" << d.lambda() << "  v=" << d.v() << '\n'
            << "pdf(mu)=" << d.pdf(x) << '\n'
            << "M=" << m.M << "  D=" << m.D2 << "  g1=" << m.g1 << "  g2=" << m.g2 << '\n';
    }
    catch (const std::exception& e) {
        std::cout << "[ОШИБКА] test_sen_class param ctor: " << e.what() << '\n';
    }

    try {
        const char* path = "sen_params.txt";
        save_sen_to_file(d, path);
        SENDistribution d2 = load_sen_from_file(path);
        std::cout << "\n[Persistence]\n"
            << "reloaded: mu=" << d2.mu() << "  lambda=" << d2.lambda() << "  v=" << d2.v() << '\n';
        Moments m1 = d2.moments();
        std::cout << "\n[Before setters]\n"
            << "mu=" << d2.mu() << "  lambda=" << d2.lambda() << "  v=" << d2.v() << '\n'
            << "pdf(mu)=" << d2.pdf(d2.mu()) << '\n'
            << "M=" << m1.M << "  D=" << m1.D2 << "  g1=" << m1.g1 << "  g2=" << m1.g2 << '\n';
    }
    catch (const std::exception& e) {
        std::cout << "[ОШИБКА] test_sen_class persistence: " << e.what() << '\n';
    }

    // 4) Setters
    try {
        SENDistribution d2 = d;
        d2.set_mu(1.5);
        d2.set_lambda(2.5);
        d2.set_v(3.0);
        Moments m2 = d2.moments();
        std::cout << "\n[After setters]\n"
            << "mu=" << d2.mu() << "  lambda=" << d2.lambda() << "  v=" << d2.v() << '\n'
            << "pdf(mu)=" << d2.pdf(d2.mu()) << '\n'
            << "M=" << m2.M << "  D=" << m2.D2 << "  g1=" << m2.g1 << "  g2=" << m2.g2 << '\n';
    }
    catch (const std::exception& e) {
        std::cout << "[ОШИБКА] test_sen_class setters: " << e.what() << '\n';
    }

    try {
        SENDistribution d2 = d;
        d2.set_lambda(0.0);
        std::cout << "\n[Negative lambda] ОШИБКА: исключение не получено\n";
    }
    catch (const std::exception& e) {
        std::cout << "\n[Negative lambda] OK: " << e.what() << '\n';
    }

    try {
        SENDistribution bad(0.0, 1.0, -1.0);
        std::cout << "[Negative v] ОШИБКА: исключение не получено\n";
    }
    catch (const std::exception& e) {
        std::cout << "[Negative v] OK: " << e.what() << "\n";
    }

    std::cout << "=== END TEST ===\n";
}

// Лабораторная работа #3 - Тесты для классов смеси распределений и эмпирического распределения
void test_new_classes() {
    std::cout << "\n=== ТЕСТИРОВАНИЕ НОВЫХ КЛАССОВ ===\n";

    std::cout << "\n--- 1. Тест MixtureDistribution (с сеттерами) ---\n";
    try {
        SENDistribution dist1(0.0, 1.0, 1.0);
        SENDistribution dist2(2.0, 1.5, 2.0);
        SenMixture mix(dist1, dist2, 0.4);

        std::cout << "Исходная смесь:\n";
        std::cout << "Компонент 1: mu=" << mix.first_component().mu()
            << ", lambda=" << mix.first_component().lambda()
            << ", v=" << mix.first_component().v() << "\n";
        std::cout << "Компонент 2: mu=" << mix.second_component().mu()
            << ", lambda=" << mix.second_component().lambda()
            << ", v=" << mix.second_component().v() << "\n";
        std::cout << "Параметр смеси p=" << mix.mixture_parameter() << "\n";

        SENDistribution new_dist1(1.0, 2.0, 0.5);
        SENDistribution new_dist2(3.0, 1.0, 1.5);

        mix.set_first_component(new_dist1);
        mix.set_second_component(new_dist2);
        mix.set_mixture_parameter(0.7);

        std::cout << "\nПосле изменения через сеттеры:\n";
        std::cout << "Компонент 1: mu=" << mix.first_component().mu()
            << ", lambda=" << mix.first_component().lambda()
            << ", v=" << mix.first_component().v() << "\n";
        std::cout << "Компонент 2: mu=" << mix.second_component().mu()
            << ", lambda=" << mix.second_component().lambda()
            << ", v=" << mix.second_component().v() << "\n";
        std::cout << "Параметр смеси p=" << mix.mixture_parameter() << "\n";

        double x = 1.0;
        std::cout << "PDF(" << x << ") = " << mix.pdf(x) << "\n";

        Moments m = mix.moments();
        std::cout << "Моменты смеси: M=" << m.M << ", D=" << m.D2
            << ", g1=" << m.g1 << ", g2=" << m.g2 << "\n";

        std::cout << "Сгенерированный сэмпл: " << mix.sample() << "\n";

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
        std::cout << "Загруженная смесь: p=" << loaded.mixture_parameter() << "\n";
    }
    catch (const std::exception& e) {
        std::cout << "[ОШИБКА] test_new_classes (MixtureDistribution): " << e.what() << "\n";
    }

    std::cout << "\n--- 2. Тест EmpiricalDistribution (через интерфейс распределения) ---\n";
    try {
        SENDistribution dist1(0.0, 1.0, 1.0);
        SENDistribution dist2(2.0, 1.5, 2.0);
        SenMixture mix(dist1, dist2, 0.4);

        EmpiricalDistribution emp_from_mix(mix, 1000);

        std::cout << "Размер выборки: " << emp_from_mix.size() << "\n";
        std::cout << "Число бинов: " << emp_from_mix.get_bins_count() << "\n";

        double x = 1.0;
        std::cout << "Empirical PDF(" << x << ") = " << emp_from_mix.pdf(x) << "\n";

        Moments m_emp = emp_from_mix.moments();
        std::cout << "Эмпирические моменты: M=" << m_emp.M
            << ", D=" << m_emp.D2
            << ", g1=" << m_emp.g1
            << ", g2=" << m_emp.g2 << "\n";

        std::cout << "Случайный сэмпл из эмпирической выборки: "
            << emp_from_mix.sample() << "\n";

        const char* emp_path = "test_empirical.txt";
        save_empirical_to_file(emp_from_mix, emp_path);
        auto loaded_emp = load_empirical_from_file("test_empirical.txt");
        std::cout << "Загруженная эмпирическая выборка, размер: " << loaded_emp.size() << "\n";

        EmpiricalDistribution copy = emp_from_mix;
        std::cout << "Конструктор копирования: размер = " << copy.size() << "\n";

        EmpiricalDistribution assigned;
        assigned = emp_from_mix;
        std::cout << "Оператор присваивания: размер = " << assigned.size() << "\n";

        std::cout << "Тест управления памятью (множественные присваивания):\n";
        for (int i = 0; i < 5; ++i) {
            EmpiricalDistribution temp = emp_from_mix;
            assigned = temp;
            std::cout << "Цикл " << i + 1 << ": OK\n";
        }
    }
    catch (const std::exception& e) {
        std::cout << "[ОШИБКА] test_new_classes (EmpiricalDistribution): " << e.what() << "\n";
    }

    std::cout << "\n=== ТЕСТИРОВАНИЕ ЗАВЕРШЕНО ===\n";
}

// Лабораторная работа #4 - Тесты интерфейсов и виртуальных функций
void test_polymorphism() {
    std::cout << "\n=== TEST: polymorphic behavior via IDistribution/IPersistent ===\n";
    std::cout.setf(ios::fixed);
    std::cout << setprecision(6);

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
        std::cout << "\n--- Полиморфные вызовы через IDistribution* ---\n";
        for (const auto& it : dists) {
            Moments m = it.dist->moments();
            double f = it.dist->pdf(x);
            double s = it.dist->sample();

            std::cout << "[" << it.name << "]\n";
            std::cout << "  pdf(" << x << ") = " << f << "\n";
            std::cout << "  M=" << m.M << ", D=" << m.D2
                << ", g1=" << m.g1 << ", g2=" << m.g2 << "\n";
            std::cout << "  sample() = " << s << "\n\n";
        }

        std::cout << "--- Полиморфная персистентность через IPersistent* ---\n";

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
            std::cout << "Все объекты записаны в файл " << path << " через IPersistent*.\n";
        }
        catch (const std::exception& e) {
            std::cout << "  [ОШИБКА] test_polymorphism save: " << e.what() << "\n";
        }

        std::cout << "=== ТЕСТИРОВАНИЕ ЗАВЕРШЕНО ===\n";
    }
    catch (const std::exception& e) {
        std::cout << "[ОШИБКА] test_polymorphism: " << e.what() << "\n";
    }
}

// Лабораторная работа #5 - Устойчивые оценки (агрегация по ссылке и паттерн "Наблюдатель")
void test_link_aggregation() {
    // ==================== ПАРАМЕТРЫ РАСПРЕДЕЛЕНИЙ ====================
    std::cout << "3.1-3. Параметры распределений и эксперимента:\n";

    // Идеальное распределение (близко к нормальному, v=10)
    double ideal_mu = 0.0;
    double ideal_lambda = 1.0;
    double ideal_v = 10.0;
    std::cout << "  Идеальное распределение: SEN(mu=" << ideal_mu
        << ", lambda=" << ideal_lambda << ", v=" << ideal_v << ")\n";

    // Засоряющее распределение (другой сдвиг и масштаб)
    double contamin_mu = 5.0;    // сильный сдвиг
    double contamin_lambda = 2.0; // больший масштаб
    double contamin_v = 10.0;    // такая же форма
    std::cout << "  Засоряющее распределение: SEN(mu=" << contamin_mu
        << ", lambda=" << contamin_lambda << ", v=" << contamin_v << ")\n\n";

    // ==================== ПАРАМЕТРЫ ЭКСПЕРИМЕНТА ====================
    std::cout << "  Параметры эксперимента:\n";

    // Объем выборки (200-1000)
    int n = 500;
    std::cout << "  Объем выборки n = " << n << "\n";

    // Уровень засорения (10-40%)
    double p_contamination = 0.25; // 25%
    std::cout << "  Уровень засорения p = " << (p_contamination * 100) << "%\n";

    // Параметры функции потерь (вариант 2: Шурыгина)
    std::vector<double> c_values = { 75.0, 25.6, 12.0, 0.0 };
    std::cout << "  Параметры c: ";
    for (size_t i = 0; i < c_values.size(); ++i) {
        std::cout << c_values[i];
        if (i < c_values.size() - 1) std::cout << ", ";
    }
    std::cout << "\n\n";

    // Количество повторений (несколько выборок)
    int num_samples = 3;
    std::cout << "3.4. Генерация " << num_samples << " выборок...\n\n";

    // ==================== 3.4 ОСНОВНОЙ ЭКСПЕРИМЕНТ ====================

    // Структура для хранения результатов
    struct Result {
        double c;
        std::vector<double> estimates_before;  // оценки до атаки
        std::vector<double> estimates_after;   // оценки после атаки
        std::vector<double> errors_before;     // ошибки до атаки
        std::vector<double> errors_after;      // ошибки после атаки
    };

    // Инициализация результатов для каждого c
    std::vector<Result> results;
    for (double c_val : c_values) {
        results.push_back({ c_val, {}, {}, {}, {} });
    }

    // Результаты для среднего арифметического
    std::vector<double> mean_before(num_samples, 0.0);
    std::vector<double> mean_after(num_samples, 0.0);
    std::vector<double> mean_errors_before(num_samples, 0.0);
    std::vector<double> mean_errors_after(num_samples, 0.0);

    // Генерация и обработка нескольких выборок
    for (int sample_idx = 0; sample_idx < num_samples; ++sample_idx) {
        std::cout << "\n=== Выборка #" << (sample_idx + 1) << " ===\n";

        // 3.4. Генерация выборки с идеальным распределением
        rng.seed(987654321 + sample_idx * 1000);
        SENDistribution ideal_dist(ideal_mu, ideal_lambda, ideal_v);
        SENDistribution contamin_dist(contamin_mu, contamin_lambda, contamin_v);

        // Генерация чистой выборки
        double* sample = new double[n];
        for (int i = 0; i < n; ++i) {
            sample[i] = ideal_dist.sample();
        }

        // Создание эмпирического распределения
        EmpiricalDistribution em(sample, n);

        // ==================== ОЦЕНКИ ДО АТАКИ ====================
        std::cout << "3.5. Оценки до атаки:\n";

        std::vector<Estimate*> estimates;
        for (size_t i = 0; i < c_values.size(); ++i) {
            estimates.push_back(new Estimate(em, ideal_lambda, c_values[i]));
            double mu_robust = estimates.back()->getMu();  // устойчивая

            double mu_ordinary = em.moments().M;  // обычная

            results[i].estimates_before.push_back(mu_robust);
            results[i].errors_before.push_back(std::abs(mu_robust - ideal_mu));

            std::cout << "  c=" << c_values[i] << ":\n";
            std::cout << "    Устойчивое: " << mu_robust << "\n";
            std::cout << "    Обычное ср.: " << mu_ordinary << "\n";
            std::cout << "    Разница: " << std::abs(mu_robust - mu_ordinary) << "\n";
        }

        // ==================== 3.6 ЗАСОРЕНИЕ ВЫБОРКИ ====================
        // Определение индексов для засорения
        int num_contaminated = static_cast<int>(n * p_contamination);
        std::vector<int> indices(n);
        for (int i = 0; i < n; ++i) indices[i] = i;

        // Перемешивание индексов для случайного выбора
        std::shuffle(indices.begin(), indices.end(),
            std::mt19937_64(std::random_device{}()));

        // Замена части наблюдений на засоряющие
        for (int i = 0; i < num_contaminated; ++i) {
            double contamin_value = contamin_dist.sample();
            em.changeData(indices[i], contamin_value);
        }

        // ==================== ОЦЕНКИ ПОСЛЕ АТАКИ ====================
        std::cout << "\n3.6 Оценки после атаки (" << (p_contamination * 100) << "%):\n";

        double mu_ordinary_last = 0.0;
        for (size_t i = 0; i < estimates.size(); ++i) {
            double mu_robust = estimates[i]->getMu();

            double mu_ordinary = em.moments().M;
            mu_ordinary_last = mu_ordinary;

            results[i].estimates_after.push_back(mu_robust);
            results[i].errors_after.push_back(std::abs(mu_robust - ideal_mu));

            std::cout << "  c=" << c_values[i] << ":\n";
            std::cout << "    Устойчивое: " << mu_robust << "\n";
            std::cout << "    Обычное ср.: " << mu_ordinary << "\n";
            std::cout << "    Разница: " << std::abs(mu_robust - mu_ordinary) << "\n";
        }

        mean_errors_after[sample_idx] = std::abs(mu_ordinary_last - ideal_mu);
    }

    // ==================== 3.7 АНАЛИЗ РЕЗУЛЬТАТОВ ====================
    std::cout << "\n3.7. Средние ошибки после атаки:\n";

    // Для каждого c
    for (size_t i = 0; i < results.size(); ++i) {
        double sum = 0.0;
        for (double err : results[i].errors_after) sum += err;
        double avg = sum / results[i].errors_after.size();

        std::cout << "  c=" << results[i].c << ": ошибка = " << avg << "\n";
    }

    // Обычное среднее
    double sum_mean = 0.0;
    for (double err : mean_errors_after) sum_mean += err;
    double avg_mean = sum_mean / mean_errors_after.size();

    std::cout << "  Обычное среднее: ошибка = " << avg_mean << "\n\n";

    // Находим лучший c
    double best_err = 1e9;
    double best_c = -1;
    for (size_t i = 0; i < results.size(); ++i) {
        double sum = 0.0;
        for (double err : results[i].errors_after) sum += err;
        double avg = sum / results[i].errors_after.size();

        if (avg < best_err) {
            best_err = avg;
            best_c = results[i].c;
        }
    }

    std::cout << "Наилучший параметр: c = " << best_c
        << " (ошибка: " << best_err << ")\n";

    // ==================== 3.8 АНАЛИЗ ВЕСОВ И СОХРАНЕНИЕ ДАННЫХ ====================
    std::cout << "\n3.8. Анализ весов наблюдений для лучшей оценки (c=" << best_c << "):\n";

    rng.seed(987654321);
    SENDistribution ideal_dist(ideal_mu, ideal_lambda, ideal_v);
    SENDistribution contamin_dist(contamin_mu, contamin_lambda, contamin_v);

    double* analysis_sample = new double[n];
    std::vector<double> ideal_values;     // чистые значения
    std::vector<double> contaminated_values; // засоренные значения
    std::vector<double> all_values;       // все значения
    std::vector<bool> is_contaminated;    // метки засорения

    // Генерация чистой выборки
    for (int i = 0; i < n; ++i) {
        double val = ideal_dist.sample();
        analysis_sample[i] = val;
        ideal_values.push_back(val);
        all_values.push_back(val);
        is_contaminated.push_back(false);
    }

    EmpiricalDistribution em_analysis(analysis_sample, n);

    // Создаем оценку с наилучшим c
    Estimate best_estimate_analysis(em_analysis, ideal_lambda, best_c);

    // Запоминаем веса ДО засорения
    std::vector<double> weights_before;
    for (int i = 0; i < n; ++i) {
        weights_before.push_back(best_estimate_analysis.weight_shurigin(analysis_sample[i]));
    }

    // Засоряем выборку (25%)
    int num_contaminated = static_cast<int>(n * p_contamination);
    std::vector<int> indices(n);
    for (int i = 0; i < n; ++i) indices[i] = i;
    std::shuffle(indices.begin(), indices.end(),
        std::mt19937_64(std::random_device{}()));

    // Засоряем и запоминаем новые значения
    for (int i = 0; i < num_contaminated; ++i) {
        double contamin_value = contamin_dist.sample();
        em_analysis.changeData(indices[i], contamin_value);
        contaminated_values.push_back(contamin_value);
        all_values[indices[i]] = contamin_value;
        is_contaminated[indices[i]] = true;
    }

    // Получаем веса после засорения
    double final_mu = best_estimate_analysis.getMu();
    std::vector<double> weights_after;
    std::vector<double> distances; // расстояния от центра

    for (int i = 0; i < n; ++i) {
        double weight = best_estimate_analysis.weight_shurigin(all_values[i]);
        weights_after.push_back(weight);
        distances.push_back(std::abs(all_values[i] - final_mu) / ideal_lambda);
    }

    // ==================== СОХРАНЕНИЕ ДАННЫХ ДЛЯ ГРАФИКОВ ====================

    // 1. Сохранение весов и значений наблюдений
    std::ofstream weights_file("weights_data_c" + std::to_string(static_cast<int>(best_c)) + ".csv");
    weights_file << "index,value,weight_before,weight_after,distance,is_contaminated\n";

    for (int i = 0; i < n; ++i) {
        weights_file << i << ","
            << all_values[i] << ","
            << weights_before[i] << ","
            << weights_after[i] << ","
            << distances[i] << ","
            << (is_contaminated[i] ? 1 : 0) << "\n";
    }
    weights_file.close();
    std::cout << "Веса сохранены в weights_data_c" << static_cast<int>(best_c) << ".csv\n";

    // 2. Сохранение плотностей распределений
    std::ofstream densities_file("densities_data.csv");
    densities_file << "x,ideal_density,contamin_density,mixture_density,weight_function\n";

    // Вычисляем диапазон для построения
    double min_val = all_values[0], max_val = all_values[0];
    for (int i = 1; i < n; ++i) {
        if (all_values[i] < min_val) min_val = all_values[i];
        if (all_values[i] > max_val) max_val = all_values[i];
    }
    // Расширяем диапазон
    min_val -= 2.0;
    max_val += 2.0;

    // Создаем смесь распределений (засоренное)
    MixtureDistribution<SENDistribution, SENDistribution> mixture_dist(
        ideal_dist, contamin_dist, p_contamination);

    // Масштабный коэффициент для весовой функции (для наглядности)
    double weight_scale = ideal_dist.pdf(ideal_mu); // масштабируем к максимальному значению плотности

    // Вычисляем значения в точках
    int points = 500;
    double step = (max_val - min_val) / (points - 1);

    for (int i = 0; i < points; ++i) {
        double x = min_val + i * step;
        double ideal_dens = ideal_dist.pdf(x);
        double contamin_dens = contamin_dist.pdf(x);
        double mixture_dens = mixture_dist.pdf(x);

        // Весовая функция с масштабированием
        double weight_func = best_estimate_analysis.weight_shurigin(x) * weight_scale;

        densities_file << x << ","
            << ideal_dens << ","
            << contamin_dens << ","
            << mixture_dens << ","
            << weight_func << "\n";
    }
    densities_file.close();
    std::cout << "Плотности сохранены в densities_data.csv\n";

    // 3. Дополнительная статистика
    std::cout << "\nСтатистика весов для c=" << best_c << ":\n";
    std::cout << "-----------------------------------------\n";

    // Разделяем веса на чистые и засоренные
    std::vector<double> clean_weights, contam_weights;
    for (int i = 0; i < n; ++i) {
        if (is_contaminated[i]) {
            contam_weights.push_back(weights_after[i]);
        }
        else {
            clean_weights.push_back(weights_after[i]);
        }
    }

    // Статистика
    double avg_clean = 0.0, avg_contam = 0.0;
    double min_clean = 1.0, min_contam = 1.0;
    double max_clean = 0.0, max_contam = 0.0;

    for (double w : clean_weights) {
        avg_clean += w;
        if (w < min_clean) min_clean = w;
        if (w > max_clean) max_clean = w;
    }
    avg_clean /= clean_weights.size();

    for (double w : contam_weights) {
        avg_contam += w;
        if (w < min_contam) min_contam = w;
        if (w > max_contam) max_contam = w;
    }
    avg_contam /= contam_weights.size();

    // Функция для вычисления медианы
    auto compute_median = [](const std::vector<double>& values) -> double {
        if (values.empty()) return 0.0;

        std::vector<double> sorted = values;
        std::sort(sorted.begin(), sorted.end());

        size_t n = sorted.size();
        if (n % 2 == 0) {
            return (sorted[n / 2 - 1] + sorted[n / 2]) / 2.0;
        }
        else {
            return sorted[n / 2];
        }
        };

    // Вычисляем медианы
    double median_clean = compute_median(clean_weights);
    double median_contam = compute_median(contam_weights);

    std::cout << "Чистые наблюдения (" << clean_weights.size() << "):\n";
    std::cout << "Средний вес: " << avg_clean << "\n";
    std::cout << "Медианный вес: " << median_clean << "\n";
    std::cout << "Мин. вес: " << min_clean << "\n";
    std::cout << "Макс. вес: " << max_clean << "\n\n";

    std::cout << "Засоренные наблюдения (" << contam_weights.size() << "):\n";
    std::cout << "Средний вес: " << avg_contam << "\n";
    std::cout << "Медианный вес: " << median_contam << "\n";
    std::cout << "Мин. вес: " << min_contam << "\n";
    std::cout << "Макс. вес: " << max_contam << "\n\n";

    // Очистка
    delete[] analysis_sample;
}
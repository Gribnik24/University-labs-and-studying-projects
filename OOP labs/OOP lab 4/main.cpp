#include <cmath>
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

using SenMixture = MixtureDistribution<SENDistribution, SENDistribution>;

double* generate_new_sample(int n,
    double mu1, double la1, double v1,
    double mu2, double la2, double v2,
    double p);

void process_sample(const double* sample, int n,
    double mu, double la, double v,
    double mu1, double la1, double v1,
    double mu2, double la2, double v2,
    double p, bool is_new_sample);


void process_sample(const double* sample, int n,
    double mu, double la, double v,
    double mu1, double la1, double v1,
    double mu2, double la2, double v2,
    double p, bool is_new_sample) {

    cout << "  Обработка " << (is_new_sample ? "новой" : "классической")
        << " выборки размером n = " << n << endl;

    EmpiricalDistribution ed(sample, n);

    stringstream filename;
    filename << n;
    if (is_new_sample) filename << "_new";
    filename << "sen_density_data.csv";
    string csv_name = filename.str();

    {
        double xmin_csv = sample[0];
        double xmax_csv = sample[0];
        for (int i = 1; i < n; ++i) {
            if (sample[i] < xmin_csv) xmin_csv = sample[i];
            if (sample[i] > xmax_csv) xmax_csv = sample[i];
        }

        double range = xmax_csv - xmin_csv;
        xmin_csv -= 0.5 * range;
        xmax_csv += 0.5 * range;

        int M_csv = 800;
        double h_csv = (xmax_csv - xmin_csv) / (M_csv - 1);

        ofstream out(csv_name);
        out.imbue(locale::classic());
        out << fixed << setprecision(12);
        out << "x;f_main;f_mix;f_emp\n";

        SENDistribution d_main(mu, la, v);

        SENDistribution d1(mu1, la1, v1);
        SENDistribution d2(mu2, la2, v2);
        SenMixture d_mix(d1, d2, p);

        for (int i = 0; i < M_csv; ++i) {
            double x = xmin_csv + i * h_csv;
            double f_main = d_main.pdf(x);
            double f_mix = d_mix.pdf(x);
            double f_emp = ed.pdf(x);
            out << x << ';' << f_main << ';' << f_mix << ';' << f_emp << '\n';
        }
        out.close();
        cout << "  Файл " << csv_name << " создан" << endl;
    }

    SENDistribution d1_theor(mu1, la1, v1);
    SENDistribution d2_theor(mu2, la2, v2);
    SenMixture mix_dist(d1_theor, d2_theor, p);
    Moments mm_theor = mix_dist.moments();

    Moments m_emp = ed.moments();

    cout << "  Теория:   M=" << mm_theor.M << ", D=" << mm_theor.D2
        << ", g1=" << mm_theor.g1 << ", g2=" << mm_theor.g2 << "\n";
    cout << "  Эмпирика: M=" << m_emp.M << ", D=" << m_emp.D2
        << ", g1=" << m_emp.g1 << ", g2=" << m_emp.g2 << "\n\n";
}

double* generate_new_sample(int n,
    double mu1, double la1, double v1,
    double mu2, double la2, double v2,
    double p) {

    std::mt19937_64 old_rng = rng;
    rng.seed(987654321);

    SENDistribution d1(mu1, la1, v1);
    SENDistribution d2(mu2, la2, v2);
    SenMixture mix_dist(d1, d2, p);

    double* new_sample = new double[n];
    for (int i = 0; i < n; ++i) {
        new_sample[i] = mix_dist.sample();
    }

    rng = old_rng;
    return new_sample;
}

int main() {
    setlocale(LC_ALL, "ru");
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    test_against_table();
    run_required_tests();
    test_sen_class();

    double mu = 1.0, la = 2.0, v = 1.0;
    double mu1 = 0.0, la1 = 1.0, v1 = 1.0;
    double mu2 = 2.0, la2 = 1.5, v2 = 2.0;
    double p = 0.4;

    const int sample_sizes[] = { 200, 1000, 5000, 20000, 100000 };
    const int sample_sizes_count = sizeof(sample_sizes) / sizeof(sample_sizes[0]);

    for (int idx = 0; idx < sample_sizes_count; ++idx) {
        int n = sample_sizes[idx];

        SENDistribution d1(mu1, la1, v1);
        SENDistribution d2(mu2, la2, v2);
        SenMixture mix_gen(d1, d2, p);

        double* sample = new double[n];
        for (int i = 0; i < n; ++i) {
            sample[i] = mix_gen.sample();
        }

        process_sample(sample, n, mu, la, v, mu1, la1, v1, mu2, la2, v2, p, false);

        if (n == 1000) {
            cout << "  ==============================\n";
            cout << "  === ВЫПОЛНЕНИЕ ТЕСТА 3.3.2 ===\n";

            double* new_sample = generate_new_sample(n, mu1, la1, v1,
                mu2, la2, v2, p);
            process_sample(new_sample, n, mu, la, v,
                mu1, la1, v1, mu2, la2, v2, p, true);

            EmpiricalDistribution ed1(sample, n);
            EmpiricalDistribution ed2(new_sample, n);

            Moments m1 = ed1.moments();
            Moments m2 = ed2.moments();

            cout << "  СРАВНЕНИЕ ВЫБОРОК РАЗМЕРОМ 1000:" << endl;
            cout << "  Классическая выборка: M=" << m1.M << ", D=" << m1.D2 << endl;
            cout << "  Новая выборка:        M=" << m2.M << ", D=" << m2.D2 << endl;
            cout << "  Разница: deltaM=" << std::fabs(m1.M - m2.M)
                << ", deltaD=" << std::fabs(m1.D2 - m2.D2) << "\n";
            cout << "  ==============================\n\n";

            delete[] new_sample;
        }

        delete[] sample;
    }

    test_new_classes();
    test_polymorphism();

    return 0;
}


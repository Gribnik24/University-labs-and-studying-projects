#include <iostream>
#include <iomanip>
#include "headers.h"

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

/*
 * Сравнивает рассчитанные функцией значения (дисперсия, эксцесс, f(0))
 * с эталонными значениями из таблицы для различных параметров v.
 * Выводит результаты в виде таблицы.
 */
void test_against_table() {
    std::cout << "Тесты стандартного распределения(mu = 0 la = 1)\n";
    std::cout << "v\t\t sigma2(thr)\t\t sigma2(calc)\t gamma2(thr)\t\t gamma2(calc)\t f0(thr)\t f0(calc)\n";
    std::cout.setf(std::ios::fixed); std::cout << std::setprecision(6);
    for (TableRow t : TAB) {
        double s2 = sen_sigma2(t.v);
        double g2 = sen_gamma2(t.v);
        double f0 = sen_pdf_standard(0.0, t.v);
        std::cout << t.v << "\t " << t.sigma2 << "\t\t " << s2
            << "\t " << t.gamma2 << "\t\t " << g2
            << "\t " << t.f0 << "\t " << f0 << "\n";
    }
    std::cout << "==========================================================\n";
}

// Функция, выполняющая серию обязательных тестов согласно заданию:
void run_required_tests() {
    std::cout << "\n3.1. Масштаб/сдвиг\n";
    double v = 1.0;
    {
        double mu = 0, la = 1;
        std::cout << "3.1.1: mu=0, la=1, v=1\n";
        std::cout << "M=" << mu << ", D=" << sen_sigma2(v) << ", g1=0, g2=" << sen_gamma2(v)
            << ", f(0)=" << sen_pdf(0, mu, la, v) << "\n\n";
    }
    {
        double mu = 0, la = 2;
        std::cout << "3.1.2: mu=0, la=2, v=1\n";
        std::cout << "M=" << mu << ", D=" << (la * la * sen_sigma2(v)) << ", g1=0, g2=" << sen_gamma2(v)
            << ", f(0)=" << sen_pdf(0, mu, la, v) << "\n\n";
    }
    {
        double mu = 1.5, la = 2;
        std::cout << "3.1.3: mu=1.5, la=2, v=1\n";
        std::cout << "M=" << mu << ", D=" << (la * la * sen_sigma2(v)) << ", g1=0, g2=" << sen_gamma2(v)
            << ", f(0)=" << sen_pdf(mu, mu, la, v) << "\n";
    }
    std::cout << "\n==========================================================\n";
    std::cout << "\n3.2. Смеси\n";
    double vv = 1.0;
    {
        double mu = 1.7, la = 2, p = 0.37;
        Moments m1, m2;
        sen_moments_c(mu, la, vv, &m1);
        sen_moments_c(mu, la, vv, &m2);
        Moments mm = mix_moments(m1, m2, p);
        std::cout << "3.2.1: mu1=mu2=1.7, la1=la2=2, v1=v2=1, p=0.37\n";
        std::cout << "M=" << mm.M << ", D=" << mm.D2 << ", g1=" << mm.g1 << ", g2=" << mm.g2 << "\n\n";
    }
    {
        double mu1 = 0, mu2 = 2, la1 = 1, la2 = 1, p = 0.75;
        Moments m1, m2;
        sen_moments_c(mu1, la1, vv, &m1);
        sen_moments_c(mu2, la2, vv, &m2);
        Moments mm = mix_moments(m1, m2, p);
        std::cout << "3.2.2: mu1=0, mu2=2, la1=la2=1, v1=v2=1, p=0.75\n";
        std::cout << "M=" << mm.M << ", D=" << mm.D2 << ", g1=" << mm.g1 << ", g2=" << mm.g2 << "\n\n";
    }
    {
        double mu1 = 0, mu2 = 0, la1 = 1, la2 = 3, p = 0.5;
        Moments m1, m2;
        sen_moments_c(mu1, la1, vv, &m1);
        sen_moments_c(mu2, la2, vv, &m2);
        Moments mm = mix_moments(m1, m2, p);
        std::cout << "3.2.3: mu1=mu2=0, la1=1, la2=3, v1=v2=1, p=0.5\n";
        std::cout << "M=" << mm.M << ", D=" << mm.D2 << ", g1=" << mm.g1 << ", g2=" << mm.g2 << "\n\n";
    }
    {
        double mu1 = 0, mu2 = 0, la1 = 1, la2 = 1, v1 = 0.5, v2 = 2.0, p = 0.5;
        Moments m1, m2;
        sen_moments_c(mu1, la1, v1, &m1);
        sen_moments_c(mu2, la2, v2, &m2);
        Moments mm = mix_moments(m1, m2, p);
        std::cout << "3.2.4: mu1=mu2=0, la1=la2=1, v1=0.5, v2=2, p=0.5\n";
        std::cout << "M=" << mm.M << ", D=" << mm.D2 << ", g1=" << mm.g1 << ", g2=" << mm.g2 << "\n";
    }
    std::cout << "\n==========================================================\n";
}
#include <cmath>
#include "headers.h"

// Функция по вычислению теоретических моментов обобщенного SEN-распределения
// по формуле (3)
double mix_pdf(double x,
    double mu1, double la1, double v1,
    double mu2, double la2, double v2,
    double p)
{
    if (p < 0.0) p = 0.0;
    if (p > 1.0) p = 1.0;
    return (1.0 - p) * sen_pdf(x, mu1, la1, v1)
        + p * sen_pdf(x, mu2, la2, v2);
}

// Функция по вычислению моментов смеси и других параметров
Moments mix_moments(Moments m1, Moments m2, double p) {
    if (p < 0.0) p = 0.0;
    if (p > 1.0) p = 1.0;
    double q = 1.0 - p;

    double M = q * m1.M + p * m2.M; // Первый момент. Формула (7)

    // Дисперсия. формула (8)
    double m2_raw = q * (m1.D2 + m1.M * m1.M) + p * (m2.D2 + m2.M * m2.M);
    double D2 = m2_raw - M * M;

    // Коэффициент асимметрии
    double D = std::sqrt(D2);
    double t1 = q * (std::pow(m1.M - M, 3) + 3.0 * (m1.M - M) * m1.D2);
    double t2 = p * (std::pow(m2.M - M, 3) + 3.0 * (m2.M - M) * m2.D2);
    double g1 = (t1 + t2) / (D2 * D);

    // Коэффициент эксцесса
    double a1 = m1.M - M;
    double a2 = m2.M - M;

    double num4 =
        q * (std::pow(a1, 4) + 6.0 * a1 * a1 * m1.D2 + (m1.g2 + 3.0) * m1.D2 * m1.D2) +
        p * (std::pow(a2, 4) + 6.0 * a2 * a2 * m2.D2 + (m2.g2 + 3.0) * m2.D2 * m2.D2);

    double g2 = num4 / (D2 * D2) - 3.0;

    Moments out;
    out.M = M; out.D2 = D2; out.g1 = g1; out.g2 = g2;
    return out;
}
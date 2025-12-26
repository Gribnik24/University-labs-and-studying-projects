#include <cmath>
#include "headers.h"


// Функция по вычислению плотности вероятности (PDF) стандартного SEN-распределения (mu = 0, lambda = 1)
// По формуле (1)
double sen_pdf_standard(double x, double v) {
    if (v <= 0.0) return 0.0;

    const double a = 0.5 * x * x + v;
    const double sqrt_a = std::sqrt(a);

    // Плотность стандартного нормального распределения
    const double phi = std::exp(-0.5 * x * x) / std::sqrt(2.0 * PI);

    // Первое слагаемое формулы с дополнительной функцией ошибок (erfc)
    const double part_erfc =
        (v * std::exp(v)) / (2.0 * std::sqrt(2.0)) * std::pow(a, -1.5) * std::erfc(sqrt_a);

    // Второе слагаемое формулы
    const double part_phi = (v * phi) / a;

    const double f = part_erfc + part_phi;
    return (f > 0.0 ? f : 0.0);
}

// Функция по вычислению PDF обобщенного SEN-распределения с параметрами сдвига (mu) и масштаба (lambda)
// По формуле (2)
double sen_pdf(double x, double mu, double lambda, double v) {
    if (lambda <= 0.0 || v <= 0.0) return 0.0;
    double z = (x - mu) / lambda;
    return sen_pdf_standard(z, v) / lambda;
}

// Функция по вычислению дисперсию стандартного SEN-распределения
// По формлуе (6)
double sen_sigma2(double v) {
    return v * std::exp(v) * expint_E1(v);
}

// Формула по вычислению коэф. эксцесса стандартного SEN-распределения
// По формуле (5)
double sen_gamma2(double v) {
    double sig2 = sen_sigma2(v);
    double num = 3.0 * v * (1.0 - sig2);
    double den = sig2 * sig2;
    return num / den - 3.0;
}

// Функция по вычислению теоретических моментов обобщенного SEN-распределения
void sen_moments_c(double mu, double lambda, double v, Moments* out) {
    out->M = mu;
    out->D2 = lambda * lambda * sen_sigma2(v);
    out->g1 = 0.0;
    out->g2 = sen_gamma2(v);
}
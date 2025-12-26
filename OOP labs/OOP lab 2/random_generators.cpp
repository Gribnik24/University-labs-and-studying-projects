#include <cmath>
#include <random>
#include "headers.h"

// Генератор СЧ
std::mt19937_64 rng(123456789);

// Генератор случайного числа в интервале (0,1)
double randU() {
    std::uniform_real_distribution<double> U(0.0, 1.0);
    double r = U(rng);
    // защита от 0 и 1
    if (r <= 0.0) r = std::nextafter(0.0, 1.0);
    if (r >= 1.0) r = std::nextafter(1.0, 0.0);
    return r;
}

// Генератор случайного числа из N(0,1)
double randN() {
    double r1 = randU();
    double r2 = randU();
    return std::sqrt(-2.0 * std::log(r1)) * std::cos(2.0 * PI * r2); // Box–Muller
}

// Генератор случайного числа из стандартного SEN-распределения.
// По формуле (16)
double sen_sample_standard(double v) {
    double r0 = randU();
    double z = randN();
    double denom = std::sqrt(1.0 - std::log(r0) / v);
    return z / denom;
}

// Генератор случайного числа из обобщенного SEN-распределения с параметрами
// По формуле (17)
double sen_sample(double mu, double lambda, double v) {
    return mu + lambda * sen_sample_standard(v);
}
#include <algorithm>
#include <iostream>
#include "estimate.h"

// Конструктор. Агрегация по ссылке на эмпирическое распределение e(e0)
Estimate::Estimate(EmpiricalDistribution& e0, double sigma0, double c0, int p0)
    : e(e0), sigma(sigma0), c(c0), p(p0) {
    estimate(); // оценка параметра mu
    e.attach(this); // паттерн "Наблюдатель"
}

// Деструктор
Estimate::~Estimate() {
    e.detach(this);
}

// Весовая функция Шурыгина для варианта 2
double Estimate::weight_shurigin(double x) {
    double z = std::fabs(x - mu) / sigma;
    double z2 = z * z;

    double exp_term = std::exp(-0.5 * z2);

    if (c == 0.0) {
        return exp_term; 
    }

    double numerator = (1.0 + c) * exp_term;
    double denominator = 1.0 + c * exp_term;

    return numerator / denominator;
}

void Estimate::estimate() {

    // Получение данных из эмпирического распределения 
    int n = e.size();
    if (n == 0) {
        mu = 0.0;
        return;
    }

    const double* data = e.data();

    // Начальное приближение - медиана
    std::vector<double> sorted(data, data + n);
    std::sort(sorted.begin(), sorted.end());
    mu = sorted[n / 2];

    // Итерации перевзешивания
    for (int iter = 0; iter < 50; ++iter) {
        double sumWeights = 0.0;
        double sumWeightedValues = 0.0;

        for (int i = 0; i < n; ++i) {
            double w = weight_shurigin(data[i]);
            sumWeights += w;
            sumWeightedValues += w * data[i];
        }

        if (sumWeights > 0) {
            double newMu = sumWeightedValues / sumWeights;
            mu = newMu;
        } 
    }
}
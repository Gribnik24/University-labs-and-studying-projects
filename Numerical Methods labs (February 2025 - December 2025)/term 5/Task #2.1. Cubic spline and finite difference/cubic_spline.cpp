#include "headers.h"
#include <algorithm>
#include <stdexcept>
#include <iostream>

void CubicSpline::initialize(const std::vector<double>& x_nodes, const std::vector<double>& y_nodes) {
    x = x_nodes;
    y = y_nodes;
    calculateCoefficients();
}

void CubicSpline::calculateCoefficients() {
    int n = x.size() - 1;
    a.resize(n + 1);
    b.resize(n + 1);
    c.resize(n + 1);
    d.resize(n + 1);

    // 1. Вычисление длин сегментов h_k (индексация с 0)
    std::vector<double> h(n);
    for (int k = 0; k < n; ++k) {
        h[k] = x[k + 1] - x[k];
    }

    // 2. Граничные условия: c0 = 0
    c[0] = 0.0;

    // 3. Построение системы для c1...c_{n-1}
    if (n >= 2) {
        int system_size = n - 1;
        std::vector<double> alpha(system_size), beta(system_size),
            gamma(system_size), phi(system_size);

        // Заполнение коэффициентов системы
        for (int k = 0; k < system_size; ++k) {
            beta[k] = 2.0 * (h[k] + h[k + 1]);
            gamma[k] = (k < system_size - 1) ? h[k + 1] : 0.0;
            alpha[k] = (k > 0) ? h[k] : 0.0;

            phi[k] = 3.0 * ((y[k + 2] - y[k + 1]) / h[k + 1] - (y[k + 1] - y[k]) / h[k]);
        }

        // Решение системы
        std::vector<double> mu(system_size), z(system_size);
        if (system_size > 0) {
            mu[0] = gamma[0] / beta[0];
            z[0] = phi[0] / beta[0];

            for (int k = 1; k < system_size; ++k) {
                double denominator = beta[k] - alpha[k] * mu[k - 1];
                mu[k] = gamma[k] / denominator;
                z[k] = (phi[k] - alpha[k] * z[k - 1]) / denominator;
            }

            c[system_size] = z[system_size - 1];
            for (int k = system_size - 1; k >= 0; --k) {
                c[k + 1] = z[k] - mu[k] * c[k + 2];
            }
        }
    }

    // 4. Вычисление коэффициентов
    for (int k = 0; k < n; ++k) {
        a[k] = y[k];

        if (k < n - 1) {
            b[k] = (y[k + 1] - y[k]) / h[k] - (2.0 * c[k] + c[k + 1]) * h[k] / 3.0;
            d[k] = (c[k + 1] - c[k]) / (3.0 * h[k]);
        }
        else {
            b[k] = (y[k + 1] - y[k]) / h[k] - (2.0 * c[k]) * h[k] / 3.0;
            d[k] = -c[k] / (3.0 * h[k]);
        }
    }

    a[n] = y[n-1];
}

int CubicSpline::findSegment(double x_point) const {
    auto it = std::upper_bound(x.begin(), x.end(), x_point);
    return std::min(static_cast<int>(std::distance(x.begin(), it)) - 1, static_cast<int>(x.size()) - 2);
}

double CubicSpline::evaluate(double x_point) const {
    int i = findSegment(x_point);
    double dx = x_point - x[i];
    return a[i] + b[i] * dx + c[i] * std::pow(dx, 2) + d[i] * std::pow(dx, 3);
}

double CubicSpline::derivative1(double x_point) const {
    int i = findSegment(x_point);
    double dx = x_point - x[i];
    return b[i] + 2.0 * c[i] * dx + 3.0 * d[i] * dx * dx;
}

double CubicSpline::derivative2(double x_point) const {
    int i = findSegment(x_point);
    double dx = x_point - x[i];
    return 2.0 * c[i] + 6.0 * d[i] * dx;
}
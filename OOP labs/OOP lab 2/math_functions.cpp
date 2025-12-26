#include <cmath>
#include <limits>
#include "headers.h"


// Функция по вычислению E1(x) = ∫_x^∞ (e^{-t}/t) dt
// Используется для расчета дисперсии SEN-распределения
double expint_E1(double x) {
    if (x <= 0.0) return std::numeric_limits<double>::infinity();

    // Для x < 1 применяется разложение в степенной ряд
    if (x < 1.0) {
        const int MAXK = 200;
        const double term0 = -std::log(x) - EULER_GAMMA;
        double sum = 0.0, xn = 1.0;
        for (int k = 1; k <= MAXK; ++k) {
            xn *= -x;                              // (-x)^k
            const double add = xn / (k * std::tgamma(k + 1.0)); // (-1)^k x^k / (k k!)
            sum += add;
            if (std::fabs(add) < 1e-16) break;
        }
        return term0 - sum;
    }

    // Для x >= 1 применяется численное интегрирование(адаптивный метод Симпсона) с оценкой хвоста.
    auto f = [](double t) noexcept { return std::exp(-t) / t; };

    // Рекурсивная лямбда для адаптивного метода Симпсона
    auto simpson = [&](auto&& self, double a, double b, double fa, double fb, double fm,
        double S, double eps, int depth) -> double {
            const double m = 0.5 * (a + b);
            const double lm = 0.5 * (a + m);
            const double rm = 0.5 * (m + b);
            const double flm = f(lm);
            const double frm = f(rm);
            const double Sleft = (m - a) * (fa + 4.0 * flm + fm) / 6.0;
            const double Sright = (b - m) * (fm + 4.0 * frm + fb) / 6.0;
            const double S2 = Sleft + Sright;
            if (depth <= 0 || std::fabs(S2 - S) < 15.0 * eps) {
                // коррекция Ричардсона
                return S2 + (S2 - S) / 15.0;
            }
            return self(self, a, m, fa, fm, flm, Sleft, eps * 0.5, depth - 1) +
                self(self, m, b, fm, fb, frm, Sright, eps * 0.5, depth - 1);
        };

    const double eps = 1e-12;

    // выбираем верхний предел так, чтобы хвост был заведомо мал
    double T = x + std::max(20.0, 5.0 * x + 10.0);
    const double fa = f(x), fb = f(T), fm = f(0.5 * (x + T));
    const double S0 = (T - x) * (fa + 4.0 * fm + fb) / 6.0;
    const double integral = simpson(simpson, x, T, fa, fb, fm, S0, eps, 20);

    // Оценка хвоста интеграла от T до бесконечности
    const double tail = std::exp(-T) / T;
    return integral + tail;
}
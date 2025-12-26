#include <cmath>
#include <vector>
#include <algorithm>
#include <random>
#include <limits>
#include <cstdlib>
#include "helpers.h"

// ==================== ГЕНЕРАТОРЫ СЛУЧАЙНЫХ ЧИСЕЛ ====================

std::mt19937_64 rng(123456789);

// Генератор равномерного числа в (0,1)
double randU() {
    std::uniform_real_distribution<double> U(0.0, 1.0);
    double r = U(rng);
    if (r <= 0.0) r = std::nextafter(0.0, 1.0);
    if (r >= 1.0) r = std::nextafter(1.0, 0.0);
    return r;
}

// Генератор стандартной нормы N(0,1)
double randN() {
    double r1 = randU();
    double r2 = randU();
    return std::sqrt(-2.0 * std::log(r1)) * std::cos(2.0 * PI * r2);
}

// Выборка из стандартного SEN-распределения по формуле (16)
double sen_sample_standard(double v) {
    double r0 = randU();
    double z = randN();
    double denom = std::sqrt(1.0 - std::log(r0) / v);
    return z / denom;
}

// Выборка из обобщенного SEN-распределения
double sen_sample(double mu, double lambda, double v) {
    return mu + lambda * sen_sample_standard(v);
}

// ==================== МАТЕМАТИЧЕСКИЕ ФУНКЦИИ ====================

// Расчет интегральной показательной функции
double expint_E1(double x) {
    if (x <= 0.0) return std::numeric_limits<double>::infinity();

    if (x < 1.0) {
        const int MAXK = 200;
        const double term0 = -std::log(x) - EULER_GAMMA;
        double sum = 0.0, xn = 1.0;
        for (int k = 1; k <= MAXK; ++k) {
            xn *= -x;
            const double add = xn / (k * std::tgamma(k + 1.0));
            sum += add;
            if (std::fabs(add) < 1e-16) break;
        }
        return term0 - sum;
    }

    auto f = [](double t) noexcept { return std::exp(-t) / t; };

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
                return S2 + (S2 - S) / 15.0;
            }
            return self(self, a, m, fa, fm, flm, Sleft, eps * 0.5, depth - 1) +
                self(self, m, b, fm, fb, frm, Sright, eps * 0.5, depth - 1);
        };

    const double eps = 1e-12;
    double T = x + std::max(20.0, 5.0 * x + 10.0);
    const double fa = f(x), fb = f(T), fm = f(0.5 * (x + T));
    const double S0 = (T - x) * (fa + 4.0 * fm + fb) / 6.0;
    const double integral = simpson(simpson, x, T, fa, fb, fm, S0, eps, 20);

    const double tail = std::exp(-T) / T;
    return integral + tail;
}

// ==================== ВСПОМОГАТЕЛЬНЫЕ ФУНКЦИИ ====================

// Возвращает интерквартильный размах (IQR) выборки
double iqr_c(const double* x, int n) {
    if (n <= 0) return 0.0;
    std::vector<double> v(x, x + n);
    std::sort(v.begin(), v.end());

    auto quant = [&](double p)->double {
        double pos = (n - 1) * p;
        int    i = (int)std::floor(pos);
        double t = pos - i;
        if (i + 1 < n) return v[i] * (1 - t) + v[i + 1] * t;
        return v.back();
        };
    return quant(0.75) - quant(0.25);
}

// Строит гистограмму (кусочно-постоянную эмпирическую плотность) по выборке
void build_empirical_density_c(const double* sample, int n, int bins, EmpiricalDensity* ed_out) {
    double a = sample[0], b = sample[0];
    for (int i = 1; i < n; ++i) {
        if (sample[i] < a) a = sample[i];
        if (sample[i] > b) b = sample[i];
    }
    if (a == b) { a -= 0.5; b += 0.5; }

    double width;
    if (bins <= 0) {
        double I = iqr_c(sample, n);
        width = (I > 0 ? 2.0 * I * std::pow((double)n, -1.0 / 3.0) : (b - a) / std::sqrt((double)n));
    }
    else {
        width = (b - a) / bins;
    }
    if (width <= 0) width = (b - a) / std::max(10, (int)std::round(std::sqrt((double)n)));

    int m = std::max(5, (int)std::ceil((b - a) / width));
    width = (b - a) / m;

    EmpiricalDensity ed;
    ed.a = a; ed.b = b; ed.delta = width; ed.m = m;
    ed.count = (int*)std::malloc(sizeof(int) * m);
    for (int i = 0; i < m; ++i) ed.count[i] = 0;

    for (int i = 0; i < n; ++i) {
        int k = (int)std::floor((sample[i] - a) / width);
        if (k < 0) k = 0;
        if (k >= m) k = m - 1;
        ed.count[k]++;
    }
    *ed_out = ed;
}
// Возвращает значение эмпирической плотности в точке x
double empirical_pdf_at_c(const EmpiricalDensity* ed, double x, int n) {
    if (!ed || !ed->count) return 0.0;
    if (x < ed->a || x > ed->b) return 0.0;
    int k = (int)std::floor((x - ed->a) / ed->delta);
    if (k < 0) k = 0;
    if (k >= ed->m) k = ed->m - 1;
    return (double)ed->count[k] / ((double)n * ed->delta);
}
#include <cmath>
#include <vector>
#include <algorithm>
#include <cstdlib>
#include "headers.h"

// Функция по вычислению интерквартильного размаха (IQR) для массива данных
double iqr_c(const double* x, int n) {
    if (n <= 0) return 0.0;
    std::vector<double> v(x, x + n);
    std::sort(v.begin(), v.end());

    // Лямбда-функция для вычисления процентиля методом линейной интерполяции
    auto quant = [&](double p)->double {
        double pos = (n - 1) * p;
        int    i = (int)std::floor(pos);
        double t = pos - i;
        if (i + 1 < n) return v[i] * (1 - t) + v[i + 1] * t;
        return v.back();
        };

    return quant(0.75) - quant(0.25);
}

// Функция по постройке эмпирической плотности по выборке данных
void build_empirical_density_c(const double* sample, int n, int bins, EmpiricalDensity* ed_out) {
    // Находим минимум и максимум в выборке
    double a = sample[0], b = sample[0];
    for (int i = 1; i < n; ++i) { if (sample[i] < a) a = sample[i]; if (sample[i] > b) b = sample[i]; }
    if (a == b) { a -= 0.5; b += 0.5; }

    // Если кол-во интервалов разбиения меньше нуля, то ширина выбирается по правилу Фридмана-Дайсона: width = 2 * IQR * n^{-1/3}
    double width;
    if (bins <= 0) {
        double I = iqr_c(sample, n);
        width = (I > 0 ? 2.0 * I * std::pow((double)n, -1.0 / 3.0)
            : (b - a) / std::sqrt((double)n));
    }
    // ...иначе фиксированное
    else {
        width = (b - a) / bins;
    }
    if (width <= 0) width = (b - a) / std::max(10, (int)std::round(std::sqrt((double)n)));

    // Определяем количество бинов
    int m = std::max(5, (int)std::ceil((b - a) / width));
    width = (b - a) / m;

    // Заполняем структуру гистограммы
    EmpiricalDensity ed;
    ed.a = a; ed.b = b; ed.delta = width; ed.m = m;
    ed.count = (int*)std::malloc(sizeof(int) * m);
    for (int i = 0; i < m; ++i) ed.count[i] = 0;

    // Подсчет количества попаданий в каждый бин
    for (int i = 0; i < n; ++i) {
        int k = (int)std::floor((sample[i] - a) / width);
        if (k < 0) k = 0;
        if (k >= m) k = m - 1;
        ed.count[k]++;
    }
    // Возвращаем результат
    *ed_out = ed;
}

// Функция по освобождению памяти, занятую массивом частот в структуре EmpiricalDensity.
void free_empirical(EmpiricalDensity* ed) {
    if (ed && ed->count) { std::free(ed->count); ed->count = nullptr; ed->m = 0; }
}

// Функция по вычислению значения эмпирической плотности в заданной точке x
// Равно частоте попадания в бин, содержащий x, деленной на (n * width)
double empirical_pdf_at_c(const EmpiricalDensity* ed, double x, int n) {
    if (!ed || !ed->count) return 0.0;
    if (x < ed->a || x > ed->b) return 0.0;
    int k = (int)std::floor((x - ed->a) / ed->delta);
    if (k < 0) k = 0;
    if (k >= ed->m) k = ed->m - 1;
    return (double)ed->count[k] / ((double)n * ed->delta);
}
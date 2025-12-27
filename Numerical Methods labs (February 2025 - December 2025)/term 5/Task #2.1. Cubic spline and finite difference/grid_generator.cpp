#include "headers.h"
#include <vector>
#include <cmath>

std::vector<double> GridGenerator::generateRegularGrid(double a, double b, int nSegments) {
    std::vector<double> grid;

    double h = (b - a) / nSegments;
    for (int i = 0; i <= nSegments; ++i) {
        grid.push_back(a + i * h);
    }

    return grid;
}

std::vector<double> GridGenerator::generateAdaptiveGrid(double a, double b, int nSegments, double r) {
    std::vector<double> grid;
    grid.push_back(a);

    // Вычисляем сумму геометрической прогрессии
    double sum;
    if (std::abs(r - 1.0) > 1e-10) {
        sum = (1.0 - std::pow(r, nSegments)) / (1.0 - r);
    }
    else {
        sum = nSegments;
    }

    double totalLength = b - a;
    double firstStep = totalLength / sum;

    double current = a;
    for (int i = 0; i < nSegments; ++i) {
        double step = firstStep * std::pow(r, i);
        current += step;
        grid.push_back(current);
    }

    // Корректируем последнюю точку точно в b
    grid.back() = b;

    return grid;
}
#include <vector>
#include <fstream>
#include "smoothing_spline.h"

void conductExperiment() {
    // Задаем параметры первого варианта
    const int n_observations = 1304;
    const double mean = 0.94;
    const double stdev = 4.28;

    // Создаем распределение и сетку
    auto random_data = RandomDataGenerator::generateNormalData(n_observations, mean, stdev);
    auto indices = RandomDataGenerator::generateUniformGrid(n_observations, 0, n_observations - 1);

    // Задаем значения параметра сглаживания для тестов и создаем набор сплайнов
    std::vector<double> p_values = { 0.0, 0.2, 0.5, 0.8, 0.99 };
    std::vector<SmoothingSpline> splines;
    for (double p : p_values) {
        SmoothingSpline spline(p);
        spline.initialize(indices, random_data, p);
        splines.push_back(spline);
    }

    // Создаем файл для визуализации
    std::ofstream file("smoothing_splines.csv");
    file << "x,Original";
    for (double p : p_values) {
        file << ",p=" << p;
    }
    file << "\n";

    // Для каждого узла сохраняем исходную точку и значения сплайнов
    for (int i = 0; i < n_observations; ++i) {
        file << indices[i] << "," << random_data[i];
        for (const auto& spline : splines) {
            file << "," << spline.evaluate(indices[i]);
        }
        file << "\n";
    }

    file.close();
}

int main() {
    conductExperiment();
    return 0;
}
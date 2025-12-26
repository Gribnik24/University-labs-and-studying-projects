#include "lotka_volterra.h"
#include <iostream>
#include <vector>

int main() {

    // Фэнтези-модель: Овцы и Драконы
    ModelParams fantasy_params = {
        1.5,    // рождаемость овец
        0.2,    // эффективность охоты драконов
        0.1,    // смертность драконов
        0.02,   // рост драконов
        "Sheep",
        "Dragons"
    };

    // Реалистичная модель: Зайцы и Лисы
    ModelParams real_params = {
        0.8,    // рождаемость зайцев
        0.05,   // эффективность охоты лис
        0.6,    // смертность лис
        0.03,   // рост лис
        "Rabbits",
        "Foxes"
    };

    LotkaVolterraSolver fantasy_solver(fantasy_params);
    LotkaVolterraSolver real_solver(real_params);

    // сценарии для драконов:
    std::vector<std::pair<double, double>> fantasy_scenarios = {
        {120, 6},   // много овец, мало драконов - драконы будут быстро размножаться
        {10, 15},   // мало овец, много драконов - драконы выживут благодаря долгой жизни
        {5, 3}
    };

    // Сценарии для реалистичной
    std::vector<std::pair<double, double>> real_scenarios = {
        {50, 8},   // много зайцев
        {8, 30},   // много лис
        {21, 17}   // около равновесия
    };

    double dt = 0.1;
    int steps_fantasy = 2000;
    int steps_real = 2000;

    // Моделирование фэнтези
    for (size_t i = 0; i < fantasy_scenarios.size(); ++i) {
        double x0 = fantasy_scenarios[i].first;
        double y0 = fantasy_scenarios[i].second;

        std::string suffix;
        if (i == 0) suffix = "many_sheep";
        else if (i == 1) suffix = "many_dragons";
        else suffix = "near_eq";

        auto rk4 = fantasy_solver.rungeKutta4(x0, y0, dt, steps_fantasy);
        fantasy_solver.saveToCSV(rk4, "fantasy_" + suffix + "_rk4.csv");

        auto ab3 = fantasy_solver.adamsBashforth3(x0, y0, dt, steps_fantasy);
        fantasy_solver.saveToCSV(ab3, "fantasy_" + suffix + "_ab3.csv");
    }

    // Моделирование реалистичной
    for (size_t i = 0; i < real_scenarios.size(); ++i) {
        double x0 = real_scenarios[i].first;
        double y0 = real_scenarios[i].second;

        std::string suffix;
        if (i == 0) suffix = "many_rabbits";
        else if (i == 1) suffix = "many_foxes";
        else suffix = "near_eq";

        auto rk4 = real_solver.rungeKutta4(x0, y0, dt, steps_real);
        real_solver.saveToCSV(rk4, "real_" + suffix + "_rk4.csv");

        auto ab3 = real_solver.adamsBashforth3(x0, y0, dt, steps_real);
        real_solver.saveToCSV(ab3, "real_" + suffix + "_ab3.csv");
    }

    return 0;
}
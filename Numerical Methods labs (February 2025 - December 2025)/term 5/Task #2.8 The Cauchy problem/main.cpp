#include "lotka_volterra.h"
#include <iostream>
#include <vector>

int main() {
    // Фэнтези-модель: Овцы и Драконы (исходные параметры)
    ModelParams fantasy_params = {
        1.5,    // рождаемость овец
        0.2,    // эффективность охоты драконов
        0.1,    // смертность драконов
        0.02,   // рост драконов
        "Sheep",
        "Dragons"
    };

    // Реалистичная модель: Зайцы и Лисы (исходные параметры)
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

    std::vector<std::pair<double, double>> fantasy_scenarios = {
        {120, 6},
        {10, 15},
        {5, 3}
    };

    std::vector<std::pair<double, double>> real_scenarios = {
        {50, 8},
        {8, 30},
        {21, 17}
    };

    // Для фэнтези модели
    double dt_fantasy = 0.1;
    int steps_fantasy = 2000;

    // Для реалистичной модели
    double dt_real = 0.25;
    int steps_real = 320;

    for (size_t i = 0; i < fantasy_scenarios.size(); ++i) {
        double x0 = fantasy_scenarios[i].first;
        double y0 = fantasy_scenarios[i].second;

        std::string suffix;
        if (i == 0) suffix = "many_sheep";
        else if (i == 1) suffix = "many_dragons";
        else suffix = "near_eq";

        auto euler = fantasy_solver.eulerExplicit(x0, y0, dt_fantasy, steps_fantasy);
        fantasy_solver.saveToCSV(euler, "fantasy_" + suffix + "_euler.csv");

        auto rk4 = fantasy_solver.rungeKutta4(x0, y0, dt_fantasy, steps_fantasy);
        fantasy_solver.saveToCSV(rk4, "fantasy_" + suffix + "_rk4.csv");

        auto ab3 = fantasy_solver.adamsBashforth3(x0, y0, dt_fantasy, steps_fantasy);
        fantasy_solver.saveToCSV(ab3, "fantasy_" + suffix + "_ab3.csv");
    }

    for (size_t i = 0; i < real_scenarios.size(); ++i) {
        double x0 = real_scenarios[i].first;
        double y0 = real_scenarios[i].second;

        std::string suffix;
        if (i == 0) suffix = "many_rabbits";
        else if (i == 1) suffix = "many_foxes";
        else suffix = "near_eq";

        auto euler = real_solver.eulerExplicit(x0, y0, dt_real, steps_real);
        real_solver.saveToCSV(euler, "real_" + suffix + "_euler.csv");

        auto rk4 = real_solver.rungeKutta4(x0, y0, dt_real, steps_real);
        real_solver.saveToCSV(rk4, "real_" + suffix + "_rk4.csv");

        auto ab3 = real_solver.adamsBashforth3(x0, y0, dt_real, steps_real);
        real_solver.saveToCSV(ab3, "real_" + suffix + "_ab3.csv");
    }

    return 0;
}
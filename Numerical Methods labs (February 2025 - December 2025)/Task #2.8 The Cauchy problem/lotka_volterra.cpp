#include "lotka_volterra.h"
#include <fstream>
#include <iostream>
#include <cmath>
#include <iomanip>

LotkaVolterraSolver::LotkaVolterraSolver(const ModelParams& p) : params(p) {}

void LotkaVolterraSolver::computeDerivatives(double x, double y, double& dx, double& dy) const {
    dx = (params.prey_birth - params.hunt_rate * y) * x;
    dy = (-params.predator_death + params.predator_growth * x) * y;
}

SimulationData LotkaVolterraSolver::rungeKutta4(double x0, double y0, double dt, int steps) const {
    SimulationData result;
    result.time.reserve(steps + 1);
    result.prey.reserve(steps + 1);
    result.predator.reserve(steps + 1);

    double x = x0;
    double y = y0;
    double t = 0.0;

    result.time.push_back(t);
    result.prey.push_back(x);
    result.predator.push_back(y);

    for (int i = 0; i < steps; ++i) {
        double k1x, k1y, k2x, k2y, k3x, k3y, k4x, k4y;

        computeDerivatives(x, y, k1x, k1y);
        computeDerivatives(x + dt * k1x / 2, y + dt * k1y / 2, k2x, k2y);
        computeDerivatives(x + dt * k2x / 2, y + dt * k2y / 2, k3x, k3y);
        computeDerivatives(x + dt * k3x, y + dt * k3y, k4x, k4y);

        x += dt * (k1x + 2 * k2x + 2 * k3x + k4x) / 6.0;
        y += dt * (k1y + 2 * k2y + 2 * k3y + k4y) / 6.0;
        t += dt;

        result.time.push_back(t);
        result.prey.push_back(x);
        result.predator.push_back(y);
    }

    return result;
}

SimulationData LotkaVolterraSolver::adamsBashforth3(double x0, double y0, double dt, int steps) const {
    SimulationData result;

    result.time.reserve(steps + 1);
    result.prey.reserve(steps + 1);
    result.predator.reserve(steps + 1);

    auto start_points = rungeKutta4(x0, y0, dt, 2);

    for (size_t i = 0; i < 3 && i < start_points.size(); ++i) {
        result.time.push_back(start_points.time[i]);
        result.prey.push_back(start_points.prey[i]);
        result.predator.push_back(start_points.predator[i]);
    }

    double fx[3], fy[3];
    for (int i = 0; i < 3; ++i) {
        computeDerivatives(result.prey[i], result.predator[i], fx[i], fy[i]);
    }

    for (int n = 2; n < steps; ++n) {
        double x_new = result.prey[n] + dt / 12.0 * (23 * fx[2] - 16 * fx[1] + 5 * fx[0]);
        double y_new = result.predator[n] + dt / 12.0 * (23 * fy[2] - 16 * fy[1] + 5 * fy[0]);
        double t_new = result.time[n] + dt;

        result.time.push_back(t_new);
        result.prey.push_back(x_new);
        result.predator.push_back(y_new);

        for (int i = 0; i < 2; ++i) {
            fx[i] = fx[i + 1];
            fy[i] = fy[i + 1];
        }

        computeDerivatives(x_new, y_new, fx[2], fy[2]);
    }

    return result;
}

void LotkaVolterraSolver::saveToCSV(const SimulationData& data, const std::string& filename) const {
    std::ofstream file(filename);

    // Заголовок
    file << "time," << params.prey_name << "," << params.predator_name << "\n";

    // Данные
    file << std::fixed << std::setprecision(6);
    for (size_t i = 0; i < data.size(); ++i) {
        file << data.time[i] << ","
            << data.prey[i] << ","
            << data.predator[i] << "\n";
    }

    file.close();
}

std::pair<double, double> LotkaVolterraSolver::calculateEquilibrium() const {
    double x_eq = params.predator_death / params.predator_growth;
    double y_eq = params.prey_birth / params.hunt_rate;
    return { x_eq, y_eq };
}
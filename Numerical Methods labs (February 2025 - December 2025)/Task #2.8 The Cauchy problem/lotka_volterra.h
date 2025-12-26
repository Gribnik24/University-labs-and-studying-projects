#ifndef LOTKA_VOLTERRA_H
#define LOTKA_VOLTERRA_H

#include <string>
#include <vector>

// Параметры модели
struct ModelParams {
    double prey_birth;      // рождаемость жертв
    double hunt_rate;       // скорость охоты
    double predator_death;  // смертность хищников
    double predator_growth; // рост хищников
    std::string prey_name;
    std::string predator_name;
};

// Результаты моделирования
struct SimulationData {
    std::vector<double> time;
    std::vector<double> prey;
    std::vector<double> predator;

    void clear() {
        time.clear();
        prey.clear();
        predator.clear();
    }

    size_t size() const {
        return time.size();
    }
};

// Класс для моделирования
class LotkaVolterraSolver {
private:
    ModelParams params;

    // Вычисление производных
    void computeDerivatives(double x, double y, double& dx, double& dy) const;

public:
    LotkaVolterraSolver(const ModelParams& p);

    // Основные методы
    SimulationData rungeKutta4(double x0, double y0, double dt, int steps) const;
    SimulationData adamsBashforth3(double x0, double y0, double dt, int steps) const;

    // Вспомогательные методы
    void saveToCSV(const SimulationData& data, const std::string& filename) const;
    std::pair<double, double> calculateEquilibrium() const;
    void printInfo() const;
};

#endif
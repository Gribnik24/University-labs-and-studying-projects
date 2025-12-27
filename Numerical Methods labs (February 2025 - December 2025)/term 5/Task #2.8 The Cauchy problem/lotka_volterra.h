#ifndef LOTKA_VOLTERRA_H
#define LOTKA_VOLTERRA_H

#include <string>
#include <vector>

struct ModelParams {
    double prey_birth;
    double hunt_rate;
    double predator_death;
    double predator_growth;
    std::string prey_name;
    std::string predator_name;
};

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

class LotkaVolterraSolver {
private:
    ModelParams params;
    void computeDerivatives(double x, double y, double& dx, double& dy) const;

public:
    LotkaVolterraSolver(const ModelParams& p);
    SimulationData eulerExplicit(double x0, double y0, double dt, int steps) const;
    SimulationData rungeKutta4(double x0, double y0, double dt, int steps) const;
    SimulationData adamsBashforth3(double x0, double y0, double dt, int steps) const;
    void saveToCSV(const SimulationData& data, const std::string& filename) const;
    std::pair<double, double> calculateEquilibrium() const;
    void printInfo() const;
};

#endif
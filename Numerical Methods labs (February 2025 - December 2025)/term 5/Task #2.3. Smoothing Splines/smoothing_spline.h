#ifndef SMOOTHING_SPLINE_H
#define SMOOTHING_SPLINE_H

#include <vector>
#include <random>


class SmoothingSpline {
private:
    std::vector<double> Points;
    std::vector<double> alpha;
    double SMOOTH;

    void Transition_To_Master_Element(int Seg_Num, double X, double& Ksi) const;


    double Basis_Function(int Number, double Ksi) const;


    double Der_Basis_Function(int Number, double Ksi) const;

public:
    SmoothingSpline(double smooth_param = 0.5) : SMOOTH(smooth_param) {}

    void initialize(const std::vector<double>& x_nodes,
        const std::vector<double>& y_nodes,
        double p = 0.5);

    double evaluate(double x) const;
};

class RandomDataGenerator {
private:
    static std::mt19937& getGenerator() {
        static std::mt19937 gen(42);
        return gen;
    }

public:
    static std::vector<double> generateNormalData(int n, double mean = 0, double stddev = 1);
    static std::vector<double> generateUniformGrid(int n, double start = 0, double end = 1);
};

#endif
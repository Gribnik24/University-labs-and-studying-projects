#include "headers.h"
#include <cmath>

double FiniteDifference::forwardDifference(std::function<double(double)> f, double x, double h) {
    return (f(x + h) - f(x)) / h;
}

double FiniteDifference::backwardDifference(std::function<double(double)> f, double x, double h) {
    return (f(x) - f(x - h)) / h;
}

double FiniteDifference::centralDifference(std::function<double(double)> f, double x, double h) {
    return (f(x + h) - f(x - h)) / (2.0 * h);
}

double FiniteDifference::centralDifference4(std::function<double(double)> f, double x, double h) {
    return (-f(x + 2 * h) + 8 * f(x + h) - 8 * f(x - h) + f(x - 2 * h)) / (12.0 * h);
}
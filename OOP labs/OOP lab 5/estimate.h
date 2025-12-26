#ifndef ESTIMATE_H
#define ESTIMATE_H

#include "ipersistent.h"
#include "empirical_distribution.h"
#include <cmath>

class Estimate : public IObserver {
private:
    EmpiricalDistribution& e;
    double sigma;
    double c;
    int p;
    double mu =0.0;

    void estimate(); // вычисление М-оценки

public:
    Estimate(EmpiricalDistribution& e0, double sigma0, double c0, int p0 = 100);
    ~Estimate();

    // Функция для вычисления веса
    double weight_shurigin(double x);

    // IObserver
    void update() override { estimate(); }

    // Геттеры
    double getMu() const { return mu; }
    double getC() const { return c; }

    // Сеттеры
    void setSigma(double sigma0) { sigma = sigma0; estimate(); }
    void setC(double c0) { c = c0; estimate(); }
    void setP(int p0) { p = p0; estimate(); }
};

#endif
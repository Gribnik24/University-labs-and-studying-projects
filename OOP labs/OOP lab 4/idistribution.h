#ifndef IDISTRIBUTION_H
#define IDISTRIBUTION_H

#include <iosfwd>
#include "helpers.h"

class IDistribution {
public:
    virtual ~IDistribution() = default;

    // Плотность в точке x
    virtual double pdf(double x) const = 0;

    // Моменты: M, D2, g1, g2
    virtual Moments moments() const = 0;

    // Моделирование случайной величины
    virtual double sample() const = 0;
};

#endif // IDISTRIBUTION_H
#pragma once

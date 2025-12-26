#ifndef MIXTURE_DISTRIBUTION_H
#define MIXTURE_DISTRIBUTION_H

#include <string>
#include <iosfwd>
#include "sen_distribution.h"
#include "helpers.h"

class MixtureDistribution {
public:
    MixtureDistribution(const SENDistribution& dist1, const SENDistribution& dist2, double p);
    MixtureDistribution(const MixtureDistribution& other) = default;
    MixtureDistribution& operator=(const MixtureDistribution& other) = default;

    // Getters
    const SENDistribution& first_component()  const noexcept { return dist1_; }
    const SENDistribution& second_component() const noexcept { return dist2_; }
    double mixture_parameter() const noexcept { return p_; }

    // Setters — замена компонент целиком
    void set_first_component(const SENDistribution& dist) { dist1_ = dist; }
    void set_second_component(const SENDistribution& dist) { dist2_ = dist; }
    void set_mixture_parameter(double p) { p_ = p; validate(); }

    // Интерфейс
    double pdf(double x) const;
    Moments moments() const;
    double sample() const;
    void save(std::ostream& os) const;
    void save(const std::string& path) const;

    static MixtureDistribution load(std::istream& is);
    static MixtureDistribution load(const std::string& path);

private:
    SENDistribution dist1_;
    SENDistribution dist2_;
    double p_{};

    void validate() const;
};

#endif // MIXTURE_DISTRIBUTION_H

#ifndef MIXTURE_DISTRIBUTION_H
#define MIXTURE_DISTRIBUTION_H

#include <string>
#include <iosfwd>
#include <stdexcept>
#include <cmath>
#include "idistribution.h"
#include "ipersistent.h"
#include "helpers.h"

// Шаблон смеси двух распределений, реализующих IDistribution и IPersistent
template <typename Dist1, typename Dist2>
class MixtureDistribution : public IDistribution, public IPersistent {
public:
    MixtureDistribution(const Dist1& dist1, const Dist2& dist2, double p)
        : dist1_(dist1), dist2_(dist2), p_(p) {
        validate();
    }

    MixtureDistribution(const MixtureDistribution&) = default;
    MixtureDistribution& operator=(const MixtureDistribution&) = default;

    // Getters
    const Dist1& first_component()  const noexcept { return dist1_; }
    const Dist2& second_component() const noexcept { return dist2_; }
    double mixture_parameter() const noexcept { return p_; }

    // Setters 
    void set_first_component(const Dist1& dist) { dist1_ = dist; }
    void set_second_component(const Dist2& dist) { dist2_ = dist; }
    void set_mixture_parameter(double p) { p_ = p; validate(); }

    // IDistribution
    double pdf(double x) const override;
    Moments moments() const override;
    double sample() const override;

    // IPersistent
    void save(std::ostream& os) const override;
    void load(std::istream& is) override;

private:
    Dist1 dist1_;
    Dist2 dist2_;
    double p_{};

    void validate() const;
};

// Реализация шаблонных методов

template <typename Dist1, typename Dist2>
void MixtureDistribution<Dist1, Dist2>::validate() const {
    if (p_ < 0.0 || p_ > 1.0) {
        throw std::invalid_argument("MixtureDistribution: parameter p must be in [0, 1]");
    }
}

template <typename Dist1, typename Dist2>
double MixtureDistribution<Dist1, Dist2>::pdf(double x) const {
    return (1.0 - p_) * dist1_.pdf(x) + p_ * dist2_.pdf(x);
}

template <typename Dist1, typename Dist2>
Moments MixtureDistribution<Dist1, Dist2>::moments() const {
    Moments m1 = dist1_.moments();
    Moments m2 = dist2_.moments();

    double q = 1.0 - p_;
    double M = q * m1.M + p_ * m2.M;
    double m2_raw = q * (m1.D2 + m1.M * m1.M)
        + p_ * (m2.D2 + m2.M * m2.M);
    double D2 = m2_raw - M * M;

    double D = std::sqrt(D2);

    double t1 = q * (std::pow(m1.M - M, 3) +
        3.0 * (m1.M - M) * m1.D2);
    double t2 = p_ * (std::pow(m2.M - M, 3) +
        3.0 * (m2.M - M) * m2.D2);

    double g1 = 0.0;
    if (D2 > 0 && D > 0) {
        g1 = (t1 + t2) / (D2 * D);
    }

    double a1 = m1.M - M;
    double a2 = m2.M - M;
    double num4 = q * (std::pow(a1, 4) +
        6.0 * a1 * a1 * m1.D2 +
        (m1.g2 + 3.0) * m1.D2 * m1.D2)
        + p_ * (std::pow(a2, 4) +
            6.0 * a2 * a2 * m2.D2 +
            (m2.g2 + 3.0) * m2.D2 * m2.D2);

    double g2 = 0.0;
    if (D2 > 0) {
        g2 = num4 / (D2 * D2) - 3.0;
    }

    Moments out;
    out.M = M;
    out.D2 = D2;
    out.g1 = g1;
    out.g2 = g2;
    return out;
}

template <typename Dist1, typename Dist2>
double MixtureDistribution<Dist1, Dist2>::sample() const {
    double u = randU();
    return (u < p_) ? dist2_.sample() : dist1_.sample();
}

template <typename Dist1, typename Dist2>
void MixtureDistribution<Dist1, Dist2>::save(std::ostream& os) const {
    dist1_.save(os);
    dist2_.save(os);
    os << p_ << '\n';
    if (!os) {
        throw std::runtime_error("MixtureDistribution: save failed (ostream bad)");
    }
}

template <typename Dist1, typename Dist2>
void MixtureDistribution<Dist1, Dist2>::load(std::istream& is) {
    dist1_.load(is);
    dist2_.load(is);
    double p;
    if (!(is >> p)) {
        throw std::runtime_error("MixtureDistribution: load failed - bad p format");
    }
    p_ = p;
    validate();
}

#endif // MIXTURE_DISTRIBUTION_H

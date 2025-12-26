#ifndef SEN_DISTRIBUTION_H
#define SEN_DISTRIBUTION_H

#include <string>
#include <iosfwd>
#include "helpers.h"

class SENDistribution {
public:
    // Конструкторы
    SENDistribution();
    SENDistribution(double mu, double lambda, double v);
    explicit SENDistribution(const std::string& path);

    // Загрузка
    static SENDistribution load(std::istream& is);
    static SENDistribution load(const std::string& path);

    // Setters
    void set_mu(double mu);
    void set_lambda(double lambda);
    void set_v(double v);

    // Getters
    double mu()     const noexcept { return mu_; }
    double lambda() const noexcept { return lambda_; }
    double v()      const noexcept { return v_; }

    // Интерфейс
    double pdf(double x) const;
    Moments moments() const;
    double sample() const;
    void save(std::ostream& os) const;
    void save(const std::string& path) const;

private:
    double mu_{ 0.0 };
    double lambda_{ 1.0 };
    double v_{ 1.0 };
    mutable bool   cache_ok_{ false };
    mutable double sig2_cached_{ 0.0 };
    mutable double gamma2_cached_{ 0.0 };

    static void validate_(double lambda, double v);
    void ensure_cache_() const;
    static double sen_pdf_standard(double x, double v);
    static double sen_sigma2(double v);
    static double sen_gamma2(double v);
};

#endif // SEN_DISTRIBUTION_H

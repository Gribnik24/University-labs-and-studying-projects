#ifndef SEN_DISTRIBUTION_H
#define SEN_DISTRIBUTION_H

#include <string>
#include <iosfwd>
#include "helpers.h"
#include "idistribution.h"
#include "ipersistent.h"
#include <stdexcept>

class SENDistribution : public IDistribution, public IPersistent {
public:
    // Конструкторы
    SENDistribution();
    SENDistribution(double mu, double lambda, double v);

    // Getters
    double mu()     const noexcept { return mu_; }
    double lambda() const noexcept { return lambda_; }
    double v()      const noexcept { return v_; }

    // Setters
    void set_mu(double mu);
    void set_lambda(double lambda);
    void set_v(double v);

    // Реализация интерфейса IDistribution
    double pdf(double x) const override;
    Moments moments() const override;
    double sample() const override;

    // Реализация интерфейса IPersistent 
    void save(std::ostream& os) const override;
    void load(std::istream& is) override;

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

// Загрузка из потока (создание нового объекта)
SENDistribution load_sen_from_stream(std::istream& is);

// Загрузка из файла
SENDistribution load_sen_from_file(const std::string& path);

// Сохранение в файл
void save_sen_to_file(const SENDistribution& d, const std::string& path);

#endif // SEN_DISTRIBUTION_H

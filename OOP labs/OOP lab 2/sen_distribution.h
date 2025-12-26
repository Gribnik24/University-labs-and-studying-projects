#ifndef SEN_DISTRIBUTION_H
#define SEN_DISTRIBUTION_H

#include <string>
#include <iosfwd>
#include <stdexcept>
#include "headers.h"

// Класс для SEN-распределения: состояние, вычисления и (де)сериализация.
class SENDistribution {
public:
    // Конструкторы
    SENDistribution();                                   // mu=0, lambda=1, v=1
    SENDistribution(double mu, double lambda, double v); // параметризованный
    explicit SENDistribution(const std::string& path);   // загрузка из файла

    // Загрузка
    static SENDistribution load(std::istream& is);
    static SENDistribution load(const std::string& path);

    // Setters
    void set_mu(double mu);
    void set_lambda(double lambda);   // > 0
    void set_v(double v);             // > 0

    // Getters
    double mu()     const noexcept { return mu_; }
    double lambda() const noexcept { return lambda_; }
    double v()      const noexcept { return v_; }

    // Плотность
    double pdf(double x) const;

    // Моменты
    Moments moments() const;

    // Сэмплирование
    double sample() const;

    // Персистентность
    void save(std::ostream& os) const;
    void save(const std::string& path) const;

private:
    // Параметры
    double mu_{ 0.0 };
    double lambda_{ 1.0 };
    double v_{ 1.0 };

    // Кэш моментов стандартного распределения (зависит только от v)
    mutable bool   cache_ok_{ false };
    mutable double sig2_cached_{ 0.0 };   // sigma^2(v)
    mutable double gamma2_cached_{ 0.0 }; // gamma2(v)

    // Инварианты
    static void validate_(double lambda, double v);
    void ensure_cache_() const;

    // Перенесённые методы со старыми именами (внутренние)
    static double sen_pdf_standard(double x, double v);
    static double sen_sigma2(double v);
    static double sen_gamma2(double v);
};

#endif // SEN_DISTRIBUTION_H

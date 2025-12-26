#include <cmath>
#include <fstream>
#include <stdexcept>
#include "sen_distribution.h"

// ==================== РЕАЛИЗАЦИЯ КЛАССА SENDistribution ====================

// Проверяет корректность параметров распределения
void SENDistribution::validate_(double lambda, double v) {
    if (!(lambda > 0.0))
        throw std::invalid_argument("SEN: lambda must be > 0");
    if (!(v > 0.0))
        throw std::invalid_argument("SEN: v must be > 0");
}

// Оптимизирует вычисления моментов. Избегает повторных тяжелых вычислений expint_E1(v)
void SENDistribution::ensure_cache_() const {
    if (!cache_ok_) {
        sig2_cached_ = SENDistribution::sen_sigma2(v_);
        gamma2_cached_ = SENDistribution::sen_gamma2(v_);
        cache_ok_ = true;
    }
}

// Конструктор. Создает распределение с заданными параметрами
SENDistribution::SENDistribution() {
    validate_(lambda_, v_);
    cache_ok_ = false;
}

// Конструктор. Создает распределение с μ=0, λ=1, v=1 (стандартное SEN)
SENDistribution::SENDistribution(double mu, double lambda, double v)
    : mu_{ mu }, lambda_{ lambda }, v_{ v } {
    validate_(lambda_, v_);
    cache_ok_ = false;
}

// Конструктор. Создает распределение с заданными параметрами из файла
SENDistribution::SENDistribution(const std::string& path) {
    *this = load(path);
}

// Загрузка данных из потока
SENDistribution SENDistribution::load(std::istream& is) {
    double mu, la, vv;
    if (!(is >> mu >> la >> vv))
        throw std::runtime_error("SEN: load(stream) bad format; expected: mu lambda v");
    validate_(la, vv);
    return SENDistribution(mu, la, vv);
}

// Загрузка данных из файла
SENDistribution SENDistribution::load(const std::string& path) {
    std::ifstream in(path);
    if (!in) throw std::runtime_error("SEN: cannot open file for load: " + path);
    return load(in);
}

// Серия сеттеров. Присваивают значения
void SENDistribution::set_mu(double mu) { mu_ = mu; }

void SENDistribution::set_lambda(double lambda) {
    validate_(lambda, v_);
    lambda_ = lambda;
}

void SENDistribution::set_v(double v) {
    validate_(lambda_, v);
    v_ = v;
    cache_ok_ = false;
}

// Возвращает f(x) по формуле (2)
double SENDistribution::pdf(double x) const {
    const double z = (x - mu_) / lambda_;
    return SENDistribution::sen_pdf_standard(z, v_) / lambda_;
}

// Создает моменты для распределения
Moments SENDistribution::moments() const {
    ensure_cache_();
    Moments m;
    m.M = mu_;
    m.D2 = lambda_ * lambda_ * sig2_cached_;
    m.g1 = 0.0;
    m.g2 = gamma2_cached_;
    return m;
}

// Делегирует существующую функцию sen_sample()
double SENDistribution::sample() const {
    return sen_sample(mu_, lambda_, v_);
}

// Метод персистентности, производящий запись и поток
void SENDistribution::save(std::ostream& os) const {
    os << mu_ << ' ' << lambda_ << ' ' << v_ << '\n';
    if (!os)
        throw std::runtime_error("SEN: save() failed (ostream bad)");
}

// Метод персистентности, производящий запись и файл
void SENDistribution::save(const std::string& path) const {
    std::ofstream out(path);
    if (!out) throw std::runtime_error("SEN: cannot open file for save: " + path);
    save(out);
}

// Возвращает значение плотности вероятности (PDF) стандартного SEN-распределения по формуле (1)
double SENDistribution::sen_pdf_standard(double x, double v) {
    if (v <= 0.0) return 0.0;

    const double a = 0.5 * x * x + v;
    const double sqrt_a = std::sqrt(a);
    const double phi = std::exp(-0.5 * x * x) / std::sqrt(2.0 * PI);
    const double part_erfc = (v * std::exp(v)) / (2.0 * std::sqrt(2.0)) * std::pow(a, -1.5) * std::erfc(sqrt_a);
    const double part_phi = (v * phi) / a;
    const double f = part_erfc + part_phi;
    return (f > 0.0 ? f : 0.0);
}

// Вычисляет дисперсию по формуле (6)
double SENDistribution::sen_sigma2(double v) {
    if (v <= 0.0) return 0.0;
    return v * std::exp(v) * expint_E1(v);
}

// Вычислет коэффициент эксцесса стандартного SEN-распределения по формуле (5)
double SENDistribution::sen_gamma2(double v) {
    if (v <= 0.0) return 0.0;
    const double sig2 = SENDistribution::sen_sigma2(v);
    const double num = 3.0 * v * (1.0 - sig2);
    const double den = sig2 * sig2;
    return num / den - 3.0;
}
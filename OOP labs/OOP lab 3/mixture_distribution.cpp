#include <cmath>
#include <fstream>
#include <stdexcept>
#include "mixture_distribution.h"


// Констурктор. Создает смесь двух SEN-распределений с весовым коэффициентом p
MixtureDistribution::MixtureDistribution(const SENDistribution& dist1, const SENDistribution& dist2, double p)
    : dist1_(dist1), dist2_(dist2), p_(p) {
    validate();
}

// Проверяет корректность параметра p (должен быть в диапазоне [0,1])
void MixtureDistribution::validate() const {
    if (p_ < 0.0 || p_ > 1.0) {
        throw std::invalid_argument("MixtureDistribution: parameter p must be in [0, 1]");
    }
}

// Вычисляет плотность смеси в точке x как взвешенную сумму плотностей компонент
double MixtureDistribution::pdf(double x) const {
    return (1.0 - p_) * dist1_.pdf(x) + p_ * dist2_.pdf(x);
}

// Вычисляет моменты смеси через моменты компонент (матожидание, дисперсию, асимметрию, эксцесс)
Moments MixtureDistribution::moments() const {
    Moments m1 = dist1_.moments();
    Moments m2 = dist2_.moments();

    // Вычисление моментов смеси
    double q = 1.0 - p_;
    double M = q * m1.M + p_ * m2.M;
    double m2_raw = q * (m1.D2 + m1.M * m1.M) + p_ * (m2.D2 + m2.M * m2.M);
    double D2 = m2_raw - M * M;

    double D = std::sqrt(D2);
    double t1 = q * (std::pow(m1.M - M, 3) + 3.0 * (m1.M - M) * m1.D2);
    double t2 = p_ * (std::pow(m2.M - M, 3) + 3.0 * (m2.M - M) * m2.D2);
    double g1 = (t1 + t2) / (D2 * D);

    double a1 = m1.M - M;
    double a2 = m2.M - M;
    double num4 = q * (std::pow(a1, 4) + 6.0 * a1 * a1 * m1.D2 + (m1.g2 + 3.0) * m1.D2 * m1.D2) +
        p_ * (std::pow(a2, 4) + 6.0 * a2 * a2 * m2.D2 + (m2.g2 + 3.0) * m2.D2 * m2.D2);

    double g2 = num4 / (D2 * D2) - 3.0;

    Moments out;
    out.M = M; out.D2 = D2; out.g1 = g1; out.g2 = g2;
    return out;
}

// Генерирует случайное значение из смеси (выбирает компоненту согласно вероятности p)
double MixtureDistribution::sample() const {
    double u = randU();
    return (u < p_) ? dist2_.sample() : dist1_.sample();
}

// Сохраняет параметры распределения в поток (оба компонента + вероятность p)
void MixtureDistribution::save(std::ostream& os) const {
    dist1_.save(os);
    dist2_.save(os);
    os << p_ << '\n';
    if (!os) {
        throw std::runtime_error("MixtureDistribution: save failed (ostream bad)");
    }
}

// Сохраняет параметры распределения в файл (оба компонента + вероятность p)
void MixtureDistribution::save(const std::string& path) const {
    std::ofstream out(path);
    if (!out) throw std::runtime_error("MixtureDistribution: cannot open file for save: " + path);
    save(out);
}

// Загружает параметры распределения из потока и создает объект смеси
MixtureDistribution MixtureDistribution::load(std::istream& is) {
    SENDistribution d1 = SENDistribution::load(is);
    SENDistribution d2 = SENDistribution::load(is);
    double p;
    if (!(is >> p)) {
        throw std::runtime_error("MixtureDistribution: load failed - bad p format");
    }
    return MixtureDistribution(d1, d2, p);
}

// Загружает параметры распределения из файла и создает объект смеси
MixtureDistribution MixtureDistribution::load(const std::string& path) {
    std::ifstream in(path);
    if (!in) throw std::runtime_error("MixtureDistribution: cannot open file for load: " + path);
    return load(in);
}
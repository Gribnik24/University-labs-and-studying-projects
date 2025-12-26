#ifndef EMPIRICAL_DISTRIBUTION_H
#define EMPIRICAL_DISTRIBUTION_H

#include <vector>
#include <string>
#include <iosfwd>
#include "helpers.h"

class SENDistribution;
class MixtureDistribution;

class EmpiricalDistribution {
public:
    EmpiricalDistribution() = default;
    EmpiricalDistribution(const std::vector<double>& sample);
    EmpiricalDistribution(const SENDistribution& dist, int sample_size);
    EmpiricalDistribution(const MixtureDistribution& dist, int sample_size);

    // Конструктор копирования, оператор присваивания, деструктор
    EmpiricalDistribution(const EmpiricalDistribution& other);
    EmpiricalDistribution& operator=(const EmpiricalDistribution& other);
    ~EmpiricalDistribution();

    // Интерфейс
    double pdf(double x) const;
    Moments moments() const;
    double sample() const;
    void save(std::ostream& os) const;
    void save(const std::string& path) const;

    static EmpiricalDistribution load(std::istream& is);
    static EmpiricalDistribution load(const std::string& path);

    // Getters
    const std::vector<double>& get_sample() const { return sample_; }
    int get_bins_count() const { return ed_.m; }

private:
    std::vector<double> sample_;
    EmpiricalDensity ed_{}; // хранит гистограмму (владение динамической памятью)

    void build_density();
    void clear_density();
};

#endif // EMPIRICAL_DISTRIBUTION_H

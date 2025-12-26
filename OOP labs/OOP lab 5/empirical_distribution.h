#ifndef EMPIRICAL_DISTRIBUTION_H
#define EMPIRICAL_DISTRIBUTION_H

#include <forward_list>
#include <string>
#include <iosfwd>
#include "helpers.h"
#include "idistribution.h"
#include "ipersistent.h"

class EmpiricalDistribution : public IDistribution, public IPersistent {
private:
    double* sample_{ nullptr };
    int     n_{ 0 };
    EmpiricalDensity ed_{}; // гистограмма
    std::forward_list<IObserver*> observers; // список наблюдателей

    void build_density();
    void clear_density();

public:
    EmpiricalDistribution() = default;

    // Конструктор из готовой выборки
    EmpiricalDistribution(const double* data, int n);

    // Конструктор: выборка из произвольного распределения
    EmpiricalDistribution(const IDistribution& dist, int sample_size);

    EmpiricalDistribution(const EmpiricalDistribution& other);
    EmpiricalDistribution& operator=(const EmpiricalDistribution& other);
    ~EmpiricalDistribution();

    // IDistribution 
    double pdf(double x) const override;
    Moments moments() const override;
    double sample() const override;

    // IPersistent 
    void save(std::ostream& os) const override;
    void load(std::istream& is) override;

    // Getters
    const double* data() const { return sample_; }
    int size() const { return n_; }
    int get_bins_count() const { return ed_.m; }

    // Методы для паттерна Наблюдатель
    void attach(IObserver* obs) { observers.push_front(obs); }
    void detach(IObserver* obs) { observers.remove(obs); }
    void notify();

    // Метод для изменения данных (имитация атаки)
    void changeData(int index, double newValue);
};

// Свободные функции для файловой персистентности
EmpiricalDistribution load_empirical_from_file(const std::string& path);
void save_empirical_to_file(const EmpiricalDistribution& ed, const std::string& path);

#endif
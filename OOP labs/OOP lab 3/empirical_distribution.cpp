#include <cmath>
#include <fstream>
#include <stdexcept>
#include <numeric>
#include <algorithm>
#include "sen_distribution.h"
#include "mixture_distribution.h"
#include "empirical_distribution.h"

// Создает распределение из готовой выборки
EmpiricalDistribution::EmpiricalDistribution(const std::vector<double>& sample)
    : sample_(sample) {
    build_density();
}

// Генерирует выборку из SEN-распределения заданного размера
EmpiricalDistribution::EmpiricalDistribution(const SENDistribution& dist, int sample_size) {
    sample_.reserve(sample_size);
    for (int i = 0; i < sample_size; ++i) {
        sample_.push_back(dist.sample());
    }
    build_density();
}

// Генерирует выборку из смеси распределений заданного размера
EmpiricalDistribution::EmpiricalDistribution(const MixtureDistribution& dist, int sample_size) {
    sample_.reserve(sample_size);
    for (int i = 0; i < sample_size; ++i) {
        sample_.push_back(dist.sample());
    }
    build_density();
}

// Конструктор копирования
EmpiricalDistribution::EmpiricalDistribution(const EmpiricalDistribution& other)
    : sample_(other.sample_), ed_(other.ed_) {
    // Копирование выполняется автоматически через конструктор копирования EmpiricalDensity
}

// Оператор присваивания с проверкой на самоприсваивание
EmpiricalDistribution& EmpiricalDistribution::operator=(const EmpiricalDistribution& other) {
    if (this != &other) {
        sample_ = other.sample_;
        ed_ = other.ed_; // Используем оператор присваивания EmpiricalDensity
    }
    return *this;
}

// Деструктор EmpiricalDensity автоматически освобождает память
EmpiricalDistribution::~EmpiricalDistribution() {}

// Строит эмпирическую плотность по имеющейся выборке
void EmpiricalDistribution::build_density() {
    // Очистка выполняется автоматически при присваивании
    if (sample_.empty()) return;
    build_empirical_density_c(sample_.data(), (int)sample_.size(), -1, &ed_);
}

// Очищает данные о плотности распределения
void EmpiricalDistribution::clear_density() {
    // Создаем пустой объект EmpiricalDensity
    EmpiricalDensity empty;
    ed_ = empty;
}

// Вычисляет значение плотности в точке x
double EmpiricalDistribution::pdf(double x) const {
    if (sample_.empty()) return 0.0;
    return empirical_pdf_at_c(&ed_, x, (int)sample_.size());
}

// Вычисляет основные моменты распределения (матожидание, дисперсию, асимметрию, эксцесс)
Moments EmpiricalDistribution::moments() const {
    Moments m{};
    int n = (int)sample_.size();
    if (n == 0) return m;

    m.M = std::accumulate(sample_.begin(), sample_.end(), 0.0) / n;

    double sum2 = 0.0, sum3 = 0.0, sum4 = 0.0;
    for (double x : sample_) {
        double d = x - m.M;
        double d2 = d * d;
        sum2 += d2;
        sum3 += d2 * d;
        sum4 += d2 * d2;
    }
    m.D2 = sum2 / n;
    double D = std::sqrt(m.D2);

    if (D > 0) {
        m.g1 = (sum3 / n) / (m.D2 * D);
    }
    m.g2 = (sum4 / n) / (m.D2 * m.D2) - 3.0;

    return m;
}

// Возвращает случайное значение из эмпирической выборки
double EmpiricalDistribution::sample() const {
    if (sample_.empty()) return 0.0;
    int index = (int)(randU() * sample_.size());
    if (index == sample_.size()) index--;
    return sample_[index];
}

// Сохраняет выборку в поток
void EmpiricalDistribution::save(std::ostream& os) const {
    os << sample_.size() << '\n';
    for (double val : sample_) {
        os << val << '\n';
    }
    if (!os) {
        throw std::runtime_error("EmpiricalDistribution: save failed (ostream bad)");
    }
}

// Сохраняет выборку в файл
void EmpiricalDistribution::save(const std::string& path) const {
    std::ofstream out(path);
    if (!out) throw std::runtime_error("EmpiricalDistribution: cannot open file for save: " + path);
    save(out);
}

// Загружает выборку из потока и создает распределение
EmpiricalDistribution EmpiricalDistribution::load(std::istream& is) {
    size_t size;
    if (!(is >> size)) {
        throw std::runtime_error("EmpiricalDistribution: load failed - bad size format");
    }
    std::vector<double> sample;
    sample.reserve(size);
    for (size_t i = 0; i < size; ++i) {
        double val;
        if (!(is >> val)) {
            throw std::runtime_error("EmpiricalDistribution: load failed - bad sample data");
        }
        sample.push_back(val);
    }
    return EmpiricalDistribution(sample);
}

// Загружает выборку из файла и создает распределение
EmpiricalDistribution EmpiricalDistribution::load(const std::string& path) {
    std::ifstream in(path);
    if (!in) throw std::runtime_error("EmpiricalDistribution: cannot open file for load: " + path);
    return load(in);
}
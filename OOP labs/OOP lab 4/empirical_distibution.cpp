#include <cmath>
#include <fstream>
#include <stdexcept>
#include <numeric>
#include "empirical_distribution.h"

EmpiricalDistribution::EmpiricalDistribution(const double* data, int n)
    : sample_(nullptr), n_(0) {
    if (n <= 0 || data == nullptr)
        throw std::invalid_argument("EmpiricalDistribution: invalid sample for ctor");
    n_ = n;
    sample_ = new double[n_];
    for (int i = 0; i < n_; ++i) {
        sample_[i] = data[i];
    }
    build_density();
}

EmpiricalDistribution::EmpiricalDistribution(const IDistribution& dist, int sample_size)
    : sample_(nullptr), n_(0) {
    if (sample_size <= 0)
        throw std::invalid_argument("EmpiricalDistribution: sample_size must be > 0");

    n_ = sample_size;
    sample_ = new double[n_];
    for (int i = 0; i < n_; ++i) {
        sample_[i] = dist.sample();
    }
    build_density();
}

// Конструктор копирования
EmpiricalDistribution::EmpiricalDistribution(const EmpiricalDistribution& other)
    : sample_(nullptr), n_(0), ed_(other.ed_) {
    if (other.n_ > 0 && other.sample_ != nullptr) {
        n_ = other.n_;
        sample_ = new double[n_];
        for (int i = 0; i < n_; ++i) {
            sample_[i] = other.sample_[i];
        }
    }
}

// Оператор присваивания
EmpiricalDistribution& EmpiricalDistribution::operator=(const EmpiricalDistribution& other) {
    if (this != &other) {
        delete[] sample_;
        sample_ = nullptr;
        n_ = 0;

        if (other.n_ > 0 && other.sample_ != nullptr) {
            n_ = other.n_;
            sample_ = new double[n_];
            for (int i = 0; i < n_; ++i) {
                sample_[i] = other.sample_[i];
            }
        }
        ed_ = other.ed_;
    }
    return *this;
}

// Деструктор
EmpiricalDistribution::~EmpiricalDistribution() {
    delete[] sample_;
    sample_ = nullptr;
    n_ = 0;
}


void EmpiricalDistribution::build_density() {
    clear_density();
    if (!sample_ || n_ <= 0) return;
    build_empirical_density_c(sample_, n_, -1, &ed_);
}

void EmpiricalDistribution::clear_density() {
    EmpiricalDensity empty;
    ed_ = empty;
}


double EmpiricalDistribution::pdf(double x) const {
    if (!sample_ || n_ <= 0) return 0.0;
    return empirical_pdf_at_c(&ed_, x, n_);
}

Moments EmpiricalDistribution::moments() const {
    Moments m{};
    if (!sample_ || n_ <= 0) return m;

    int n = n_;

    double sum = 0.0;
    for (int i = 0; i < n; ++i) sum += sample_[i];
    m.M = sum / n;

    double sum2 = 0.0, sum3 = 0.0, sum4 = 0.0;
    for (int i = 0; i < n; ++i) {
        double d = sample_[i] - m.M;
        double d2 = d * d;
        sum2 += d2;
        sum3 += d2 * d;
        sum4 += d2 * d2;
    }
    m.D2 = sum2 / n;
    double D = std::sqrt(m.D2);

    if (D > 0) {
        m.g1 = (sum3 / n) / (m.D2 * D);
        m.g2 = (sum4 / n) / (m.D2 * m.D2) - 3.0;
    }
    else {
        m.g1 = 0.0;
        m.g2 = 0.0;
    }
    return m;
}

double EmpiricalDistribution::sample() const {
    if (!sample_ || n_ <= 0) return 0.0;
    int index = static_cast<int>(randU() * n_);
    if (index == n_) index--;
    return sample_[index];
}

void EmpiricalDistribution::save(std::ostream& os) const {
    os << n_ << '\n';
    for (int i = 0; i < n_; ++i) {
        os << sample_[i] << '\n';
    }
    if (!os) {
        throw std::runtime_error("EmpiricalDistribution: save failed (ostream bad)");
    }
}

void EmpiricalDistribution::load(std::istream& is) {
    int size;
    if (!(is >> size) || size < 0) {
        throw std::runtime_error("EmpiricalDistribution: load failed - bad size format");
    }

    double* tmp = nullptr;
    if (size > 0) {
        tmp = new double[size];
        for (int i = 0; i < size; ++i) {
            if (!(is >> tmp[i])) {
                delete[] tmp;
                throw std::runtime_error("EmpiricalDistribution: load failed - bad sample data");
            }
        }
    }

    delete[] sample_;
    sample_ = tmp;
    n_ = size;
    build_density();
}

EmpiricalDistribution load_empirical_from_file(const std::string& path) {
    std::ifstream in(path);
    if (!in)
        throw std::runtime_error("EmpiricalDistribution: cannot open file for load: " + path);
    EmpiricalDistribution ed;
    ed.load(in);
    return ed;
}

void save_empirical_to_file(const EmpiricalDistribution& ed, const std::string& path) {
    std::ofstream out(path);
    if (!out)
        throw std::runtime_error("EmpiricalDistribution: cannot open file for save: " + path);
    ed.save(out);
}

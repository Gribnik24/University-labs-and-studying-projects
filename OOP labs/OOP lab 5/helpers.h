#ifndef HELPERS_H
#define HELPERS_H

#include <random>
#include <algorithm>  
#include <cstdlib>

// Константы 
static const double PI = 3.1415926535897932384626433832795;
static const double EULER_GAMMA = 0.57721566490153286060651209;

// Структура моментов 
struct Moments {
    double M;   // Математическое ожидание
    double D2;  // Дисперсия
    double g1;  // Асимметрия
    double g2;  // Эксцесс
};

// Структура эмпирической плотности (для внутреннего использования) 
struct EmpiricalDensity {
    double a;     // левая граница
    double b;     // правая граница
    double delta; // ширина бина
    int    m;     // число бинов
    int* count; // частоты длины m

    // Конструктор по умолчанию
    EmpiricalDensity() : a(0), b(0), delta(0), m(0), count(nullptr) {}

    // Деструктор
    ~EmpiricalDensity() {
        if (count) {
            std::free(count);
            count = nullptr;
        }
    }

    // Конструктор копирования
    EmpiricalDensity(const EmpiricalDensity& other)
        : a(other.a), b(other.b), delta(other.delta), m(other.m), count(nullptr) {
        if (other.m > 0 && other.count != nullptr) {
            count = static_cast<int*>(std::malloc(sizeof(int) * m));
            std::copy(other.count, other.count + m, count);
        }
    }

    // Оператор присваивания
    EmpiricalDensity& operator=(const EmpiricalDensity& other) {
        if (this != &other) {
            if (count) {
                std::free(count);
                count = nullptr;
            }
            a = other.a;
            b = other.b;
            delta = other.delta;
            m = other.m;
            if (other.m > 0 && other.count != nullptr) {
                count = static_cast<int*>(std::malloc(sizeof(int) * m));
                std::copy(other.count, other.count + m, count);
            }
        }
        return *this;
    }
};

// Генераторы случайных чисел 
extern std::mt19937_64 rng;

// Генератор равномерного числа в (0,1)
double randU();

// Генератор стандартной нормы N(0,1)
double randN();

// Выборка из обобщенного SEN-распределения
double sen_sample(double mu, double lambda, double v);

// Математические функции 

// Расчет интегральной показательной функции
double expint_E1(double x);

// Вспомогательные функции для эмпирической плотности 

// Возвращает интерквартильный размах (IQR) выборки
double iqr_c(const double* x, int n);

// Строит гистограмму (кусочно-постоянную эмпирическую плотность) по выборке
void build_empirical_density_c(const double* sample, int n, int bins, EmpiricalDensity* ed_out);

// Возвращает значение эмпирической плотности в точке x
double empirical_pdf_at_c(const EmpiricalDensity* ed, double x, int n);

#endif // HELPERS_H

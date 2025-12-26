#ifndef HEADERS_H
#define HEADERS_H

#include <random>

#ifndef CONSTANTS_H
#define CONSTANTS_H

static const double PI = 3.1415926535897932384626433832795;
static const double EULER_GAMMA = 0.57721566490153286060651209;

#endif


#ifndef MOMENTS_STRUCT_H
#define MOMENTS_STRUCT_H

typedef struct {
    double M;   // Математическое ожидание
    double D2;  // Дисперсия
    double g1;  // Асимметрия
    double g2;  // Эксцесс
} Moments;

#endif

#ifndef MATH_FUNCTIONS_H
#define MATH_FUNCTIONS_H

double expint_E1(double x);

#endif

#ifndef DISTRIBUTION_FUNCTIONS_H
#define DISTRIBUTION_FUNCTIONS_H

double sen_pdf_standard(double x, double v);

double sen_pdf(double x, double mu, double lambda, double v);

double sen_sigma2(double v);

double sen_gamma2(double v);

void sen_moments_c(double mu, double lambda, double v, Moments* out);

#endif

#ifndef EMPIRICAL_DENSITY_H
#define EMPIRICAL_DENSITY_H

typedef struct {
    double a;       // левая граница
    double b;       // правая граница
    double delta;   // ширина бина
    int    m;       // число бинов
    int* count;   // частоты длины m (malloc/free)
} EmpiricalDensity;

double iqr_c(const double* x, int n);

void build_empirical_density_c(const double* sample, int n, int bins, EmpiricalDensity* ed_out);

void free_empirical(EmpiricalDensity* ed);

double empirical_pdf_at_c(const EmpiricalDensity* ed, double x, int n);

#endif

#ifndef RANDOM_GENERATORS_H
#define RANDOM_GENERATORS_H

extern std::mt19937_64 rng;

double randU();

double randN();

double sen_sample_standard(double v);

double sen_sample(double mu, double lambda, double v);

#endif

#ifndef MIXTURE_FUNCTIONS_H
#define MIXTURE_FUNCTIONS_H

double mix_pdf(double x,
    double mu1, double la1, double v1,
    double mu2, double la2, double v2,
    double p);

Moments mix_moments(Moments m1, Moments m2, double p);

#endif

#ifndef TEST_FUNCTIONS_H
#define TEST_FUNCTIONS_H

struct TableRow {
    double v, sigma2, gamma2, f0;
};

void test_against_table();

void run_required_tests();

#endif

#endif
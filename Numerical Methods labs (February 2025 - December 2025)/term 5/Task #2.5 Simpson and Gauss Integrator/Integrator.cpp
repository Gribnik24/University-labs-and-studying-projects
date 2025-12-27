#include <iostream>
#include <iomanip>
#include <vector>
#include <cmath>
#include <locale>

using namespace std;

// Полиномиальная функция φ(x) = x^4 + x^3 - 2x^2 + 5x - 3
double phi(double x) {
    return pow(x, 4) + pow(x, 3) - 2 * pow(x, 2) + 5 * x - 3;
}

// Аналитическое значение интеграла
double analytical_integral(double a, double b) {
    auto F = [](double x) {
        return pow(x, 5) / 5.0 + pow(x, 4) / 4.0 - 2.0 * pow(x, 3) / 3.0 + 5.0 * pow(x, 2) / 2.0 - 3.0 * x;
        };
    return F(b) - F(a);
}

class Integrator {
private:
    double a, b;
    int N;

public:
    Integrator(double a_val, double b_val, int segments = 1)
        : a(a_val), b(b_val), N(segments) {
    }

    double simpson(double (*f)(double)) {
        double h = (b - a) / N;
        double sum = f(a) + f(b);

        for (int i = 1; i < N; i += 2) {
            sum += 4.0 * f(a + i * h);
        }
        for (int i = 2; i < N; i += 2) {
            sum += 2.0 * f(a + i * h);
        }

        return h * sum / 3.0;
    }

    double gauss(double (*f)(double), int n = 2) {
        double h_k = (b - a) / N;
        vector<double> xi, alpha_i;

        if (n == 2) {
            xi = { -1.0 / sqrt(3.0), 1.0 / sqrt(3.0) };
            alpha_i = { 1.0, 1.0 };
        }

        double sum = 0.0;
        for (int k = 0; k < N; k++) {
            double x_start = a + k * h_k;
            double inner_sum = 0.0;

            for (int i = 0; i < n; i++) {
                double x = h_k * (xi[i] + 1.0) / 2.0 + x_start;
                inner_sum += alpha_i[i] * f(x);
            }
            sum += h_k * inner_sum;
        }

        return sum / 2.0;
    }
};

// Оценка порядка
double estimate_order(double Ih, double Ih2, double I_s) {
    double numerator = Ih2 - Ih;
    double denominator = I_s - Ih2;

    if (abs(denominator) < 1e-15) {
        return 0.0;
    }

    double ratio = numerator / denominator;
    double value = 1.0 + ratio;

    if (value <= 0) {
        return 0.0;
    }

    return log2(abs(value));
}

// Уточнение по Рунге
double runge_richardson(double Ih, double Ih2, int k) {
    return Ih2 + (Ih2 - Ih) / (pow(2, k) - 1);
}

int main() {
    setlocale(LC_ALL, "Russian");

    const double a = 0.04;
    const double b = 0.6;
    const int k_simpson = 4;
    const int k_gauss = 4;

    // Аналитическое значение с высокой точностью
    double I_s = analytical_integral(a, b);
    cout << "АНАЛИТИЧЕСКОЕ ЗНАЧЕНИЕ ИНТЕГРАЛА: I* = " << scientific << setprecision(16) << I_s << endl;
    cout << "==========================================" << endl;

    // Три вложенные сетки
    vector<int> ns = { 2, 4, 8 };
    vector<double> hs;
    for (int n : ns) {
        hs.push_back((b - a) / n);
    }

    // Метод Симпсона
    vector<double> Ih_simpson;
    cout << "\nМЕТОД СИМПСОНА:" << endl;
    cout << "==========================================" << endl;
    for (int i = 0; i < ns.size(); i++) {
        Integrator integr(a, b, ns[i]);
        double result = integr.simpson(phi);
        Ih_simpson.push_back(result);

        double error = I_s - result;
        cout << "h = " << fixed << setprecision(2) << hs[i]
            << " | I* - I^h = " << scientific << setprecision(16) << error << endl;
    }

    // Оценка порядка для Симпсона
    cout << "\nОЦЕНКА ПОРЯДКА СИМПСОНА:" << endl;
    double order_simpson1 = estimate_order(Ih_simpson[0], Ih_simpson[1], I_s);
    double order_simpson2 = estimate_order(Ih_simpson[1], Ih_simpson[2], I_s);
    cout << "h и h/2: k = " << fixed << setprecision(2) << order_simpson1 << endl;
    cout << "h/2 и h/4: k = " << fixed << setprecision(2) << order_simpson2 << endl;

    // Данные для таблицы - Симпсон
    cout << "\nДАННЫЕ ДЛЯ ТАБЛИЦЫ - СИМПСОН:" << endl;
    cout << "h=0.28: I*-I^h = " << scientific << setprecision(16) << I_s - Ih_simpson[0] << endl;
    cout << "h=0.14: I*-I^h = " << scientific << setprecision(16) << I_s - Ih_simpson[1] << endl;
    cout << "h=0.07: I*-I^h = " << scientific << setprecision(16) << I_s - Ih_simpson[2] << endl;
    cout << "(I*-I^h)/(I*-I^{h/2}) для h=0.28 h/2=0.14: " << scientific << setprecision(16) << (I_s - Ih_simpson[0]) / (I_s - Ih_simpson[1]) << endl;
    cout << "(I*-I^h)/(I*-I^{h/2}) для h=0.14  h/2=0.07: " << scientific << setprecision(16) << (I_s - Ih_simpson[1]) / (I_s - Ih_simpson[2]) << endl;
    cout << "(I^{h/2}-I^h)/(2^k-1) для h=0.28 h/2=0.14: " << scientific << setprecision(16) << (Ih_simpson[1] - Ih_simpson[0]) / (pow(2, k_simpson) - 1) << endl;
    cout << "(I^{h/2}-I^h)/(2^k-1) для h=0.14  h/2=0.07: " << scientific << setprecision(16) << (Ih_simpson[2] - Ih_simpson[1]) / (pow(2, k_simpson) - 1) << endl; // ИСПРАВЛЕНО

    double IR_simpson1 = runge_richardson(Ih_simpson[0], Ih_simpson[1], k_simpson);
    double IR_simpson2 = runge_richardson(Ih_simpson[1], Ih_simpson[2], k_simpson);
    cout << "I^R для h=0.28 h/2=0.14: " << scientific << setprecision(16) << IR_simpson1 << endl;
    cout << "I*-I^R для h=0.28 h/2=0.14: " << scientific << setprecision(16) << I_s - IR_simpson1 << endl;
    cout << "I^R для h=0.14 h/2=0.07: " << scientific << setprecision(16) << IR_simpson2 << endl;
    cout << "I*-I^R для h=0.14 h/2=0.07: " << scientific << setprecision(16) << I_s - IR_simpson2 << endl;

    // Метод Гаусса
    vector<double> Ih_gauss;
    cout << "\nМЕТОД ГАУССА-2:" << endl;
    cout << "==========================================" << endl;
    for (int i = 0; i < ns.size(); i++) {
        Integrator integr(a, b, ns[i]);
        double result = integr.gauss(phi, 2);
        Ih_gauss.push_back(result);

        double error = I_s - result;
        cout << "h = " << fixed << setprecision(2) << hs[i]
            << " | I* - I^h = " << scientific << setprecision(16) << error << endl;
    }

    // Оценка порядка для Гаусса
    cout << "\nОЦЕНКА ПОРЯДКА ГАУССА-2:" << endl;
    double order_gauss1 = estimate_order(Ih_gauss[0], Ih_gauss[1], I_s);
    double order_gauss2 = estimate_order(Ih_gauss[1], Ih_gauss[2], I_s);
    cout << "h и h/2: k = " << fixed << setprecision(2) << order_gauss1 << endl;
    cout << "h/2 и h/4: k = " << fixed << setprecision(2) << order_gauss2 << endl;

    // Данные для таблицы - Гаусс
    cout << "\nДАННЫЕ ДЛЯ ТАБЛИЦЫ - ГАУСС-2:" << endl;
    cout << "h=0.28: I*-I^h = " << scientific << setprecision(16) << I_s - Ih_gauss[0] << endl;
    cout << "h=0.14: I*-I^h = " << scientific << setprecision(16) << I_s - Ih_gauss[1] << endl;
    cout << "h=0.07: I*-I^h = " << scientific << setprecision(16) << I_s - Ih_gauss[2] << endl;
    cout << "(I*-I^h)/(I*-I^{h/2}) для h=0.28 h/2=0.14: " << scientific << setprecision(16) << (I_s - Ih_gauss[0]) / (I_s - Ih_gauss[1]) << endl;
    cout << "(I*-I^h)/(I*-I^{h/2}) для h=0.14  h/2=0.07: " << scientific << setprecision(16) << (I_s - Ih_gauss[1]) / (I_s - Ih_gauss[2]) << endl;
    cout << "(I^{h/2}-I^h)/(2^k-1) для h=0.28 h/2=0.14: " << scientific << setprecision(16) << (Ih_gauss[1] - Ih_gauss[0]) / (pow(2, k_gauss) - 1) << endl;
    cout << "(I^{h/2}-I^h)/(2^k-1) для h=0.14  h/2=0.07: " << scientific << setprecision(16) << (Ih_gauss[2] - Ih_gauss[1]) / (pow(2, k_gauss) - 1) << endl; // ИСПРАВЛЕНО

    double IR_gauss1 = runge_richardson(Ih_gauss[0], Ih_gauss[1], k_gauss);
    double IR_gauss2 = runge_richardson(Ih_gauss[1], Ih_gauss[2], k_gauss);
    cout << "I^R для h=0.28 h/2=0.14: " << scientific << setprecision(16) << IR_gauss1 << endl;
    cout << "I*-I^R для h=0.28 h/2=0.14: " << scientific << setprecision(16) << I_s - IR_gauss1 << endl;
    cout << "I^R для h=0.14 h/2=0.07: " << scientific << setprecision(16) << IR_gauss2 << endl;
    cout << "I*-I^R для h=0.14 h/2=0.07: " << scientific << setprecision(16) << I_s - IR_gauss2 << endl;

    return 0;
}
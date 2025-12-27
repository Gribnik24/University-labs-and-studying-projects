#include "smoothing_spline.h"
#include <iostream>
#include <functional>


// Преобразует координату X из реального сегмента [x_i, x_{i+1}] в нормализованные координаты [-1, 1] мастер-элемента по формуле
void SmoothingSpline::Transition_To_Master_Element(int Seg_Num, double X, double& Ksi) const {
    double h = Points[Seg_Num + 1] - Points[Seg_Num];
    Ksi = 2.0 * (X - Points[Seg_Num]) / h - 1.0;
}


// Определяет линейные базисные функции на мастер-элементе [-1, 1] по формулам
double SmoothingSpline::Basis_Function(int Number, double Ksi) const {
    switch (Number) {
    case 1: return 0.5 * (1 - Ksi);
    case 2: return 0.5 * (1 + Ksi);
    default: throw std::invalid_argument("Error in basis function number");
    }
}


// Определяет производные базисных функций
double SmoothingSpline::Der_Basis_Function(int Number, double Ksi) const {
    switch (Number) {
    case 1: return -0.5;
    case 2: return 0.5;
    default: throw std::invalid_argument("Error in basis function derivative number");
    }
}


// Инициализация сплайна
void SmoothingSpline::initialize(const std::vector<double>& x_nodes,
                                 const std::vector<double>& y_nodes,
                                 double p) 
{
    SMOOTH = p;
    Points.clear();

    // Создаем точки, определяем сегменты
    for (size_t i = 0; i < x_nodes.size(); ++i) {
        Points.push_back(x_nodes[i]);
    }

    int Num_Segments = Points.size() - 1;
    alpha.resize(Num_Segments + 1, 0.0);

    // Диагонали матрицы СЛАУ
    std::vector<double> a(Num_Segments + 1, 0.0);  // нижняя диагональ
    std::vector<double> b(Num_Segments + 1, 0.0);  // главная диагональ  
    std::vector<double> c(Num_Segments + 1, 0.0);  // верхняя диагональ

    // Процедура ассемблирования СЛАУ
    auto Assembling = [&](int i, double X, double F_Val, double w) {
        double Ksi;
        Transition_To_Master_Element(i, X, Ksi);

        double f1 = Basis_Function(1, Ksi);
        double f2 = Basis_Function(2, Ksi);

        // Вклад в матрицу (интерполяционная часть)
        b[i] += (1.0 - SMOOTH) * w * f1 * f1;
        b[i + 1] += (1.0 - SMOOTH) * w * f2 * f2;
        a[i + 1] += (1.0 - SMOOTH) * w * f1 * f2;
        c[i] += (1.0 - SMOOTH) * w * f2 * f1;

        // Вклад в правую часть
        alpha[i] += (1.0 - SMOOTH) * w * f1 * F_Val;
        alpha[i + 1] += (1.0 - SMOOTH) * w * f2 * F_Val;
        };

    // Сборка СЛАУ
    for (int i = 0; i < Num_Segments; i++) {
        // Вклад от узлов сетки
        Assembling(i, Points[i], y_nodes[i], 1.0);
        Assembling(i, Points[i + 1], y_nodes[i + 1], 1.0);

        // Вклад от сглаживания (регуляризация)
        double h = Points[i + 1] - Points[i];

        // Плавное увеличение регуляризации при p от 0 до 1
        double reg_scale = 1.0 + 99.0 * p * p;
        double reg_weight = SMOOTH * reg_scale;

        b[i] += reg_weight / h;
        b[i + 1] += reg_weight / h;
        a[i + 1] -= reg_weight / h;
        c[i] -= reg_weight / h;
    }

    // Для очень больших p - дополнительная регуляризация, но плавная
    if (p > 0.8) {
        double extra_scale = (p - 0.8) / 0.2; // От 0 до 1 при p от 0.8 до 1.0
        double extra_reg = extra_scale * extra_scale * 500.0; // Квадратичное увеличение

        for (int i = 0; i < Num_Segments; i++) {
            double h = Points[i + 1] - Points[i];
            b[i] += extra_reg / h;
            b[i + 1] += extra_reg / h;
        }
    }

    // Метод прогонки: прямой ход
    for (int j = 1; j < Num_Segments + 1; j++) {
        if (std::abs(b[j - 1]) < 1e-12) {
            double sum = 0.0;
            for (double val : y_nodes) sum += val;
            double avg = sum / y_nodes.size();
            for (size_t i = 0; i < alpha.size(); ++i) alpha[i] = avg;
            return;
        }
        double m = a[j] / b[j - 1];
        b[j] -= m * c[j - 1];
        alpha[j] -= m * alpha[j - 1];
    }

    // Метод прогонки: обратный ход
    if (std::abs(b[Num_Segments]) < 1e-12) {
        double sum = 0.0;
        for (double val : y_nodes) sum += val;
        double avg = sum / y_nodes.size();
        for (size_t i = 0; i < alpha.size(); ++i) alpha[i] = avg;
        return;
    }

    alpha[Num_Segments] /= b[Num_Segments];
    for (int j = Num_Segments - 1; j >= 0; j--) {
        alpha[j] = (alpha[j] - c[j] * alpha[j + 1]) / b[j];
    }
}


// Вычисляет значение сплайна
double SmoothingSpline::evaluate(double x) const {
    if (Points.empty()) return 0.0;

    double eps = 1e-7;
    int Num_Segments = Points.size() - 1;

    // Поиск сегмента, содержащего x
    for (int i = 0; i < Num_Segments; i++) {
        if ((x >= Points[i] && x <= Points[i + 1]) ||
            std::abs(x - Points[i]) < eps ||
            std::abs(x - Points[i + 1]) < eps) {

            // Переход на мастер-элемент и вычисление полинома
            double Ksi;
            Transition_To_Master_Element(i, x, Ksi);
            return alpha[i] * Basis_Function(1, Ksi) + alpha[i + 1] * Basis_Function(2, Ksi);
        }
    }

    // Если точка вне диапазона
    if (x < Points[0]) return alpha[0];
    if (x > Points[Num_Segments]) return alpha[Num_Segments];

    return 0.0;
}


// Генерирует данные в нормальном распределении с указанными M и std
std::vector<double> RandomDataGenerator::generateNormalData(int n, double mean, double stddev) {
    std::vector<double> data;
    data.reserve(n);

    auto& gen = getGenerator();
    std::normal_distribution<double> dist(mean, stddev);

    for (int i = 0; i < n; ++i) {
        data.push_back(dist(gen));
    }

    return data;
}



// Генерирует сетку
std::vector<double> RandomDataGenerator::generateUniformGrid(int n, double start, double end) {
    std::vector<double> grid;
    grid.reserve(n);

    double step = (end - start) / (n - 1);
    for (int i = 0; i < n; ++i) {
        grid.push_back(start + i * step);
    }

    return grid;
}
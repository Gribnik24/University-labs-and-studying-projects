#ifndef HEADERS_H
#define HEADERS_H


#include <vector>
#include <functional>
#include <cmath>


// Класс генератора сеток
class GridGenerator {
public:
    /**
     * a - начало отрезка
     * b - конец отрезка
     * nSegments - количество сегментов
     * r - коэффициент разрядки
     */

    // Генерация регулярной равномерной сетки на отрезке [a, b]. Возвращает вектор узлов сетки
    static std::vector<double> generateRegularGrid(double a, double b, int nSegments);

    // Генерация адаптивной сетки с геометрической прогрессией шагов. Возвращает вектор узлов адаптивной сетки
    static std::vector<double> generateAdaptiveGrid(double a, double b, int nSegments, double r);
};


// Класс кубического слпайна
class CubicSpline {
private:
    std::vector<double> x;          // Узлы сетки
    std::vector<double> y;          // Значения функции в узлах
    std::vector<double> a, b, c, d; // Коэффициенты сплайна

    // Подсчет коэффициентов сплайна
    void calculateCoefficients();

    // Поиск сегмента, в который попадает заданная точка
    int findSegment(double x_point) const;

public:
    CubicSpline() = default;

    /**
     * x_nodes - узлы сетки
     * y_nodes - значения функции в узлах
     * x_point - точка вычисления
     */

    // Инициализация сплайна по табличной функции
    void initialize(const std::vector<double>& x_nodes, const std::vector<double>& y_nodes);

    // Вычисление значения сплайна в точке
    double evaluate(double x_point) const;

    // Вычисление первой производной сплайна
    double derivative1(double x_point) const;

    // Вычисление второй производной сплайна
    double derivative2(double x_point) const;
};


// Класс методов конечной разности
class FiniteDifference {
public:
    /**
     * f - функция
     * x - точка вычисления
     * h - шаг
     * epsilon - требуемая точность
     */

    // Левосторонняя разность
    static double forwardDifference(std::function<double(double)> f, double x, double h);

    // Правосторонняя разность
    static double backwardDifference(std::function<double(double)> f, double x, double h);

    // Центральная разность O(h^2)
    static double centralDifference(std::function<double(double)> f, double x, double h);

    // Центральная разность O(h^4)
    static double centralDifference4(std::function<double(double)> f, double x, double h);
};

#endif
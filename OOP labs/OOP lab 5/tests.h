#ifndef TESTS_H
#define TESTS_H

#include "sen_distribution.h"
#include "empirical_distribution.h"
#include "mixture_distribution.h"

// Структура для тестовой таблицы 
struct TableRow {
    double v, sigma2, gamma2, f0;
};

extern const TableRow TAB[];

// Лабораторная работа #1 - Теоретическая таблица SEN-распределения
void test_against_table();

// Лабораторная работа #1 - Набор тестов 3.1-3.3
void run_required_tests();

// Лабораторная работа #2 - Тесты для класса SEN-распределения
void test_sen_class();

// Лабораторная работа #3 - Тесты для классов смеси распределений и эмпирического распределения
void test_new_classes();

// Лабораторная работа #4 - Тесты интерфейсов и виртуальных функций
void test_polymorphism();

// Лабораторная работа #5 - Устойчивые оценки (агрегация по ссылке и паттерн "Наблюдатель")
void test_link_aggregation();
#endif

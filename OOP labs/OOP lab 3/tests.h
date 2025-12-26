#ifndef TESTS_H
#define TESTS_H

#include "sen_distribution.h"
#include "mixture_distribution.h"
#include "empirical_distribution.h"

// ==================== Структура для тестовой таблицы ====================
struct TableRow {
    double v, sigma2, gamma2, f0;
};


// ==================== Внешнее объявление тестовой таблицы ====================
extern const TableRow TAB[];


// ==================== Прототипы тестовых функций ====================
// Сверяет реализацию с эталоном
void test_against_table();

// Запускает серию «обязательных» тестов из первой лабораторной работы
void run_required_tests();

// Комплексное тестирование всех возможностей класса SENDistribution
void test_sen_class();

// Комплексное тестирование всех возможностей классов MixtureDistribution и EmpiricalDistribution
void test_new_classes();

#endif
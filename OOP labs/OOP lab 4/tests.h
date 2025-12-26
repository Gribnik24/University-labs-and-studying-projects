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

void test_against_table();

void run_required_tests();

void test_sen_class();

void test_new_classes();

void test_polymorphism();

#endif

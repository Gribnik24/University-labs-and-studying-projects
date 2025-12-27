#pragma once
#ifndef MULTISCALE_ANALYZER_H
#define MULTISCALE_ANALYZER_H

#include <vector>
#include <complex>

namespace SpectralAnalysis
{
    // Типы вейвлетов для многоуровневого анализа
    enum class WaveletFamily
    {
        Haar = 1,       // Вейвлет Хаара
        Shannon = 2,    // Вейвлет Шеннона  
        Daubechies6 = 3 // Вейвлет Добеши 6-го порядка
    };

    // Основной класс для выполнения многоуровневого вейвлет-анализа
    class MultiScaleAnalyzer
    {
    private:
        std::vector<std::complex<double>> lowPassKernel, highPassKernel;
        std::vector<std::vector<std::complex<double>>> detailFilters, approximationFilters;

    public:
        // Конструктор с указанием размера данных и типа вейвлета
        MultiScaleAnalyzer(int signalLength, WaveletFamily waveletType);

    private:
        // Построение системы фильтров для заданного количества уровней
        void ConstructFilterSystem(int decompositionLevels);

        // Генерация базисных функций для конкретного уровня разложения
        void GenerateBasisVectors(int level,
            std::vector<std::vector<std::complex<double>>>& detailBasis,
            std::vector<std::vector<std::complex<double>>>& approximationBasis);

    public:
        // Выполнение прямого вейвлет-преобразования (разложение)
        void ExecuteDecomposition(int level,
            const std::vector<std::complex<double>>& inputSignal,
            std::vector<std::complex<double>>& detailCoefficients,
            std::vector<std::complex<double>>& approximationCoefficients);

        // Выполнение обратного вейвлет-преобразования (восстановление)
        void ExecuteReconstruction(int level,
            const std::vector<std::complex<double>>& detailCoefficients,
            const std::vector<std::complex<double>>& approximationCoefficients,
            std::vector<std::complex<double>>& lowFreqComponent,
            std::vector<std::complex<double>>& highFreqComponent,
            std::vector<std::complex<double>>& reconstructedSignal);
    };
}

#endif
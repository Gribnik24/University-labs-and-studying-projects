#pragma once
#ifndef SIGNAL_MANIPULATOR_H
#define SIGNAL_MANIPULATOR_H

#include <complex>
#include <vector>

namespace SpectralAnalysis
{
    class SignalManipulator
    {
    public:
        // Циклический сдвиг сигнала
        void ApplyCyclicShift(int shiftValue,
            const std::vector<std::complex<double>>& inputData,
            std::vector<std::complex<double>>& outputData);

        // Децимация (прореживание) сигнала
        void PerformDecimation(int decimationFactor,
            const std::vector<std::complex<double>>& inputData,
            std::vector<std::complex<double>>& outputData);

        // Интерполяция (восстановление) сигнала
        void PerformInterpolation(int interpolationFactor,
            const std::vector<std::complex<double>>& inputData,
            std::vector<std::complex<double>>& outputData);

        // Вычисление скалярного произведения
        std::complex<double> CalculateInnerProduct(const std::vector<std::complex<double>>& vectorA,
            const std::vector<std::complex<double>>& vectorB);
    };
}

#endif
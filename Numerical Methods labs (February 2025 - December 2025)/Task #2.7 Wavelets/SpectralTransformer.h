#pragma once
#ifndef SPECTRAL_TRANSFORMER_H
#define SPECTRAL_TRANSFORMER_H

#include <vector>
#include <complex>

namespace SpectralAnalysis
{
    class SpectralTransformer
    {
    public:
        // Быстрое преобразование Фурье
        void ComputeFFT(const std::vector<std::complex<double>>& timeDomain,
            std::vector<std::complex<double>>& frequencyDomain);

        // Обратное быстрое преобразование Фурье
        void ComputeIFFT(const std::vector<std::complex<double>>& frequencyDomain,
            std::vector<std::complex<double>>& timeDomain);

        // Вычисление линейной свертки
        void CalculateConvolution(const std::vector<std::complex<double>>& sequenceA,
            const std::vector<std::complex<double>>& sequenceB,
            std::vector<std::complex<double>>& resultSequence);
    };
}

#endif
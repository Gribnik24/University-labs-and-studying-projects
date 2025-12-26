#include "MultiScaleAnalyzer.h"
#include "AnalysisConstants.h"
#include "SignalManipulator.h"
#include "SpectralTransformer.h"
#include <cmath>
#include <stdexcept>

namespace SpectralAnalysis
{
    // Вспомогательная функция для корректной индексации
    static int ComputeWrappedIndex(int index, int modulus)
    {
        int remainder = index % modulus;
        return (remainder < 0) ? (remainder + modulus) : remainder;
    }

    // Конструктор с инициализацией фильтров
    MultiScaleAnalyzer::MultiScaleAnalyzer(int signalLength, WaveletFamily waveletType)
    {
        int N = signalLength;
        lowPassKernel.assign(N, std::complex<double>(0.0, 0.0));
        highPassKernel.assign(N, std::complex<double>(0.0, 0.0));

        switch (waveletType)
        {
        case WaveletFamily::Shannon:
        {

            double normalization = 1.0 / std::sqrt(2.0);
            lowPassKernel[0] = std::complex<double>(normalization, 0.0);
            highPassKernel[0] = std::complex<double>(normalization, 0.0);

            for (int i = 1; i < N; i++)
            {
                double denominator = std::sin(AnalysisConstants::PI * i / N);
                if (std::abs(denominator) < 1e-10)
                {
                    lowPassKernel[i] = std::complex<double>(0.0, 0.0);
                    highPassKernel[i] = std::complex<double>(0.0, 0.0);
                    continue;
                }

                double realComponent = std::sqrt(2.0) / N * std::cos(AnalysisConstants::PI * i / N) *
                    std::sin(AnalysisConstants::PI * i / 2.0) / denominator;
                double imagComponent = -std::sqrt(2.0) / N * std::sin(AnalysisConstants::PI * i / N) *
                    std::sin(AnalysisConstants::PI * i / 2.0) / denominator;

                std::complex<double> filterValue(realComponent, imagComponent);
                lowPassKernel[i] = filterValue;
                highPassKernel[i] = std::complex<double>(((i % 2 == 0) ? 1 : -1) * filterValue.real(),
                    ((i % 2 == 0) ? 1 : -1) * filterValue.imag());
            }
            break;
        }
        case WaveletFamily::Haar:
        {
            double scaleFactor = AnalysisConstants::INV_SQRT_2;
            lowPassKernel[0] = std::complex<double>(scaleFactor, 0.0);
            lowPassKernel[1] = std::complex<double>(scaleFactor, 0.0);
            highPassKernel[0] = std::complex<double>(scaleFactor, 0.0);
            highPassKernel[1] = std::complex<double>(-scaleFactor, 0.0);
            break;
        }
        case WaveletFamily::Daubechies6:
        {
            // Коэффициенты фильтра Добеши 6-го порядка
            const double db6Coefficients[6] = {
                0.3326705529500826,
                0.8068915093110928,
                0.4598775021184915,
                -0.1350110200102546,
                -0.08544127388202666,
                0.03522629188570953
            };

            for (int i = 0; i < 6; i++)
            {
                lowPassKernel[i] = std::complex<double>(db6Coefficients[i], 0.0);
            }

            for (int k = 0; k < N; k++)
            {
                int mirroredIndex = ComputeWrappedIndex(1 - k, N);
                double signFactor = (k % 2 == 0) ? -1.0 : 1.0;
                highPassKernel[k] = std::complex<double>(signFactor * lowPassKernel[mirroredIndex].real(), 0.0);
            }
            break;
        }
        }
    }

    // Построение системы фильтров
    void MultiScaleAnalyzer::ConstructFilterSystem(int decompositionLevels)
    {
        SignalManipulator manipulator;
        int N = (int)lowPassKernel.size();

        std::vector<std::vector<std::complex<double>>> lowPassFilters(decompositionLevels);
        std::vector<std::vector<std::complex<double>>> highPassFilters(decompositionLevels);

        lowPassFilters[0] = lowPassKernel;
        highPassFilters[0] = highPassKernel;

        for (int level = 1; level < decompositionLevels; level++)
        {
            int elementsCount = N / (int)std::pow(2.0, level);
            lowPassFilters[level].assign(elementsCount, std::complex<double>(0.0, 0.0));
            highPassFilters[level].assign(elementsCount, std::complex<double>(0.0, 0.0));

            for (int n = 0; n < elementsCount; n++)
            {
                int maxIndex = (int)std::pow(2.0, level);
                for (int k = 0; k < maxIndex; k++)
                {
                    lowPassFilters[level][n] += lowPassFilters[0][n + k * N / maxIndex];
                    highPassFilters[level][n] += highPassFilters[0][n + k * N / maxIndex];
                }
            }
        }

        SpectralTransformer transformer;
        std::vector<std::complex<double>> upsampledLowPass, upsampledHighPass;

        detailFilters.resize(decompositionLevels);
        approximationFilters.resize(decompositionLevels);

        detailFilters[0] = highPassFilters[0];
        approximationFilters[0] = lowPassFilters[0];

        for (int level = 1; level < decompositionLevels; level++)
        {
            manipulator.PerformInterpolation(level, lowPassFilters[level], upsampledLowPass);
            manipulator.PerformInterpolation(level, highPassFilters[level], upsampledHighPass);

            transformer.CalculateConvolution(approximationFilters[level - 1],
                upsampledHighPass, detailFilters[level]);
            transformer.CalculateConvolution(approximationFilters[level - 1],
                upsampledLowPass, approximationFilters[level]);
        }
    }

    // Генерация базисных функций
    void MultiScaleAnalyzer::GenerateBasisVectors(int level,
        std::vector<std::vector<std::complex<double>>>& detailBasis,
        std::vector<std::vector<std::complex<double>>>& approximationBasis)
    {
        SignalManipulator manipulator;

        int dataSize = (int)lowPassKernel.size();
        int basisCount = dataSize / (int)std::pow(2.0, level);

        if ((int)approximationFilters.size() < level)
        {
            ConstructFilterSystem(level + 1);
        }

        detailBasis.resize(basisCount);
        approximationBasis.resize(basisCount);

        for (int i = 0; i < basisCount; i++)
        {
            int shiftValue = (int)std::pow(2.0, level) * i;

            std::vector<std::complex<double>> shiftedDetail;
            manipulator.ApplyCyclicShift(shiftValue, detailFilters[level - 1], shiftedDetail);
            detailBasis[i] = shiftedDetail;

            std::vector<std::complex<double>> shiftedApproximation;
            manipulator.ApplyCyclicShift(shiftValue, approximationFilters[level - 1], shiftedApproximation);
            approximationBasis[i] = shiftedApproximation;
        }
    }

    // Прямое вейвлет-преобразование (разложение)
    void MultiScaleAnalyzer::ExecuteDecomposition(int level,
        const std::vector<std::complex<double>>& inputSignal,
        std::vector<std::complex<double>>& detailCoefficients,
        std::vector<std::complex<double>>& approximationCoefficients)
    {
        SignalManipulator manipulator;

        std::vector<std::vector<std::complex<double>>> detailBasis, approximationBasis;
        GenerateBasisVectors(level, detailBasis, approximationBasis);

        int basisCount = (int)detailBasis.size();
        detailCoefficients.assign(basisCount, std::complex<double>(0.0, 0.0));
        approximationCoefficients.assign(basisCount, std::complex<double>(0.0, 0.0));

        for (int basisIdx = 0; basisIdx < basisCount; basisIdx++)
        {
            detailCoefficients[basisIdx] = manipulator.CalculateInnerProduct(inputSignal,
                detailBasis[basisIdx]);
            approximationCoefficients[basisIdx] = manipulator.CalculateInnerProduct(inputSignal,
                approximationBasis[basisIdx]);
        }
    }

    // Обратное вейвлет-преобразование (восстановление)
    void MultiScaleAnalyzer::ExecuteReconstruction(int level,
        const std::vector<std::complex<double>>& detailCoefficients,
        const std::vector<std::complex<double>>& approximationCoefficients,
        std::vector<std::complex<double>>& lowFreqComponent,
        std::vector<std::complex<double>>& highFreqComponent,
        std::vector<std::complex<double>>& reconstructedSignal)
    {
        std::vector<std::vector<std::complex<double>>> detailBasis, approximationBasis;
        GenerateBasisVectors(level, detailBasis, approximationBasis);

        int basisCount = (int)detailBasis.size();
        int dataSize = (int)lowPassKernel.size();

        lowFreqComponent.assign(dataSize, std::complex<double>(0.0, 0.0));
        highFreqComponent.assign(dataSize, std::complex<double>(0.0, 0.0));
        reconstructedSignal.assign(dataSize, std::complex<double>(0.0, 0.0));

        for (int dataIdx = 0; dataIdx < dataSize; dataIdx++)
        {
            std::complex<double> approxSum(0.0, 0.0);
            std::complex<double> detailSum(0.0, 0.0);

            for (int basisIdx = 0; basisIdx < basisCount; basisIdx++)
            {
                approxSum += (approximationCoefficients[basisIdx] *
                    approximationBasis[basisIdx][dataIdx]);
                detailSum += (detailCoefficients[basisIdx] *
                    detailBasis[basisIdx][dataIdx]);
            }

            lowFreqComponent[dataIdx] = approxSum;
            highFreqComponent[dataIdx] = detailSum;
            reconstructedSignal[dataIdx] = approxSum + detailSum;
        }
    }
}
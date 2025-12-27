#include "SignalManipulator.h"
#include <cmath>

namespace SpectralAnalysis
{
    void SignalManipulator::ApplyCyclicShift(int shiftValue,
        const std::vector<std::complex<double>>& inputData,
        std::vector<std::complex<double>>& outputData)
    {
        int dataSize = (int)inputData.size();
        outputData.assign(dataSize, std::complex<double>(0.0, 0.0));

        for (int idx = 0; idx < dataSize; idx++)
        {
            int shiftedIdx = idx - shiftValue;
            if (shiftedIdx < 0) shiftedIdx += dataSize;
            outputData[idx] = inputData[shiftedIdx];
        }
    }

    void SignalManipulator::PerformDecimation(int decimationFactor,
        const std::vector<std::complex<double>>& inputData,
        std::vector<std::complex<double>>& outputData)
    {
        int factor = (int)std::pow(2.0, decimationFactor);
        int newSize = (int)inputData.size() / factor;
        outputData.assign(newSize, std::complex<double>(0.0, 0.0));

        for (int i = 0; i < newSize; i++)
            outputData[i] = inputData[i * factor];
    }

    void SignalManipulator::PerformInterpolation(int interpolationFactor,
        const std::vector<std::complex<double>>& inputData,
        std::vector<std::complex<double>>& outputData)
    {
        int factor = (int)std::pow(2.0, interpolationFactor);
        int newSize = (int)inputData.size() * factor;
        outputData.assign(newSize, std::complex<double>(0.0, 0.0));

        for (int idx = 0; idx < newSize; idx++)
        {
            if (idx % factor == 0)
                outputData[idx] = inputData[idx / factor];
            else
                outputData[idx] = std::complex<double>(0.0, 0.0);
        }
    }

    std::complex<double> SignalManipulator::CalculateInnerProduct(
        const std::vector<std::complex<double>>& vectorA,
        const std::vector<std::complex<double>>& vectorB)
    {
        int size = (int)vectorA.size();
        std::complex<double> result(0.0, 0.0);

        for (int idx = 0; idx < size; idx++)
            result += vectorA[idx] * std::conj(vectorB[idx]);

        return result;
    }
}
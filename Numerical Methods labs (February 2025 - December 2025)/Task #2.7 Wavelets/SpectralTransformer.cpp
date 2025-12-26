#include "SpectralTransformer.h"
#include "AnalysisConstants.h"
#include <cmath>

namespace SpectralAnalysis
{
    void SpectralTransformer::ComputeFFT(const std::vector<std::complex<double>>& timeDomain,
        std::vector<std::complex<double>>& frequencyDomain)
    {
        int size = (int)timeDomain.size();
        int halfSize = size / 2;
        frequencyDomain.assign(size, std::complex<double>(0.0, 0.0));

        std::complex<double> phaseFactor, evenPart, oddPart;

        for (int freqIdx = 0; freqIdx < halfSize; freqIdx++)
        {
            evenPart = { 0.0, 0.0 };
            oddPart = { 0.0, 0.0 };

            for (int timeIdx = 0; timeIdx < halfSize; timeIdx++)
            {
                phaseFactor = {
                    std::cos(-AnalysisConstants::TWO_PI * freqIdx * timeIdx / halfSize),
                    std::sin(-AnalysisConstants::TWO_PI * freqIdx * timeIdx / halfSize)
                };
                evenPart += timeDomain[2 * timeIdx] * phaseFactor;
                oddPart += timeDomain[2 * timeIdx + 1] * phaseFactor;
            }

            phaseFactor = {
                std::cos(-AnalysisConstants::TWO_PI * freqIdx / size),
                std::sin(-AnalysisConstants::TWO_PI * freqIdx / size)
            };
            frequencyDomain[freqIdx] = evenPart + phaseFactor * oddPart;
            frequencyDomain[freqIdx + halfSize] = evenPart - phaseFactor * oddPart;
        }
    }

    void SpectralTransformer::ComputeIFFT(const std::vector<std::complex<double>>& frequencyDomain,
        std::vector<std::complex<double>>& timeDomain)
    {
        int size = (int)frequencyDomain.size();
        ComputeFFT(frequencyDomain, timeDomain);

        std::complex<double> tempValue;
        for (int i = 1; i <= size / 2; i++)
        {
            tempValue = timeDomain[i];
            timeDomain[i] = timeDomain[size - i] / double(size);
            timeDomain[size - i] = tempValue / double(size);
        }
        timeDomain[0] /= double(size);
    }

    void SpectralTransformer::CalculateConvolution(
        const std::vector<std::complex<double>>& sequenceA,
        const std::vector<std::complex<double>>& sequenceB,
        std::vector<std::complex<double>>& resultSequence)
    {
        int size = (int)sequenceA.size();
        std::vector<std::complex<double>> intermediate(size);

        resultSequence.clear();
        resultSequence.resize(size);

        ComputeFFT(sequenceA, resultSequence);
        ComputeFFT(sequenceB, intermediate);

        for (int idx = 0; idx < size; idx++)
            intermediate[idx] *= resultSequence[idx];

        ComputeIFFT(intermediate, resultSequence);
    }
}
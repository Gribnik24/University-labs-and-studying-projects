#include <iostream>
#include <vector>
#include <complex>
#include <cmath>
#include <random>
#include <string>
#include <fstream>
#include <iomanip>
#include <chrono>

#include "MultiScaleAnalyzer.h"
//#include "AnalysisConstants.h"

constexpr double MATH_PI = 3.14159265358979323846;

// Функция для сохранения сигнала в CSV формат
static void ExportSignalData(const std::string& filePath, const std::vector<std::complex<double>>& signalData)
{
    std::ofstream outputFile(filePath);
    outputFile << "sample_index,real_component,imag_component\n";
    for (int idx = 0; idx < (int)signalData.size(); idx++)
        outputFile << idx << "," << std::setprecision(17)
        << signalData[idx].real() << "," << signalData[idx].imag() << "\n";
}

// Функция для сохранения вейвлет-коэффициентов
static void ExportWaveletCoefficients(const std::string& filePath, int decompositionLevel,
    const std::vector<std::complex<double>>& detailCoefs,
    const std::vector<std::complex<double>>& approxCoefs)
{
    std::ofstream outputFile(filePath);
    outputFile << "basis_index,sample_position,detail_real,detail_imag,"
        << "approx_real,approx_imag,detail_magnitude,approx_magnitude\n";

    for (int k = 0; k < (int)detailCoefs.size(); k++)
    {
        int position = (int)(std::pow(2.0, decompositionLevel) * k);
        outputFile << k << "," << position << ","
            << std::setprecision(17) << detailCoefs[k].real() << "," << detailCoefs[k].imag() << ","
            << approxCoefs[k].real() << "," << approxCoefs[k].imag() << ","
            << std::abs(detailCoefs[k]) << "," << std::abs(approxCoefs[k]) << "\n";
    }
}

// Функция для сохранения результатов фильтрации
static void ExportFilteringResults(const std::string& filePath,
    const std::vector<std::complex<double>>& originalSignal,
    const std::vector<std::complex<double>>& filteredSignal,
    const std::vector<std::complex<double>>& residualSignal)
{
    std::ofstream outputFile(filePath);
    outputFile << "sample_index,original_real,filtered_real,residual_real\n";
    for (int idx = 0; idx < (int)originalSignal.size(); idx++)
        outputFile << idx << "," << std::setprecision(17)
        << originalSignal[idx].real() << ","
        << filteredSignal[idx].real() << ","
        << residualSignal[idx].real() << "\n";
}

// Функция для сохранения проекционных компонент
static void ExportProjectionComponents(const std::string& filePath,
    const std::vector<std::complex<double>>& lowFreqProjection,
    const std::vector<std::complex<double>>& highFreqProjection)
{
    std::ofstream outputFile(filePath);
    outputFile << "sample_index,lowfreq_real,highfreq_real\n";
    for (int idx = 0; idx < (int)lowFreqProjection.size(); idx++)
        outputFile << idx << "," << std::setprecision(17)
        << lowFreqProjection[idx].real() << ","
        << highFreqProjection[idx].real() << "\n";
}

// Преобразование типа вейвлета в строковое представление
static std::string GetWaveletFamilyName(SpectralAnalysis::WaveletFamily type)
{
    using WF = SpectralAnalysis::WaveletFamily;
    if (type == WF::Haar) return "haar";
    if (type == WF::Shannon) return "shannon";
    return "d6";
}

// Вычисление только низкочастотной проекции (P-компоненты)
static void ExtractLowFrequencyComponent(SpectralAnalysis::MultiScaleAnalyzer& analyzer,
    int level,
    const std::vector<std::complex<double>>& signal,
    std::vector<std::complex<double>>& lowFreqResult)
{
    std::vector<std::complex<double>> detailCoefs, approxCoefs;
    analyzer.ExecuteDecomposition(level, signal, detailCoefs, approxCoefs);

    // Обнуляем детализирующие коэффициенты
    std::vector<std::complex<double>> zeroedDetails(detailCoefs.size(), { 0.0, 0.0 });

    std::vector<std::complex<double>> tempLow, tempHigh, tempReconstructed;
    analyzer.ExecuteReconstruction(level, zeroedDetails, approxCoefs,
        tempLow, tempHigh, tempReconstructed);
    lowFreqResult = tempLow;
}

// Основная процедура обработки для конкретного вейвлет-базиса
static void ProcessWaveletBasis(const std::string& outputDirectory,
    SpectralAnalysis::WaveletFamily waveletType,
    const std::vector<std::complex<double>>& inputSignal,
    int maxDecompositionLevels)
{
    int signalLength = (int)inputSignal.size();
    SpectralAnalysis::MultiScaleAnalyzer analyzer(signalLength, waveletType);
    std::string basisIdentifier = GetWaveletFamilyName(waveletType);

    for (int currentLevel = 1; currentLevel <= maxDecompositionLevels; currentLevel++)
    {
        // Этап 1: Разложение сигнала
        std::vector<std::complex<double>> detailCoefs, approxCoefs;
        analyzer.ExecuteDecomposition(currentLevel, inputSignal, detailCoefs, approxCoefs);

        // Сохраняем коэффициенты до обработки
        ExportWaveletCoefficients(
            outputDirectory + "/coeffs_initial_" + basisIdentifier + "_level" +
            std::to_string(currentLevel) + ".csv",
            currentLevel, detailCoefs, approxCoefs);

        // Обнуление детализирующих коэффициентов (имитация фильтрации)
        std::vector<std::complex<double>> zeroedDetails(detailCoefs.size(), { 0.0, 0.0 });

        // Сохраняем коэффициенты после "обнуления"
        ExportWaveletCoefficients(
            outputDirectory + "/coeffs_processed_" + basisIdentifier + "_level" +
            std::to_string(currentLevel) + ".csv",
            currentLevel, zeroedDetails, approxCoefs);

        // Этап 2: Восстановление сигнала
        std::vector<std::complex<double>> lowFreqComp, highFreqComp, restoredSignal;
        analyzer.ExecuteReconstruction(currentLevel, zeroedDetails, approxCoefs,
            lowFreqComp, highFreqComp, restoredSignal);

        // Вычисление остаточного сигнала
        std::vector<std::complex<double>> residual(signalLength);
        for (int idx = 0; idx < signalLength; idx++)
            residual[idx] = inputSignal[idx] - restoredSignal[idx];

        // Сохранение результатов фильтрации
        ExportFilteringResults(
            outputDirectory + "/filtering_results_" + basisIdentifier +
            "_level" + std::to_string(currentLevel) + ".csv",
            inputSignal, restoredSignal, residual);

        // Вычисление проекционных компонент предыдущего уровня
        std::vector<std::complex<double>> prevLowFreq(signalLength), prevHighFreq(signalLength);
        if (currentLevel == 1)
        {
            prevLowFreq = restoredSignal;
            for (int idx = 0; idx < signalLength; idx++)
                prevHighFreq[idx] = { 0.0, 0.0 };
        }
        else
        {
            ExtractLowFrequencyComponent(analyzer, currentLevel - 1,
                restoredSignal, prevLowFreq);
            for (int idx = 0; idx < signalLength; idx++)
                prevHighFreq[idx] = restoredSignal[idx] - prevLowFreq[idx];
        }

        // Сохранение проекционных компонент
        ExportProjectionComponents(
            outputDirectory + "/projection_components_" + basisIdentifier +
            "_level" + std::to_string(currentLevel) + ".csv",
            prevLowFreq, prevHighFreq);
    }
}

// Генератор гармонического сигнала для задания 6
std::vector<std::complex<double>> GenerateHarmonicSignal(size_t samples,
    double amplitudeA,
    double amplitudeB,
    double frequencyW1,
    double frequencyW2,
    double phase = 0.0)
{
    std::vector<std::complex<double>> signal(samples, { 0.0, 0.0 });

    // Инициализация генератора случайных чисел для шума
    unsigned randomSeed = std::chrono::steady_clock::now().time_since_epoch().count();
    std::mt19937 randomGenerator(randomSeed);
    std::uniform_real_distribution<double> noiseDist(-0.05, 0.05);

    for (size_t j = 0; j < samples; ++j) {
        // z(j) = A·cos(2πω₁j/N + φ) + B·cos(2πω₂j/N)
        double cleanSignal = amplitudeA * std::cos(2.0 * MATH_PI * frequencyW1 * j / samples + phase)
            + amplitudeB * std::cos(2.0 * MATH_PI * frequencyW2 * j / samples);

        // Добавление шума
        double noise = noiseDist(randomGenerator);
        signal[j] = { cleanSignal + noise, 0.0 };
    }

    return signal;
}

// Генератор кусочно-постоянного сигнала для заданий 2-5
std::vector<std::complex<double>> GeneratePiecewiseSignal(size_t samples,
    double amplitudeA,
    double amplitudeB,
    double frequencyW2)
{
    std::vector<std::complex<double>> signal(samples, { 0.0, 0.0 });

    // Инициализация генератора случайных чисел для шума
    unsigned randomSeed = std::chrono::steady_clock::now().time_since_epoch().count();
    std::mt19937 randomGenerator(randomSeed);
    std::uniform_real_distribution<double> noiseDist(-0.05, 0.05);

    const double quarterPoint = samples / 4.0;
    const double halfPoint = samples / 2.0;
    const double threeQuarterPoint = 3.0 * samples / 4.0;

    for (size_t j = 0; j < samples; ++j) {
        double cleanSignal = 0.0;

        // Куски сигнала согласно формуле
        if (j >= quarterPoint && j <= halfPoint) {
            // N/4 ≤ j ≤ N/2
            cleanSignal = amplitudeA + amplitudeB * std::cos(2.0 * MATH_PI * frequencyW2 * j / samples);
        }
        else if (j > threeQuarterPoint) {
            // 3N/4 < j ≤ N
            cleanSignal = amplitudeA + amplitudeB * std::cos(2.0 * MATH_PI * frequencyW2 * j / samples);
        }
        // В остальных случаях cleanSignal = 0

        // Добавление шума
        double noise = noiseDist(randomGenerator);
        signal[j] = { cleanSignal + noise, 0.0 };
    }

    return signal;
}

int main() {
    // Установка кодировки для корректного отображения
    system("chcp 65001 > nul");

    using namespace SpectralAnalysis;

    const int n = 9;
    const int N = pow(2, n);  // 512 отсчетов
    const int maxLevels = 4;

    // ============================================================================
    // ВАРИАНТ 1: ЗАДАНИЯ 2-5 (Кусочно-постоянный сигнал)
    // ============================================================================

    const double A = 2.44;
    const double B = 0.10;
    const int w2 = 184;

    const std::string resultsDir1 = "wavelet_results_piecewise";
    system(("mkdir " + resultsDir1).c_str());

    // Генерация кусочно-постоянного сигнала
    std::vector<std::complex<double>> piecewiseSignal = GeneratePiecewiseSignal(N, A, B, w2);

    // Сохранение исходного сигнала
    ExportSignalData(resultsDir1 + "/original_signal.csv", piecewiseSignal);

    // Многоуровневый анализ для трех типов вейвлетов
    ProcessWaveletBasis(resultsDir1, WaveletFamily::Haar, piecewiseSignal, maxLevels);
    ProcessWaveletBasis(resultsDir1, WaveletFamily::Shannon, piecewiseSignal, maxLevels);
    ProcessWaveletBasis(resultsDir1, WaveletFamily::Daubechies6, piecewiseSignal, maxLevels);


    // ============================================================================
    // ВАРИАНТ 2: ЗАДАНИЕ 6 (Гармонический сигнал)
    // ============================================================================

    const int w1 = 1;
    const double phase = MATH_PI / 4.0;

    const std::string resultsDir2 = "wavelet_results_harmonic";
    system(("mkdir " + resultsDir2).c_str());

    // Генерация гармонического сигнала
    std::vector<std::complex<double>> harmonicSignal = GenerateHarmonicSignal(N, A, B, w1, w2, phase);

    // Сохранение исходного сигнала
    ExportSignalData(resultsDir2 + "/original_signal.csv", harmonicSignal);

    // Многоуровневый анализ для трех типов вейвлетов
    ProcessWaveletBasis(resultsDir2, WaveletFamily::Haar, harmonicSignal, maxLevels);
    ProcessWaveletBasis(resultsDir2, WaveletFamily::Shannon, harmonicSignal, maxLevels);
    ProcessWaveletBasis(resultsDir2, WaveletFamily::Daubechies6, harmonicSignal, maxLevels);

    return 0;
}
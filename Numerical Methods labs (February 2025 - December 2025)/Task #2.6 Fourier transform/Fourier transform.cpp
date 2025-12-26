#include <iostream>
#include <vector>
#include <complex>
#include <cmath>
#include <chrono>
#include <algorithm>
#include <map>
#include <fstream>
#include <string>
#include <iomanip>

using namespace std;
using namespace std::chrono;

using Complex = complex<double>;
const double PI = 3.14159265358979323846;

// ==================== ПУНКТ 1: БАЗОВЫЕ ПРЕОБРАЗОВАНИЯ ====================

vector<Complex> dft(const vector<Complex>& input) {
    int N = input.size();
    vector<Complex> output(N, 0);

    for (int m = 0; m < N; m++) {
        for (int n = 0; n < N; n++) {
            double angle = -2 * PI * m * n / N;
            output[m] += input[n] * exp(Complex(0, angle));
        }
    }
    return output;
}

vector<Complex> idft(const vector<Complex>& input) {
    int N = input.size();
    vector<Complex> output(N, 0);

    for (int n = 0; n < N; n++) {
        for (int m = 0; m < N; m++) {
            double angle = 2 * PI * m * n / N;
            output[n] += input[m] * exp(Complex(0, angle));
        }
        output[n] /= N;
    }
    return output;
}

vector<Complex> fft(const vector<Complex>& input) {
    int N = input.size();

    if (N == 1) {
        return input;
    }

    int M = N / 2;

    // 1. Разделяем на четные и нечетные
    vector<Complex> u(M), v(M);
    for (int k = 0; k < M; k++) {
        u[k] = input[2 * k];      // z(2k)
        v[k] = input[2 * k + 1];  // z(2k+1)
    }

    // 2. Рекурсивно вычисляем FFT для половин
    vector<Complex> u_hat = fft(u);
    vector<Complex> v_hat = fft(v);

    // 3. Объединяем результаты
    vector<Complex> output(N);

    for (int m = 0; m < M; m++) {
        // 3. Для m = 0...M-1
        double angle = -2 * PI * m / N;
        Complex twiddle = exp(Complex(0, angle));
        output[m] = u_hat[m] + twiddle * v_hat[m];

        // 4. Для m = M...N-1
        int l = m;
        output[m + M] = u_hat[l] - twiddle * v_hat[l];
    }

    return output;
}

vector<Complex> ifft(const vector<Complex>& input) {
    int N = input.size();

    // z(-j) = z(N-j) - используем свойство периодичности
    vector<Complex> conjugated_input(N);
    for (int j = 0; j < N; j++) {
        conjugated_input[j] = conj(input[j]);
    }

    // Вычисляем FFT от сопряженного
    vector<Complex> temp = fft(conjugated_input);

    // Сопрягаем результат и делим на N
    vector<Complex> output(N);
    for (int j = 0; j < N; j++) {
        output[j] = conj(temp[j]) / double(N);
    }

    return output;
}

// ==================== ПУНКТ 2: ГЕНЕРАЦИЯ СИГНАЛОВ ====================

struct SignalParams {
    int N;
    double A, B;
    double omega1, omega2;
    double phi;
};

vector<Complex> generateSignal1(const SignalParams& params) {
    vector<Complex> signal(params.N);
    for (int j = 0; j < params.N; j++) {
        double value = params.A * cos(2 * PI * params.omega1 * j / params.N + params.phi) +
            params.B * cos(2 * PI * params.omega2 * j / params.N);
        signal[j] = value;
    }
    return signal;
}

vector<Complex> generateSignal2(const SignalParams& params) {
    vector<Complex> signal(params.N, 0);
    for (int j = params.N / 4; j <= params.N / 2; j++) {
        signal[j] = params.A + params.B * cos(2 * PI * params.omega2 * j / params.N);
    }
    for (int j = 3 * params.N / 4; j < params.N; j++) {
        signal[j] = params.A + params.B * cos(2 * PI * params.omega2 * j / params.N);
    }
    return signal;
}

// ==================== ПУНКТ 3: АНАЛИЗ И СРАВНЕНИЕ ====================

struct TimingResults {
    long long dft_time;
    long long fft_time;
};

struct AnalysisResults {
    vector<Complex> dft_result;
    vector<Complex> fft_result;
    TimingResults timing;
};

void printResultsTable(const vector<Complex>& signal, const vector<Complex>& dft_result) {
    cout << fixed << setprecision(6);
    cout << setw(4) << "m" << setw(12) << "Re z" << setw(15) << "Re z_hat"
        << setw(15) << "Im z_hat" << setw(15) << "Amplitude" << setw(12) << "Phase" << endl;
    cout << string(75, '-') << endl;

    int N = signal.size();
    double amplitude_limit = 1e-6;
    double phase_limit = 1e-6;
    int count = 0;

    for (int m = 0; m < N; m++) {
        double amplitude = abs(dft_result[m]);
        double phase = arg(dft_result[m]);

        bool significant_amplitude = (amplitude > amplitude_limit);
        bool significant_phase = (abs(phase) > phase_limit);

        if (significant_amplitude) { // вариант без  '|| significant_phase'
            cout << setw(4) << m << setw(12) << signal[m].real()
                << setw(15) << dft_result[m].real()
                << setw(15) << dft_result[m].imag()
                << setw(15) << amplitude
                << setw(12) << phase << endl;
            count++;
        }
    }

    cout << defaultfloat;
    cout << "Всего выведено компонент: " << count << " из " << N << endl;
}

AnalysisResults analyzeSignal(const vector<Complex>& signal) {
    AnalysisResults results;

    auto start_dft = high_resolution_clock::now();
    results.dft_result = dft(signal);
    auto end_dft = high_resolution_clock::now();

    auto start_fft = high_resolution_clock::now();
    results.fft_result = fft(signal);
    auto end_fft = high_resolution_clock::now();

    results.timing.dft_time = duration_cast<microseconds>(end_dft - start_dft).count();
    results.timing.fft_time = duration_cast<microseconds>(end_fft - start_fft).count();

    return results;
}

// ==================== ПУНКТ 4: ФИЛЬТРАЦИЯ ШУМА ====================

vector<Complex> filterHighFrequencies(const vector<Complex>& dft_result) {
    int N = dft_result.size();
    vector<Complex> filtered = dft_result;

    // Оставляем только низкие частоты (первые и последние 10%)
    int keep_count = N / 10; // 10% от N

    cout << "Простая фильтрация:" << endl;
    cout << "Сохраняем частоты: m = 0..." << keep_count << " и " << N - keep_count << "...N-1" << endl;
    cout << "Обнуляем все остальные частоты" << endl;

    int removed_count = 0;
    for (int k = keep_count + 1; k < N - keep_count; k++) {
        if (abs(filtered[k]) > 1e-6) {
            removed_count++;
        }
        filtered[k] = 0;
    }

    cout << "Удалено компонент с ненулевой амплитудой: " << removed_count << endl;
    return filtered;
}

// ==================== ПУНКТ 5: ВИЗУАЛИЗАЦИЯ И ЭКСПОРТ ====================

void exportToCSV(const string& filename,
    const vector<Complex>& original_signal,
    const vector<Complex>& filtered_signal,
    const vector<Complex>& dft_original,
    const vector<Complex>& dft_filtered) {
    ofstream file(filename);

    file << "index,original_signal_real,filtered_signal_real,"
        << "dft_original_real,dft_original_imag,dft_original_amplitude,"
        << "dft_filtered_real,dft_filtered_imag,dft_filtered_amplitude" << endl;

    int N = original_signal.size();
    for (int i = 0; i < N; i++) {
        file << i << ","
            << original_signal[i].real() << ","
            << filtered_signal[i].real() << ","
            << dft_original[i].real() << ","
            << dft_original[i].imag() << ","
            << abs(dft_original[i]) << ","
            << dft_filtered[i].real() << ","
            << dft_filtered[i].imag() << ","
            << abs(dft_filtered[i]) << endl;
    }

    file.close();
    cout << "Данные экспортированы в файл: " << filename << endl;
}

// ==================== ПУНКТ 6: АНАЛИЗ СИГНАЛА С РАЗРЫВАМИ ====================

void exportDiscontinuousSignal(const string& filename, const vector<Complex>& signal) {
    ofstream file(filename);

    file << "index,signal_real" << endl;

    int N = signal.size();
    for (int i = 0; i < N; i++) {
        file << i << "," << signal[i].real() << endl;
    }

    file.close();
    cout << "Данные нового сигнала экспортированы в файл: " << filename << endl;
}

void analyzeDiscontinuousSignal(const SignalParams& params) {
    cout << "\n" << string(70, '=') << "\n";
    cout << "АНАЛИЗ НОВОГО СИГНАЛА (ПУНКТ 6)" << "\n";

    // Генерируем сигнал с разрывами
    vector<Complex> signal = generateSignal2(params);

    // Экспортируем для визуализации
    exportDiscontinuousSignal("discontinuous_signal.csv", signal);
}

// ==================== ВСПОМОГАТЕЛЬНЫЕ ФУНКЦИИ ====================

void printSectionHeader(const string& title) {
    cout << "\n" << string(60, '=') << "\n";
    cout << title << "\n\n";
}

// ==================== MAIN ====================

int main() {
    setlocale(LC_ALL, "Ru");

    // Параметры сигнала
    SignalParams params;
    params.N = 512;
    params.A = 2.44;
    params.B = 0.1;
    params.omega1 = 1;
    params.omega2 = 184;
    params.phi = PI / 4;

    // ==================== ПУНКТ 2 ====================
    printSectionHeader("ПУНКТ 2: ГЕНЕРАЦИЯ СИГНАЛА");
    vector<Complex> signal = generateSignal1(params);
    cout << "Сигнал сгенерирован. N = " << params.N << " точек" << endl;

    // ==================== ПУНКТ 3 ====================
    printSectionHeader("ПУНКТ 3: АНАЛИЗ СИГНАЛА (DFT И FFT)");
    AnalysisResults analysis = analyzeSignal(signal);

    cout << "Время выполнения DFT: " << analysis.timing.dft_time << " мкс" << endl;
    cout << "Время выполнения FFT: " << analysis.timing.fft_time << " мкс" << endl;

    cout << "\nТаблица значимых компонент DFT:" << endl;
    printResultsTable(signal, analysis.dft_result);

    // ==================== ПУНКТ 4 ====================
    printSectionHeader("ПУНКТ 4: ФИЛЬТРАЦИЯ ШУМОВЫХ КОМПОНЕНТ");

    vector<Complex> filtered_dft = filterHighFrequencies(analysis.dft_result);

    // ==================== ПУНКТ 5 ====================
    printSectionHeader("ПУНКТ 5: ЭКСПОРТ ДАННЫХ ДЛЯ ВИЗУАЛИЗАЦИИ");
    vector<Complex> reconstructed = idft(filtered_dft);

    exportToCSV("signal_analysis.csv", signal, reconstructed,
        analysis.dft_result, filtered_dft);

    // ==================== ПУНКТ 6 ====================
    analyzeDiscontinuousSignal(params);

    return 0;
}
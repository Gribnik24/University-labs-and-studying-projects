#include "LinearAlgebra.h"
#include <iostream>
#include <iomanip>
#include <chrono>
#include <algorithm>
#include <vector>

// Median calculation
long long calculateMedianTime(std::vector<long long>& timings) {
    std::sort(timings.begin(), timings.end());

    if (timings.size() % 2 == 1) {
        // for odd num of measurments
        return timings[timings.size() / 2];
    }
    else {
        // for even num of measurments
        return (timings[timings.size() / 2 - 1] + timings[timings.size() / 2]) / 2;
    }
}

int main() {
    std::vector<int> sizes = { 250, 500, 1000 };
    const int num_measurements = 5;
    std::cout << std::fixed << std::setprecision(6);
    std::cout << "Size  | Method   | Median Time (ms) | Error" << std::endl;
    std::cout << "-------------------------------------------" << std::endl;

    for (int N : sizes) {
        Matrix A = createMatrix(N);
        Vector f = createRightHandSide(A);
        Vector x_exact(N, 1.0);

        // LU decomposition
        {
            std::vector<long long> timings;
            double final_error = 0.0;

            for (int i = 0; i < num_measurements; ++i) {
                auto start = std::chrono::high_resolution_clock::now();
                Matrix LU = A;
                std::vector<int> pivot;
                luDecomposition(LU, pivot);
                Vector x = solveLU(LU, pivot, f);
                auto stop = std::chrono::high_resolution_clock::now();
                auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
                timings.push_back(duration.count());
                final_error = computeError(x, x_exact);
            }

            long long median_time = calculateMedianTime(timings);
            std::cout << std::setw(5) << N << " | LU      | " << std::setw(15) << median_time
                << " | " << std::scientific << std::setprecision(3) << final_error << std::endl;
        }

        // QR decomposition
        {
            std::vector<long long> timings;
            double final_error = 0.0;

            for (int i = 0; i < num_measurements; ++i) {
                auto start = std::chrono::high_resolution_clock::now();
                Matrix Q, R;
                householderQR(A, Q, R);
                Vector x = solveQR(Q, R, f);
                auto stop = std::chrono::high_resolution_clock::now();
                auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
                timings.push_back(duration.count());
                final_error = computeError(x, x_exact);
            }

            long long median_time = calculateMedianTime(timings);
            std::cout << std::setw(5) << N << " | QR      | " << std::setw(15) << median_time
                << " | " << std::scientific << std::setprecision(3) << final_error << std::endl;
        }
        std::cout << "-------------------------------------------" << std::endl;
    }

    system("pause");
    return 0;
}
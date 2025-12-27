#include "LinearAlgebra.h"
#include <algorithm>

void luDecomposition(Matrix& A, std::vector<int>& pivot) {
    int N = A.size();
    pivot.resize(N);
    for (int i = 0; i < N; ++i) pivot[i] = i;

    for (int k = 0; k < N; ++k) {
        // Partial pivoting
        int max_row = k;
        for (int i = k + 1; i < N; ++i) {
            if (std::abs(A[i][k]) > std::abs(A[max_row][k])) {
                max_row = i;
            }
        }

        if (max_row != k) {
            std::swap(A[k], A[max_row]);
            std::swap(pivot[k], pivot[max_row]);
        }

        // LU decomposition
        for (int i = k + 1; i < N; ++i) {
            A[i][k] /= A[k][k];
            for (int j = k + 1; j < N; ++j) {
                A[i][j] -= A[i][k] * A[k][j];
            }
        }
    }
}

Vector solveLU(const Matrix& LU, const std::vector<int>& pivot, const Vector& f) {
    int N = LU.size();
    Vector x(N), b(N), y(N);

    // Apply permutation
    for (int i = 0; i < N; ++i) {
        b[i] = f[pivot[i]];
    }

    // Forward substitution (Ly = b)
    for (int i = 0; i < N; ++i) {
        y[i] = b[i];
        for (int j = 0; j < i; ++j) {
            y[i] -= LU[i][j] * y[j];
        }
    }

    // Backward substitution (Ux = y)
    for (int i = N - 1; i >= 0; --i) {
        x[i] = y[i];
        for (int j = i + 1; j < N; ++j) {
            x[i] -= LU[i][j] * x[j];
        }
        x[i] /= LU[i][i];
    }

    return x;
}
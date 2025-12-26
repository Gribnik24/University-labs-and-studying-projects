#include "LinearAlgebra.h"

Matrix createMatrix(int N) {
    Matrix A(N, Vector(N));
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            if (i == j) {
                A[i][j] = 100.0;
            }
            else {
                A[i][j] = 1.0 + (i + 1) + (j + 1);
            }
        }
    }
    return A;
}

Vector createRightHandSide(const Matrix& A) {
    int N = A.size();
    Vector f(N, 0.0);
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            f[i] += A[i][j];
        }
    }
    return f;
}

double computeError(const Vector& x, const Vector& x_exact) {
    double diff_norm = 0.0;
    double exact_norm = 0.0;
    for (size_t i = 0; i < x.size(); ++i) {
        diff_norm += (x[i] - x_exact[i]) * (x[i] - x_exact[i]);
        exact_norm += x_exact[i] * x_exact[i];
    }
    return std::sqrt(diff_norm) / std::sqrt(exact_norm);
}
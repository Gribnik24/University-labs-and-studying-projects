#include "LinearAlgebra.h"

Matrix createMatrix(int N) {
    Matrix A(N, Vector(N));
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            A[i][j] = 1.0 / (1.0 + 0.6 * (i + 1) + 2.0 * (j + 1));
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

Matrix transpose(const Matrix& A) {
    int m = A.size(), n = A[0].size();
    Matrix At(n, Vector(m));
    for (int i = 0; i < m; ++i)
        for (int j = 0; j < n; ++j)
            At[j][i] = A[i][j];
    return At;
}

Matrix multiply(const Matrix& A, const Matrix& B) {
    int m = A.size(), n = B[0].size(), p = B.size();
    Matrix C(m, Vector(n, 0));
    for (int i = 0; i < m; ++i)
        for (int j = 0; j < n; ++j)
            for (int k = 0; k < p; ++k)
                C[i][j] += A[i][k] * B[k][j];
    return C;
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


double computeConditionNumber(const Matrix& A) {
    Matrix U, V;
    Vector S;
    svdDecomposition(A, U, S, V);

    if (S.empty()) {
        return std::numeric_limits<double>::infinity();
    }

    double sigma_max = S.front();

    double sigma_min = 0.0;
    const double epsilon = 1e-12;

    for (auto it = S.rbegin(); it != S.rend(); ++it) {
        if (std::abs(*it) > epsilon) {
            sigma_min = *it;
            break;
        }
    }

    if (sigma_min == 0.0) {
        return std::numeric_limits<double>::infinity();
    }

    return sigma_max / sigma_min;
}
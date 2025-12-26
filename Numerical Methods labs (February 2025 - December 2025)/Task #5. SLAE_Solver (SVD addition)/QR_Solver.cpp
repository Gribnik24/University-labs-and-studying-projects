#include "LinearAlgebra.h"
#include <cmath>
#include <iostream>

void householderQR(const Matrix& A, Matrix& Q, Matrix& R) {
    int n = A.size();
    Q = Matrix(n, Vector(n, 0.0));
    for (int i = 0; i < n; ++i) Q[i][i] = 1.0;
    R = A;

    for (int k = 0; k < n - 1; ++k) {
        // reflection vector calculation 
        double norm = 0.0;
        for (int i = k; i < n; ++i)
            norm += R[i][k] * R[i][k];
        norm = sqrt(norm);

        // if norm ~ 0 then in beta calculation there is zero devision
        if (fabs(norm) < 1e-12) continue;

        double alpha = -copysign(norm, R[k][k]);
        Vector v(n, 0.0);
        for (int i = k; i < n; ++i)
            if (i == k) {
                v[i] = R[i][k] - alpha;
            }
            else {
                v[i] = R[i][k];
            }

        double beta = 0.0;
        for (int i = k; i < n; ++i)
            beta += v[i] * v[i];
        beta = 2.0 / beta;

        // R update
        for (int j = k; j < n; ++j) {
            double dot = 0.0;
            for (int i = k; i < n; ++i)
                dot += v[i] * R[i][j];
            for (int i = k; i < n; ++i)
                R[i][j] -= beta * v[i] * dot;
        }

        // Q update
        for (int j = 0; j < n; ++j) {
            double dot = 0.0;
            for (int i = k; i < n; ++i)
                dot += Q[j][i] * v[i];
            for (int i = k; i < n; ++i)
                Q[j][i] -= beta * v[i] * dot;
        }
    }
}


Vector solveQR(const Matrix& Q, const Matrix& R, const Vector& f) {
    int N = Q.size();

    // Compute Q^T * f  (Q is orthogonal, so Q^T = Q^H = Q.transpose())
    Vector y(N, 0.0);
    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            y[i] += Q[j][i] * f[j];
        }
    }

    // Back substitution for Rx = y
    Vector x(N, 0.0);
    for (int i = N - 1; i >= 0; --i) {
        x[i] = y[i];
        for (int j = i + 1; j < N; ++j) {
            x[i] -= R[i][j] * x[j];
        }
        x[i] /= R[i][i];
    }

    return x;
}
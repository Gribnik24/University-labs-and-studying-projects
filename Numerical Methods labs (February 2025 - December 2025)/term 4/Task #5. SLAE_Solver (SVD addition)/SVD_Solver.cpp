#include "LinearAlgebra.h"
#include <vector>
#include <cmath>
#include <algorithm>

const double SVD_EPS = 1e-14;  // increase SVD accuracy
const int MAX_ITER = 10;
const double SVD_THRESHOLD = 1e-10;

// Calculating eigen values using QR decomposition
void computeEigenvalues(const Matrix& A, Vector& eigenvalues, Matrix& eigenvectors) {
    int n = A.size();
    Matrix Ak = A;
    Matrix Q, R;
    eigenvectors = Matrix(n, Vector(n, 0.0));

    for (int i = 0; i < n; ++i) 
        eigenvectors[i][i] = 1.0;

    for (int iter = 0; iter < MAX_ITER; ++iter) {
        
        householderQR(Ak, Q, R);

        Ak = multiply(R, Q);

        Matrix temp = multiply(eigenvectors, Q);
        eigenvectors = temp;
    }

    eigenvalues.resize(n);
    for (int i = 0; i < n; ++i) {
        eigenvalues[i] = Ak[i][i];
    }

    for (int i = 0; i < n; ++i) {
        for (int j = i + 1; j < n; ++j) {
            if (eigenvalues[i] < eigenvalues[j]) {
                std::swap(eigenvalues[i], eigenvalues[j]);
                for (int k = 0; k < n; ++k)
                    std::swap(eigenvectors[k][i], eigenvectors[k][j]);
            }
        }
    }
}

void svdDecomposition(const Matrix& A, Matrix& U, Vector& S, Matrix& Vt) {
    int m = A.size();
    if (m == 0) return;
    int n = A[0].size();
    int k = std::min(m, n);

    // 1. Calculating AtA
    Matrix At = transpose(A);
    Matrix AtA = multiply(At, A);

    // 2. Calculating AtA's eigen values using QR decomposition
    Vector eigenvalues;
    Matrix V;
    computeEigenvalues(AtA, eigenvalues, V);

    // 3. Singular number - root of eigen number
    S.resize(k);
    for (int i = 0; i < k; ++i) {
        S[i] = sqrt(fabs(eigenvalues[i]));
    }

    // 4. Calculating Vt
    Vt = transpose(V);

    // 5. Calculating U like A*V*diag(S)^(-1)
    U = Matrix(m, Vector(k, 0.0));
    for (int i = 0; i < m; ++i) {
        for (int j = 0; j < k; ++j) {
            if (S[j] > SVD_EPS) {
                for (int l = 0; l < n; ++l) {
                    U[i][j] += A[i][l] * Vt[j][l] / S[j];
                }
            }
        }
    }

    // 6. New U's columns normalization
    for (int j = 0; j < k; ++j) {

        // first normalization
        double norm = 0.0;
        for (int i = 0; i < m; ++i) {
            norm += U[i][j] * U[i][j];
        }
        norm = sqrt(norm);
        if (norm > SVD_EPS) {
            for (int i = 0; i < m; ++i) {
                U[i][j] /= norm;
            }
        }

        // second normalization
        for (int p = 0; p < j; ++p) {
            double dot = 0.0;
            for (int i = 0; i < m; ++i) {
                dot += U[i][p] * U[i][j];
            }
            for (int i = 0; i < m; ++i) {
                U[i][j] -= dot * U[i][p];
            }
        }
    }
}


Vector solveSVD(const Matrix& U, const Vector& S, const Matrix& Vt, const Vector& f) {
    int m = U.size();
    int n = Vt[0].size();

    // 1. Ut * f
    Vector y(m, 0.0);
    for (int i = 0; i < m; ++i) {
        for (int j = 0; j < m; ++j) {
            y[i] += U[j][i] * f[j];
        }
    }

    // 2. diag(S)^(-1) * (Ut * f)
    double max_s = *std::max_element(S.begin(), S.end());
    double threshold = max_s * std::max(m, n) * SVD_THRESHOLD;
    for (int i = 0; i < S.size(); ++i) {
        if (S[i] > threshold) {
            y[i] /= S[i];
        }
        else {
            y[i] = 0.0;
        }
    }

    // 3. V * (diag(S)^(-1) * Ut * f)
    Vector x(n, 0.0);
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            x[i] += Vt[j][i] * (j < y.size() ? y[j] : 0.0);
        }
    }

    return x;
}

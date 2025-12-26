#pragma once
#include <vector>
#include <cmath>
#include <stdexcept>
#include <chrono>

using Matrix = std::vector<std::vector<double>>;
using Vector = std::vector<double>;

// Matrix operations
Matrix createMatrix(int N);
Vector createRightHandSide(const Matrix& A);
double computeError(const Vector& x, const Vector& x_exact);

// LU decomposition
void luDecomposition(Matrix& A, std::vector<int>& pivot);
Vector solveLU(const Matrix& LU, const std::vector<int>& pivot, const Vector& f);

// QR decomposition
void householderQR(const Matrix& A, Matrix& Q, Matrix& R);
Vector solveQR(const Matrix& Q, const Matrix& R, const Vector& f);
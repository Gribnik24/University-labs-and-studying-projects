#include <iostream>
#include <iomanip>
#include <vector>
#include <cmath>
#include "headers.h"
#include "sen_distribution.h"

using namespace std;

// По-блочно: отдельный try-catch для каждого теста
void test_sen_class() {
    using std::cout; using std::endl;

    cout << "\n=== TEST: SENDistribution class ===\n";
    cout.setf(std::ios::fixed); cout << std::setprecision(6);

    // 1) Конструктор по умолчанию + базовые вычисления
    try {
        SENDistribution d0; // mu=0, la=1, v=1
        double x = d0.mu();
        Moments m = d0.moments();
        cout << "\n[Default distribution]\n"
            << "mu=" << d0.mu() << "  lambda=" << d0.lambda() << "  v=" << d0.v() << '\n'
            << "pdf(mu)=" << d0.pdf(x) << '\n'
            << "M=" << m.M << "  D=" << m.D2 << "  g1=" << m.g1 << "  g2=" << m.g2 << '\n';
    }
    catch (const std::exception& e) {
        cout << "[Default distribution] exception: " << e.what() << '\n';
    }

    // 2) Параметрический конструктор + базовые вычисления
    SENDistribution d(0.0, 2.0, 1.0);
    try {
        double x = d.mu();
        Moments m = d.moments();
        cout << "\n[Param distribution]\n"
            << "mu=" << d.mu() << "  lambda=" << d.lambda() << "  v=" << d.v() << '\n'
            << "pdf(mu)=" << d.pdf(x) << '\n'
            << "M=" << m.M << "  D=" << m.D2 << "  g1=" << m.g1 << "  g2=" << m.g2 << '\n';
    }
    catch (const std::exception& e) {
        cout << "[Param distribution] exception: " << e.what() << '\n';
    }

    // 3) Персистентность (save → load)
    try {
        const char* path = "sen_params.txt";
        d.save(path);
        auto d2 = SENDistribution::load(path);
        cout << "\n[Persistence]\n"
            << "reloaded: mu=" << d2.mu() << "  lambda=" << d2.lambda() << "  v=" << d2.v() << '\n';
        Moments m1 = d2.moments();
        cout << "\n[Before setters]\n"
            << "mu=" << d2.mu() << "  lambda=" << d2.lambda() << "  v=" << d2.v() << '\n'
            << "pdf(mu)=" << d2.pdf(d2.mu()) << '\n'
            << "M=" << m1.M << "  D=" << m1.D2 << "  g1=" << m1.g1 << "  g2=" << m1.g2 << '\n';
    }
    catch (const std::exception& e) {
        cout << "[Persistence] exception: " << e.what() << '\n';
    }

    // 4) Setters + проверка pdf/moments после изменения
    try {
        SENDistribution d2 = d;
        d2.set_mu(1.5);
        d2.set_lambda(2.5);
        d2.set_v(3.0);
        Moments m2 = d2.moments();
        cout << "\n[After setters]\n"
            << "mu=" << d2.mu() << "  lambda=" << d2.lambda() << "  v=" << d2.v() << '\n'
            << "pdf(mu)=" << d2.pdf(d2.mu()) << '\n'
            << "M=" << m2.M << "  D=" << m2.D2 << "  g1=" << m2.g1 << "  g2=" << m2.g2 << '\n';
    }
    catch (const std::exception& e) {
        cout << "[After setters] exception: " << e.what() << '\n';
    }

    // 5) Негативные тесты (ожидаемые исключения)
    try {
        SENDistribution d2 = d;
        d2.set_lambda(0.0);
        cout << "\n[Negative lambda] ERROR: исключение не получено\n";
    }
    catch (const std::exception& e) {
        cout << "\n[Negative lambda] OK: " << e.what() << '\n';
    }

    try {
        SENDistribution bad(0.0, 1.0, -1.0);
        cout << "[Negative v] ERROR: исключение не получено\n";
    }
    catch (const std::exception& e) {
        cout << "[Negative v] OK: " << e.what() << "\n";
    }

    cout << "=== END TEST ===\n";
}

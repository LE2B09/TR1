#include "CFT.h"
#include <cmath>

Complex CFT(const std::vector<double>& x, double omega)
{
    int N = x.size();
    Complex X(0, 0);
    for (int n = 0; n < N; ++n) {
        X += x[n] * std::exp(Complex(0, omega * n));
    }
    return X;
}
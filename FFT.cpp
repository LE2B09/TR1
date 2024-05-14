#include <cmath>
#include "FFT.h"

const double PI = 3.14159265358979323846;

void butterfly(Complex* a, int n, bool invert) {
    for (int i = 0; i < n; ++i) {
        int j = 0;
        for (int bit = 0; (1 << bit) < n; ++bit) {
            j <<= 1;
            if (i & (1 << bit))
                j |= 1;
        }
        if (i < j)
            std::swap(a[i], a[j]);
    }

    for (int len = 2; len <= n; len <<= 1) {
        double ang = 2 * PI / len * (invert ? 1 : -1);  // Invert decides the sign of the angle
        Complex wlen(cos(ang), sin(ang));
        for (int i = 0; i < n; i += len) {
            Complex w(1, 0);
            for (int j = 0; j < len / 2; ++j) {
                Complex u = a[i + j], v = a[i + j + len / 2] * w;
                a[i + j] = u + v;
                a[i + j + len / 2] = u - v;
                w = w * wlen;
            }
        }
    }

    if (invert) {
        for (int i = 0; i < n; ++i) {
            a[i].real /= n;
            a[i].imag /= n;
        }
    }
}

void FFT(Complex* a, int n) {
    butterfly(a, n, false);
}

void InverseFFT(Complex* a, int n) {
    butterfly(a, n, true);

    // 非常に小さな値を0に丸める
    const double EPSILON = 1e-10;
    for (int i = 0; i < n; ++i) {
        if (fabs(a[i].real) < EPSILON) a[i].real = 0;
        if (fabs(a[i].imag) < EPSILON) a[i].imag = 0;
    }
}
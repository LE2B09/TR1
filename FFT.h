#ifndef FFT_H
#define FFT_H

#include "Complex.h"

/// <summary>
/// フーリエ変換
/// </summary>
/// <param name="a"></param>
/// <param name="n"></param>
void FFT(Complex* a, int n);

/// <summary>
/// 逆フーリエ変換
/// </summary>
/// <param name="a"></param>
/// <param name="n"></param>
void InverseFFT(Complex* a, int n);

#endif
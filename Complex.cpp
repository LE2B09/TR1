//#include "complex.h"
//
//Complex::Complex() : real(0), imag(0) {}
//
//Complex::Complex(double r, double i) : real(r), imag(i) {}
//
//Complex Complex::operator + (const Complex& other) const
//{
//	return Complex(real + other.real, imag + other.imag);
//}
//
//Complex Complex::operator - (const Complex& other) const
//{
//	return Complex(real - other.real, imag - other.imag);
//}
//
//Complex Complex::operator * (const Complex& other) const
//{
//	return Complex(real * other.real - imag * other.imag,
//		real * other.imag + imag * other.real);
//}
#ifndef COMPLEX_H
#define COMPLEX_H

class Complex {
public:
    double real;
    double imag;

    Complex() : real(0), imag(0) {}
    Complex(double r, double i) : real(r), imag(i) {}

    Complex operator+(const Complex& other) const {
        return Complex(real + other.real, imag + other.imag);
    }

    Complex operator-(const Complex& other) const {
        return Complex(real - other.real, imag - other.imag);
    }

    Complex operator*(const Complex& other) const {
        return Complex(real * other.real - imag * other.imag, real * other.imag + imag * other.real);
    }

    Complex& operator=(const Complex& other) {
        if (this == &other) {
            return *this;
        }
        real = other.real;
        imag = other.imag;
        return *this;
    }
};

#endif
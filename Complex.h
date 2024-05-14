#ifndef COMPLEX_H
#define COMPLEX_H

class Complex {
public:
	double real;    //実部
	double imag;    //虚部

	/// <summary>
	/// デフォルトコンストラクタ、実部と虚部を0で初期化
	/// </summary>
	Complex() : real(0), imag(0) {}

	/// <summary>
	/// 実部 r と虚部 i を指定して複素数を初期化するコンストラクタ
	/// </summary>
	/// <param name="r"></param>
	/// <param name="i"></param>
	Complex(double r, double i) : real(r), imag(i) {}

	//2つの複素数の加算を行う
	Complex operator+(const Complex& other) const
	{
		//加算された複素数を返す
		return Complex(real + other.real, imag + other.imag);
	}

	//2つの複素数の減算を行う
	Complex operator-(const Complex& other) const
	{
		//減算された複素数を返す
		return Complex(real - other.real, imag - other.imag);
	}

	//2つの複素数の乗算を行う
	Complex operator*(const Complex& other) const
	{
		//乗算された複素数を返す
		return Complex(real * other.real - imag * other.imag, real * other.imag + imag * other.real);
	}

	//他の複素数からこの複素数に代入を行う
	Complex& operator=(const Complex& other)
	{
		if (this == &other)
		{
			return *this;
		}
		real = other.real;
		imag = other.imag;

		//代入後の複素数への参照を返す
		return *this;
	}
};

#endif
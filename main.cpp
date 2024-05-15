#include <Novice.h>
#include <iostream>
#include "Complex.h"
#include "FFT.h"
#include <cmath>
#include <vector>

#define PI 3.141592653589793238462643383279502884

std::vector<Complex> GenerateSineWave(int numSamples, double frequency, double sampleRate)
{
	std::vector<Complex> wave(numSamples);
	for (int i = 0; i < numSamples; ++i)
	{
		double t = i / sampleRate;
		wave[i].real = sin(2 * PI * frequency * t);
		wave[i].imag = 0;
	}
	return wave;
}

void DrawWave(const std::vector<Complex>& wave, int offsetX, int offsetY, int scaleX, int scaleY)
{
	for (int i = 1; i < wave.size(); ++i)
	{
		int x1 = offsetX + (i - 1) * scaleX;
		int y1 = static_cast<int>(offsetY - wave[i - 1].real * scaleY);
		int x2 = offsetX + i * scaleX;
		int y2 = static_cast<int>(offsetY - wave[i].real * scaleY);
		Novice::DrawLine(x1, y1, x2, y2, 0xFFFFFFFF);
	}
}

const char kWindowTitle[] = "提出用課題";

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, 1280, 720);

	// キー入力結果を受け取る箱
	char keys[256] = { 0 };
	char preKeys[256] = { 0 };

	const int N = 1024; // 波形のサンプル数
	const double frequency = 0.5; // 周波数
	const double sampleRate = 100.0; // サンプルレート

	std::vector<Complex> wave = GenerateSineWave(N, frequency, sampleRate);

	// ウィンドウの×ボタンが押されるまでループ
	while (Novice::ProcessMessage() == 0)
	{
		// フレームの開始
		Novice::BeginFrame();

		// キー入力を受け取る
		memcpy(preKeys, keys, 256);
		Novice::GetHitKeyStateAll(keys);

		///
		/// ↓更新処理ここから
		///

		// 特定のキーが押されたらFFTと逆FFTを計算する
		if (keys[DIK_SPACE] || !preKeys[DIK_SPACE])
		{
			FFT(wave.data(), N);
		}

		if (keys[DIK_RETURN] || !preKeys[DIK_RETURN])
		{
			InverseFFT(wave.data(), N);
		}

		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///

		DrawWave(wave, 100, 360, 1, 100);

		///
		/// ↑描画処理ここまで
		///

		// フレームの終了
		Novice::EndFrame();

		// ESCキーが押されたらループを抜ける
		if (preKeys[DIK_ESCAPE] == 0 && keys[DIK_ESCAPE] != 0)
		{
			break;
		}
	}

	// ライブラリの終了
	Novice::Finalize();
	return 0;
}

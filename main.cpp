#include <Novice.h>
#include <iostream>
#include "Complex.h"
#include "FFT.h"

const char kWindowTitle[] = "提出用課題";

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{

	// ライブラリの初期化
	Novice::Initialize(kWindowTitle, 1280, 720);

	// キー入力結果を受け取る箱
	char keys[256] = { 0 };
	char preKeys[256] = { 0 };

	const int N = 8;
	Complex originalData[N] = { {0,0},{1,0},{2,0},{3,0},{4,0},{5,0},{6,0},{7,0} };
	Complex FFTData[N];
	Complex IFFTData[N];

	bool FFTComputed = false;
	bool inverseFFTComputed = false;

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

		if (keys[DIK_SPACE] && !FFTComputed) {
			for (int i = 0; i < N; ++i) {
				FFTData[i] = originalData[i];
			}
			FFT(FFTData, N);
			FFTComputed = true;
		}

		if (keys[DIK_RETURN] && FFTComputed && !inverseFFTComputed) {
			for (int i = 0; i < N; ++i) {
				IFFTData[i] = FFTData[i];
			}
			InverseFFT(IFFTData, N);
			inverseFFTComputed = true;
		}

		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///

		if (FFTComputed) {
			Novice::ScreenPrintf(0, 0, "FFT:");
			for (int i = 0; i < N; ++i) {
				Novice::ScreenPrintf(0, 20 * (i + 1), "FFTData[%d]: (%.5f, %.5f)", i, FFTData[i].real, FFTData[i].imag);
			}
		}

		if (inverseFFTComputed) {
			Novice::ScreenPrintf(0, 20 * (N + 1), "Inverse FFT:");
			for (int i = 0; i < N; ++i) {
				Novice::ScreenPrintf(0, 20 * (N + 2 + i), "IFFTData[%d]: (%.5f, %.5f)", i, IFFTData[i].real, IFFTData[i].imag);
			}
		}

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

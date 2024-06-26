#include <Novice.h>
#include <iostream>
#include "Complex.h"
#include "FFT.h"
#include <cmath>
#include <vector>
#include <imgui.h>
#include <string>

#include <al.h>
#include <alc.h>
#include <iostream>

#define SAMPLINGRATE 44100 
#define BUFFER_SIZE 4410
#pragma comment(lib, "C:/Program Files (x86)/OpenAL 1.1 SDK/libs/Win64/OpenAL32.lib")

#define PI 3.141592653589793238462643383279502884

using namespace std;

// 正弦波を生成する関数
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

// 波形を描画する関数
void DrawWave(const std::vector<Complex>& wave, int offsetX, int offsetY, int scaleX, int scaleY)
{
	for (int i = 1; i < wave.size(); ++i)
	{
		int x1 = offsetX + (i - 1) * scaleX;
		int y1 = static_cast<int>(offsetY - wave[i - 1].real * scaleY);
		int x2 = offsetX + i * scaleX;
		int y2 = static_cast<int>(offsetY - wave[i].real * scaleY);
		std::cout << "Drawing line from (" << x1 << ", " << y1 << ") to (" << x2 << ", " << y2 << ")" << std::endl;
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

	int Sound1 = Novice::LoadAudio("./c00002_kamatamago_Chopin_Etude-op10-3.mp3");
	int voiceHandle = -1;


	std::vector<Complex> wave(N);

	//マイクセットアップ
	ALCdevice* mic = alcCaptureOpenDevice(NULL, SAMPLINGRATE, AL_FORMAT_MONO16, BUFFER_SIZE);
	if (!mic) {
		std::cerr << "Failed to open microphone device" << std::endl;
		return -1;
	}
	//スピーカーセットアップ
	ALCdevice* speaker = alcOpenDevice(NULL);
	if (!speaker) {
		std::cerr << "Failed to open speaker device" << std::endl;
		alcCaptureCloseDevice(mic);
		return -1;
	}
	//いつもの
	ALCcontext* context = alcCreateContext(speaker, NULL);
	if (!context) {
		std::cerr << "Failed to create OpenAL context" << std::endl;
		alcCloseDevice(speaker);
		alcCaptureCloseDevice(mic);
		return -1;
	}
	alcMakeContextCurrent(context);

	//バッファ(保存領域)とソース(音源)を宣言
	//ストリーミング用にバッファを二つ
	ALuint buffer[2];
	ALuint source;
	//それを生成
	alGenBuffers(2, &buffer[0]);
	alGenSources(1, &source);

	//マイクから録音した音を一旦入れておく
	ALshort* store = new ALshort[N];

	//再生状態を監視するための準備
	alBufferData(buffer[0], AL_FORMAT_MONO16, &store[0], 0, SAMPLINGRATE);
	alSourceQueueBuffers(source, 1, &buffer[0]);
	alSourcePlay(source);

	//録音開始
	alcCaptureStart(mic);

	//録音と再生を制御
	int a = 0, count = 240;
	ALint sample, state;

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

		if (Novice::IsPlayingAudio(voiceHandle) == 0 || voiceHandle == -1)
		{
			voiceHandle = Novice::PlayAudio(Sound1, true, 0.5f);
		}

		//sourceが再生中か確認
		alGetSourcei(source, AL_BUFFERS_PROCESSED, &state);
		//録音可能なバッファ長を取得
		alcGetIntegerv(mic, ALC_CAPTURE_SAMPLES, sizeof(sample), &sample);
		//再生が終わり、録音が可能なとき
		if (sample > 0 && state == 1) {
			//録音してstoreに格納
			if (sample > N) sample = N; // バッファオーバーフローを防ぐ
			alcCaptureSamples(mic, (ALCvoid*)&store[0], sample);

			// デバッグ: 録音データを表示
			for (int i = 0; i < sample; ++i) {
				std::cout << "Sample[" << i << "] = " << store[i] << std::endl;
			}

			//再生バッファをソースから外す
			alSourceUnqueueBuffers(source, 1, &buffer[a]);
			//待機バッファをソースに適用
			alSourceQueueBuffers(source, 1, &buffer[a ^ 1]);
			//再生
			alSourcePlay(source);
			//待機バッファに録音した音を入れる
			alBufferData(buffer[a], AL_FORMAT_MONO16, &store[0], sample * sizeof(ALshort), SAMPLINGRATE);
			//ここでバッファの切り替え
			a = a ^ 1;
			//count--;

			// FFTにかけるためのwaveデータを準備
			for (int i = 0; i < sample; ++i) {
				wave[i].real = store[i] / 32768.0; // 16ビットPCMの正規化
				wave[i].imag = 0;
			}

			// 残りの部分をゼロクリア
			for (int i = sample; i < N; ++i) {
				wave[i].real = 0;
				wave[i].imag = 0;
			}

			// FFTを計算
			FFT(wave.data(), N);

			// デバッグ: FFTの結果を表示
			for (int i = 0; i < N; ++i) {
				std::cout << "FFT[" << i << "] = (" << wave[i].real << ", " << wave[i].imag << ")" << std::endl;
			}
		}

		if (count == 0) {
			break;
		}

		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///

		Novice::DrawBox(0, 0, 1280, 720, 0.0f, BLACK, kFillModeSolid);

		DrawWave(wave, 100, 360, 1, 100); // scaleYを大きくして波形を見やすくする

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

	//マイク停止
	alcCaptureStop(mic);

	//バッファ・ソースの後始末
	alDeleteBuffers(2, &buffer[0]);
	alDeleteSources(1, &source);

	//OpenALの後始末
	alcMakeContextCurrent(NULL);
	alcDestroyContext(context);
	//マイクを閉じる
	alcCloseDevice(mic);
	//スピーカを閉じる
	alcCloseDevice(speaker);

	delete[] store;

	// ライブラリの終了
	Novice::Finalize();
	return 0;
}

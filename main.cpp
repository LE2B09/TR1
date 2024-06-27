#include <Novice.h>
#include <iostream>
#include "Complex.h"
#include "FFT.h"
#include <cmath>
#include <vector>
#include <imgui.h>
#include <string>
#include <fstream>

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

void SaveWavFile(const std::string& filename, const std::vector<ALshort>& data, int sampleRate)
{
	std::ofstream file(filename, std::ios::binary);

	// WAVファイルのヘッダー情報を記述
	file.write("RIFF", 4);
	int fileSize = 36 + static_cast<int>(data.size() * sizeof(ALshort));
	file.write(reinterpret_cast<const char*>(&fileSize), 4);
	file.write("WAVE", 4);
	file.write("fmt ", 4);
	int fmtChunkSize = 16;
	file.write(reinterpret_cast<const char*>(&fmtChunkSize), 4);
	short audioFormat = 1; // PCM
	file.write(reinterpret_cast<const char*>(&audioFormat), 2);
	short numChannels = 1; // モノラル
	file.write(reinterpret_cast<const char*>(&numChannels), 2);
	file.write(reinterpret_cast<const char*>(&sampleRate), 4);
	int byteRate = sampleRate * numChannels * sizeof(ALshort);
	file.write(reinterpret_cast<const char*>(&byteRate), 4);
	short blockAlign = numChannels * sizeof(ALshort);
	file.write(reinterpret_cast<const char*>(&blockAlign), 2);
	short bitsPerSample = 16;
	file.write(reinterpret_cast<const char*>(&bitsPerSample), 2);
	file.write("data", 4);
	int dataChunkSize = static_cast<int>(data.size() * sizeof(ALshort));
	file.write(reinterpret_cast<const char*>(&dataChunkSize), 4);

	// WAVファイルの音声データを書き込む
	file.write(reinterpret_cast<const char*>(data.data()), data.size() * sizeof(ALshort));
}

bool LoadWavFile(const std::string& filename, std::vector<char>& buffer, ALenum& format, ALsizei& freq)
{
	std::ifstream file(filename, std::ios::binary);
	if (!file) return false;

	char chunkId[4];
	file.read(chunkId, 4);
	if (std::strncmp(chunkId, "RIFF", 4) != 0) return false;

	file.seekg(4, std::ios::cur); // ファイルサイズをスキップ

	file.read(chunkId, 4);
	if (std::strncmp(chunkId, "WAVE", 4) != 0) return false;

	file.read(chunkId, 4);
	if (std::strncmp(chunkId, "fmt ", 4) != 0) return false;

	int chunkSize;
	file.read(reinterpret_cast<char*>(&chunkSize), 4);
	file.seekg(chunkSize, std::ios::cur); // fmtチャンクをスキップ

	file.read(chunkId, 4);
	if (std::strncmp(chunkId, "data", 4) != 0) return false;

	int dataSize;
	file.read(reinterpret_cast<char*>(&dataSize), 4);
	buffer.resize(dataSize);
	file.read(buffer.data(), dataSize);

	format = AL_FORMAT_MONO16; // モノラル、16ビット
	freq = SAMPLINGRATE;

	return true;
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

	const int N = 768; // 波形のサンプル数

	int Sound1 = Novice::LoadAudio("./c00002_kamatamago_Chopin_Etude-op10-3.mp3");
	int voiceHandle = -1;

	std::vector<Complex> wave(N);

	// 録音と再生の状態管理
	bool isRecording = false;
	bool isPlaying = false;

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
	ALuint buffer[1];
	ALuint source;
	//それを生成
	alGenBuffers(1, &buffer[0]);
	alGenSources(1, &source);

	//マイクから録音した音を一旦入れておく
	ALshort* store = new ALshort[N];
	std::vector<ALshort> recordedData; // 録音データを保存するベクター

	//録音開始
	alcCaptureStart(mic);

	//録音と再生を制御
	ALint sample = 0, state;

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
			voiceHandle = Novice::PlayAudio(Sound1, true, 0.0f);
		}

		// ImGuiウィジェットの描画
		ImGui::Begin("Control Panel");

		if (ImGui::Button("Start Recording")) {
			isRecording = true;
			recordedData.clear(); // 録音データをクリア
			alcCaptureStart(mic);
		}

		if (ImGui::Button("Stop Recording")) {
			isRecording = false;
			alcCaptureStop(mic);

			// 録音データをWAVファイルに保存
			SaveWavFile("recorded_audio.wav", recordedData, SAMPLINGRATE);
		}

		if (ImGui::Button("Play Recorded Audio")) {
			isPlaying = true;

			// WAVファイルを読み込む
			std::vector<char> audioData;
			ALenum format;
			ALsizei freq;
			if (LoadWavFile("recorded_audio.wav", audioData, format, freq)) {
				alBufferData(buffer[0], format, audioData.data(), static_cast<ALsizei>(audioData.size()), freq);
				alSourcei(source, AL_BUFFER, buffer[0]);
				alSourcePlay(source);
			}
			else {
				std::cerr << "Failed to load WAV file" << std::endl;
			}
		}

		ImGui::End();

		// 録音処理
		if (isRecording) {
			// 録音可能なバッファ長を取得
			alcGetIntegerv(mic, ALC_CAPTURE_SAMPLES, sizeof(sample), &sample);
			//録音してstoreに格納
			if (sample > N) sample = N; // バッファオーバーフローを防ぐ
			alcCaptureSamples(mic, (ALCvoid*)&store[0], sample);

			// 録音データを保存
			recordedData.insert(recordedData.end(), store, store + sample);
		}

		// 再生状態を監視
		alGetSourcei(source, AL_SOURCE_STATE, &state);

		if (isPlaying && state != AL_PLAYING) {
			// 再生が終わったらフラグをリセット
			isPlaying = false;
		}

		///
		/// ↑更新処理ここまで
		///

		///
		/// ↓描画処理ここから
		///

		Novice::DrawBox(0, 0, 1280, 720, 0.0f, BLACK, kFillModeSolid);

		// 録音中の波形を描画
		if (isRecording) {
			for (int i = 0; i < sample; ++i) {
				wave[i].real = store[i] / 32768.0; // 16ビットPCMの正規化
				wave[i].imag = 0;
			}

			// 残りの部分をゼロクリア
			for (int i = sample; i < N; ++i) {
				wave[i].real = 0;
				wave[i].imag = 0;
			}

			DrawWave(wave, 250, 360, 1, 100); // scaleYを大きくして波形を見やすくする
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

	//マイク停止
	alcCaptureStop(mic);

	//バッファ・ソースの後始末
	alDeleteBuffers(1, &buffer[0]);
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

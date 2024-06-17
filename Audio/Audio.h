#pragma once

#include <wrl.h>
#include <xaudio2.h>

#include <cmath>
#include <map>
#include <string>

using namespace Microsoft::WRL;


/// <summary>
/// 音声データ
/// </summary>
struct SoundData {
	WAVEFORMATEX wfex;			//- 波形フォーマット
	BYTE* pBuffer;				//- バッファの先頭アドレス
	unsigned int bufferSize;	//- バッファのサイズ
};

/// <summary>
/// 音声
/// </summary>
class Audio {
public:

	/// <summary>
	/// インスタンスの確保
	/// </summary>
	/// <returns></returns>
	static Audio* GetInstance();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 終了処理
	/// </summary>
	void Finalize();

	/// <summary>
	/// 音声ファイルの読み込み
	/// </summary>
	/// <param name="fileName">音声ファイルの名前</param>
	void Load(const std::string& fileName, const std::string& newKey);

	/// <summary>
	/// 音声の再生
	/// </summary>
	/// <param name="key"></param>
	void PlayAudio(const std::string& key);

private:
	
	/// <summary>
	/// .waveファイルの読み込み
	/// </summary>
	/// <param name="fileName">.waveファイルへのDirectoryPath</param>
	/// <returns>.wave内の音声データ</returns>
	SoundData SoundLoadWave(const char* fileName);

	/// <summary>
	/// 音声の再生
	/// </summary>
	/// <param name="soundData">音声データの構造体</param>
	void SoundPlayAudio(const SoundData& soundData);

	/// <summary>
	/// 音声データの消去
	/// </summary>
	/// <param name="soundData"></param>
	void ClearSoundData(SoundData* soundData);

private:

	ComPtr<IXAudio2> xAudio2_;
	IXAudio2MasteringVoice* masterVoice_;

	std::map<std::string, SoundData> soundDatas_;
	std::string directoryPath_ = "./Resources/Audios/";

private:
	Audio() = default;
	~Audio() = default;
	Audio(const Audio&) = delete;
	Audio& operator= (const Audio&) = delete;
};
#include <Audio.h>


#include <cassert>
#include <fstream>

#pragma comment(lib, "xaudio2.lib")

namespace {

	/// <summary>
	/// チャンクヘッダ
	/// </summary>
	struct ChunkHeader {
		char id[4];			//- チャンク毎のID
		int32_t size;		//- チャンクサイズ
	};

	/// <summary>
	/// RIFFヘッダチャンク
	/// </summary>
	struct RiffHeader {
		ChunkHeader chunk;	//- "RIFF"
		char type[4];		//- "WAVE"
	};

	/// <summary>
	/// FMTチャンク
	/// </summary>
	struct FormatChunk {
		ChunkHeader chunk;	//- "fmt"
		WAVEFORMATEX fmt;		//- 波形フォーマット
	};

}

Audio* Audio::GetInstance() {
	static Audio instance;
	return &instance;
}

void Audio::Initialize() {
	HRESULT result = S_FALSE;

	///- xAudioインスタンスの生成
	result = XAudio2Create(&xAudio2_, 0, XAUDIO2_DEFAULT_PROCESSOR);
	assert(SUCCEEDED(result));

	///- MasterVoiceの生成
	result = xAudio2_->CreateMasteringVoice(&masterVoice_);
	assert(SUCCEEDED(result));

}

void Audio::Finalize() {
	for(auto& soundData : soundDatas_) {
		ClearSoundData(&soundData.second);
	}
	xAudio2_.Reset();
}

void Audio::Load(const std::string& fileName, const std::string& newKey) {
	///- すでに読み込まれている音声か確認
	if(soundDatas_.find(newKey) == soundDatas_.end()) {
		soundDatas_[newKey] = SoundLoadWave((directoryPath_ + fileName).c_str());
	}
}

void Audio::PlayAudio(const std::string& key) {
	if(soundDatas_.find(key) == soundDatas_.end()) { return; }
	SoundPlayAudio(soundDatas_.at(key));
}

SoundData Audio::SoundLoadWave(const char* fileName) {
	HRESULT result = S_FALSE;

	///- ファイルオープン
	std::ifstream file;
	file.open(fileName, std::ios_base::binary); //- .wavをバイナリモードで開く
	assert(file.is_open());


	///- .wavデータの読み込み
	RiffHeader riff;
	file.read((char*)&riff, sizeof(riff));

	///- ファイルがRIFFかどうか確認する
	if(strncmp(riff.chunk.id, "RIFF", 4) != 0) { assert(false); }
	///- ファイルがWAVEかチェック
	if(strncmp(riff.type, "WAVE", 4) != 0) { assert(false); }

	FormatChunk format{};
	file.read((char*)&format, sizeof(ChunkHeader));

	///- チャンクヘッダの確認
	if(strncmp(format.chunk.id, "fmt ", 4) != 0) { assert(false); }

	///- チャンク本体の読み込み
	assert(format.chunk.size <= sizeof(format.fmt));
	file.read((char*)&format.fmt, format.chunk.size);

	ChunkHeader data;
	file.read((char*)&data, sizeof(data));

	///- JUNKチャンクを検出した場合
	if(strncmp(data.id, "JUNK", 4) == 0) {

		file.seekg(data.size, std::ios_base::cur);
		file.read((char*)&data, sizeof(data));
	}

	if(strncmp(data.id, "data", 4) != 0) { assert(false); }

	///- Dataチャンクのデータ部(波形データ)の読み込み
	char* pBuffer = new char[data.size];
	file.read(pBuffer, data.size);

	///- ファイルクローズ
	file.close();

	///- 読み込んだ音声データをreturn
	SoundData soundData{};
	soundData.wfex = format.fmt;
	soundData.pBuffer = reinterpret_cast<BYTE*>(pBuffer);
	soundData.bufferSize = data.size;

	return soundData;
}

void Audio::SoundPlayAudio(const SoundData& soundData) {
	HRESULT result = S_FALSE;

	///- 波形フォーマットを元にSourceVoiceの生成
	IXAudio2SourceVoice* pSourceVoice = nullptr;
	result = xAudio2_->CreateSourceVoice(&pSourceVoice, &soundData.wfex);
	assert(SUCCEEDED(result));

	///- 再生する波形データの設定
	XAUDIO2_BUFFER buffer{};
	buffer.pAudioData = soundData.pBuffer;
	buffer.AudioBytes = soundData.bufferSize;
	buffer.Flags = XAUDIO2_END_OF_STREAM;

	///- 波形データの再生
	pSourceVoice->SubmitSourceBuffer(&buffer);
	pSourceVoice->Start();

}

void Audio::ClearSoundData(SoundData* soundData) {
	delete[] soundData->pBuffer;
	soundData->pBuffer = 0;
	soundData->bufferSize = 0;
	soundData->wfex = {};
}



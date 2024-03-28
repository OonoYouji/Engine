#include <DirectXCommon.h>

#include <WinApp.h>
#include <Engine.h>

#include <format>

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")


using namespace Microsoft::WRL;

/// ---------------------------
/// ↓ 初期化を行う
/// ---------------------------
void DirectXCommon::Initialize(WinApp* winApp) {

	p_winApp_ = winApp;

	InitializeDXGIDevice();

}



/// ---------------------------
/// ↓ メンバ変数の解放などを行う
/// ---------------------------
void DirectXCommon::Finalize() {}



/// ---------------------------
/// ↓ インスタンスを生成; 返す
/// ---------------------------
DirectXCommon* DirectXCommon::GetInstance() {
	static DirectXCommon instance;
	return &instance;
}

void DirectXCommon::InitializeDXGIDevice() {
	HRESULT result = S_FALSE;

	/// ---------------------------
	/// ↓ DXGIFactoryを生成; 失敗したらassertで止める
	/// ---------------------------
	dxgiFactory_ = nullptr;
	result = CreateDXGIFactory(IID_PPV_ARGS(&dxgiFactory_));
	assert(SUCCEEDED(result));



	/// ---------------------------
	/// ↓ Adapterを生成; 
	/// ---------------------------
	useAdapter_ = nullptr;

	///- 良い順にアダプタを頼む
	for(UINT i = 0; dxgiFactory_->EnumAdapterByGpuPreference(i,DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&useAdapter_))!= DXGI_ERROR_NOT_FOUND; i++) {

		///- アダプターの情報を取得する
		DXGI_ADAPTER_DESC3 desc;

		result = useAdapter_->GetDesc3(&desc);
		assert(SUCCEEDED(result));

		///- ソフトウェアアダプタでなければ採用
		if(!(desc.Flags & DXGI_ADAPTER_FLAG3_SOFTWARE)) {
			///- 採用したアダプタ情報をログに出力;
			Engine::ConsolePrint(std::format(L"Use Adapter:{}\n", desc.Description));
			break;
		}

		///- ソフトフェアアダプタのときは無視
		useAdapter_ = nullptr;
	}

	///- for()が終了してもadapterが取得できなかった場合起動できない
	assert(useAdapter_ != nullptr);




	/// ---------------------------
	/// ↓ Deviceを生成; 
	/// ---------------------------
	device_ = nullptr;

	///- 機能レベルを高い順に用意
	const char* featureLevelStaring[] = { "12.2", "12.1","12.0" };
	D3D_FEATURE_LEVEL featureLevels[] = {
		D3D_FEATURE_LEVEL_12_2,
		D3D_FEATURE_LEVEL_12_1,
		D3D_FEATURE_LEVEL_12_0
	};

	///- 高い順に生成できるか試す
	for(size_t i = 0; i < _countof(featureLevels); i++) {
		// 生成
		result = D3D12CreateDevice(useAdapter_.Get(), featureLevels[i], IID_PPV_ARGS(&device_));
		// 指定した機能れhベルで生成できたか確認
		if(SUCCEEDED(result)) {
			// 生成後にログ出力
			Engine::ConsolePrint(std::format("FeatureLevel : {}\n", featureLevelStaring[i]));
			break;
		}
	}

	///- 生成できたか確認; 生成出来ていたらログ出力する
	assert(device_ != nullptr);
	Engine::ConsolePrint("Compile create D3D12Device!!!\n");

}

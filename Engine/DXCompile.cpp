#include <DXCompile.h>

#include <Engine.h>
#include <Vector4.h>
#include <Matrix4x4.h>
#include <DirectXCommon.h>
#include <d3dx12.h>
#include <vector>

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "Winmm.lib")
#pragma comment(lib, "dxcompiler.lib")



DXCompile::DXCompile() { }
DXCompile::~DXCompile() { Finalize(); }

DXCompile* DXCompile::GetInstance() {
	static DXCompile instance;
	return &instance;
}

void DXCompile::Initialize() {

	p_directXCommon_ = DirectXCommon::GetInstance();

	/// DXCの初期化
	InitializeDXC();

	InitializeDescriptorRange();

	/// ルートシグネチャの生成
	CreateRootSignature();

	/// インプットレイアウトの設定
	SetingInputLayout();

	/// ブレンドステートの設定
	SetingBlendState();

	/// ラスタライザーステートの設定
	SetingRasterizerState();

	/// 
	SetingShader();

	/// 
	CreatePSO();

	/// VertexResourceの生成
	CreateVertexResource();

	/// VertexBufferViewの生成
	CreateVBV();

	InitializeViewport();

	WriteVertexData(
		{ -0.5f,-0.5f,0.0f,1.0f },
		{ 0.0f,0.5f,0.0f,1.0f },
		{ 0.5f,-0.5f,0.0f,1.0f }
	);
	/// マテリアルリソースの生成
	CreateMaterialResource();


	wvpResource_ = CreateBufferResource(sizeof(Matrix4x4));

	/// srvDescriptorHeapの設定
	srvDescriptorHeap_ = p_directXCommon_->CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 128, true);

	/// Textureを読んで転送する
	mipImages_ = LoadTexture("./Resources/Images/uvChecker.png");
	metaData_ = mipImages_.GetMetadata();
	textureResource_ = CreateTextureResource(metaData_);
	//UploadTextureData(textureResource_, mipImages_);


	CreateShaderResourceView();
	intermediateResource_ = UploadTextureData(textureResource_, mipImages_);

}

void DXCompile::Finalize() {

	intermediateResource_->Release();

	srvDescriptorHeap_->Release();
	textureResource_->Release();
	vertexResource_->Release();
	materialResource_->Release();
	wvpResource_->Release();

	graphicsPipelineState_->Release();
	signatureBlob_->Release();

	if (errorBlob_) {
		errorBlob_->Release();
	}

	rootSignature_->Release();
	pixelShaderBlob_->Release();
	vertexShaderBlob_->Release();



	dxcCompiler_->Release();
	dxcUtils_->Release();
	includeHandler_->Release();

	//vertexResource_->Release();



}



IDxcBlob* DXCompile::CompileShader(const std::wstring& filePath, const wchar_t* profile) {

	HRESULT hr = S_FALSE;

	Engine::ConsolePrint(std::format(L"Begin Compile, path:{}, profile:{}\n", filePath, profile));

	/// hlslファイルを読み込む
	IDxcBlobEncoding* shaderSource = nullptr;
	hr = dxcUtils_->LoadFile(filePath.c_str(), nullptr, &shaderSource);
	assert(SUCCEEDED(hr));

	/// ファイルの内容を設定する
	DxcBuffer shaderSourceBuffer;
	shaderSourceBuffer.Ptr = shaderSource->GetBufferPointer();
	shaderSourceBuffer.Size = shaderSource->GetBufferSize();
	shaderSourceBuffer.Encoding = DXC_CP_UTF8; // UTF8の文字コードであることを通知

	LPCWSTR arguments[] = {
		filePath.c_str(),			/// コンパイル対象のhlslファイル名
		L"-E", L"main",				/// エントリーポイントの指定。基本的にmain以外指定しない
		L"-T", profile,				/// ShaderProfileの設定
		L"-Zi", L"-Qembed_debug",	/// デバッグ用の情報を埋め込む
		L"-Od",						/// 最適化を外しておく	
		L"-Zpr",					/// メモリのレイアウトは行優先
	};

	/// 実際にShaderをコンパイルする
	IDxcResult* shaderResult = nullptr;
	hr = dxcCompiler_->Compile(
		&shaderSourceBuffer,
		arguments,
		_countof(arguments),
		includeHandler_,
		IID_PPV_ARGS(&shaderResult)
	);
	assert(SUCCEEDED(hr));


	/// 警告・エラーが出ていたらログに出して止める
	IDxcBlobUtf8* shaderError = nullptr;
	shaderResult->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&shaderError), nullptr);
	if (shaderError != nullptr && shaderError->GetStringLength() != 0) {
		Engine::ConsolePrint(shaderError->GetStringPointer());
		assert(false);
	}

	/// コンパイル結果から実行用のバイナリ部分を取得
	IDxcBlob* shaderBlob = nullptr;
	hr = shaderResult->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&shaderBlob), nullptr);
	assert(SUCCEEDED(hr));

	/// 成功したログの出力
	Engine::ConsolePrint(std::format(L"Compile Succeded, path:{}, prifile:{}\n", filePath, profile));

	/// リソースの解放
	shaderSource->Release();
	shaderResult->Release();
	shaderError->Release();

	return shaderBlob;
}


void DXCompile::TestDraw(const Vector4& v1, const Vector4& v2, const Vector4& v3, const Vec3f& scale, const Vec3f& rotate, Vec3f& translate) {

	ID3D12GraphicsCommandList* commandList = p_directXCommon_->GetCommandList();

	commandList->RSSetViewports(1, &viewport_);
	commandList->RSSetScissorRects(1, &scissorRect_);

	/// RootSignatureを設定; PS0に設定しているけど別途設定が必要
	commandList->SetGraphicsRootSignature(rootSignature_);
	commandList->SetPipelineState(graphicsPipelineState_);

	v1;
	v2;
	v3;
	//WriteVertexData(v1, v2, v3);
	commandList->IASetVertexBuffers(0, 1, &vertexBufferView_);

	/// 形状を設定; PS0に設定している物とはまた別; 同じものを設定すると考えておけばいい
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	/// マテリアルCBufferの場所を設定
	commandList->SetGraphicsRootConstantBufferView(0, materialResource_->GetGPUVirtualAddress());

	/// wvp用のCBufferの場所を指定
	CreateWVPResource(scale, rotate, translate);
	commandList->SetGraphicsRootConstantBufferView(1, wvpResource_->GetGPUVirtualAddress());

	/// SRVのDescriptorTableの先頭を設定; 2はrootParameter[2]である;
	commandList->SetGraphicsRootDescriptorTable(2, textureSrvHandleGPU_);


	/// 描画! (DrawCall/ドローコール); 3頂点で1つのインスタンス; インスタンスについては今後
	commandList->DrawInstanced(3, 1, 0, 0);

}

//ID3D12Resource* DXCompile::UploadTextureData() {
//	return UploadTextureData(textureResource_, mipImages_);
//}

void DXCompile::InitializeDXC() {

	HRESULT hr = S_FALSE;

	hr = DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&dxcUtils_));
	assert(SUCCEEDED(hr));

	hr = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&dxcCompiler_));
	assert(SUCCEEDED(hr));

	hr = dxcUtils_->CreateDefaultIncludeHandler(&includeHandler_);
	assert(SUCCEEDED(hr));

}

void DXCompile::CreateRootSignature() {
	HRESULT hr = S_FALSE;

	/// RootSigneture作成
	D3D12_ROOT_SIGNATURE_DESC descriptionRootSigneture{};
	descriptionRootSigneture.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;


	/// RootSignature作成; 複数設定できるので配列; 今回は1つしか設定しないので長さ1の配列;
	rootParameters_[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;		// CBVを使う
	rootParameters_[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;	// PixelShaderで使う
	rootParameters_[0].Descriptor.ShaderRegister = 0;						// レジスタ番号0とバインド

	rootParameters_[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;		// CBVを使う
	rootParameters_[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;	// VertexShaderを使う
	rootParameters_[1].Descriptor.ShaderRegister = 0;						// レジスタ番号0を使う

	rootParameters_[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;		 // DescriptorTableを使う
	rootParameters_[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;				 // PixelShaderを使う
	rootParameters_[2].DescriptorTable.pDescriptorRanges = descriptorRange_;			 // Tableの中身の配列を指定
	rootParameters_[2].DescriptorTable.NumDescriptorRanges = _countof(descriptorRange_); // Tableで利用する数


	D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};
	staticSamplers[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR; //- バイリニアフィルタ
	staticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP; //- 0~1の範囲外をリピート
	staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER; //- 比較しない
	staticSamplers[0].MaxLOD = D3D12_FLOAT32_MAX; //- ありったけのMipmapを使う
	staticSamplers[0].ShaderRegister = 0; //- レジスタ番号0を使う
	staticSamplers[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; //- PixelShaderで使う

	descriptionRootSigneture.pStaticSamplers = staticSamplers;
	descriptionRootSigneture.NumStaticSamplers = _countof(staticSamplers);
	descriptionRootSigneture.pParameters = rootParameters_;					// ルートパラメータ配列へのポインタ
	descriptionRootSigneture.NumParameters = _countof(rootParameters_);		// 配列の長さs

	/// シリアライズしてバイナリする

	hr = D3D12SerializeRootSignature(
		&descriptionRootSigneture,
		D3D_ROOT_SIGNATURE_VERSION_1,
		&signatureBlob_,
		&errorBlob_
	);

	if (FAILED(hr)) {
		Engine::ConsolePrint(reinterpret_cast<char*>(errorBlob_->GetBufferPointer()));
		assert(false);
	}

	/// バイナリを元に生成
	hr = p_directXCommon_->GetDevice()->CreateRootSignature(
		0,
		signatureBlob_->GetBufferPointer(),
		signatureBlob_->GetBufferSize(),
		IID_PPV_ARGS(&rootSignature_)
	);
	assert(SUCCEEDED(hr));

}

void DXCompile::SetingInputLayout() {

	/// InputLayout
	inputElemntDescs_[0].SemanticName = "POSITION";
	inputElemntDescs_[0].SemanticIndex = 0;
	inputElemntDescs_[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElemntDescs_[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;

	inputElemntDescs_[1].SemanticName = "TEXCOORD";
	inputElemntDescs_[1].SemanticIndex = 0;
	inputElemntDescs_[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	inputElemntDescs_[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;


	inputlayoutDesc_.pInputElementDescs = inputElemntDescs_;
	inputlayoutDesc_.NumElements = _countof(inputElemntDescs_);

}

void DXCompile::SetingBlendState() {

	/// BlendStateの設定
	/// すべての色要素を書き込む
	blendDesc_.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

}

void DXCompile::SetingRasterizerState() {

	/// RasterizerStateの設定
	/// 裏面(時計回り)を表示しない
	rasterizerDesc_.CullMode = D3D12_CULL_MODE_BACK;
	/// 三角形の中を塗りつぶす
	rasterizerDesc_.FillMode = D3D12_FILL_MODE_SOLID;

}

void DXCompile::SetingShader() {


	vertexShaderBlob_ = CompileShader(Engine::ConvertString("./Engine/Object3d.VS.hlsl"), L"vs_6_0");
	assert(vertexShaderBlob_ != nullptr);

	pixelShaderBlob_ = CompileShader(Engine::ConvertString("./Engine/Object3d.PS.hlsl"), L"ps_6_0");
	assert(pixelShaderBlob_ != nullptr);

}

void DXCompile::CreatePSO() {

	HRESULT hr = S_FALSE;

	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};
	graphicsPipelineStateDesc.pRootSignature = rootSignature_;
	graphicsPipelineStateDesc.InputLayout = inputlayoutDesc_;

	/// VertexShader
	graphicsPipelineStateDesc.VS = {
		vertexShaderBlob_->GetBufferPointer(),
		vertexShaderBlob_->GetBufferSize()
	};

	/// PixelShader
	graphicsPipelineStateDesc.PS = {
		pixelShaderBlob_->GetBufferPointer(),
		pixelShaderBlob_->GetBufferSize()
	};

	graphicsPipelineStateDesc.BlendState = blendDesc_;
	graphicsPipelineStateDesc.RasterizerState = rasterizerDesc_;

	/// 書き込むRTVの情報
	graphicsPipelineStateDesc.NumRenderTargets = 1;
	graphicsPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;

	/// 利用するトロポジ(形状)のタイプ; 三角形
	graphicsPipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	/// どのような画面に色を打ち込むかの設定(気にしなくていい)
	graphicsPipelineStateDesc.SampleDesc.Count = 1;
	graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;

	/// 実際に生成
	hr = p_directXCommon_->GetDevice()->CreateGraphicsPipelineState(
		&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&graphicsPipelineState_)
	);
	assert(SUCCEEDED(hr));



}


void DXCompile::CreateVertexResource() {

	HRESULT hr = S_FALSE;

	/// 頂点リソース用のヒープの設定
	D3D12_HEAP_PROPERTIES uploadHeapProperties{};
	uploadHeapProperties.Type = D3D12_HEAP_TYPE_UPLOAD; // UploadHeapを使う

	/// 頂点リソースのの設定
	D3D12_RESOURCE_DESC vertexResourceDesc{};
	/// バッファリソース; テクスチャの場合はまた別の設定を行う
	vertexResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	vertexResourceDesc.Width = sizeof(VertexData) * 3; // リソースのサイズ; 今回はVector4を3頂点分

	/// バッファの場合はこれらは1にする決まり
	vertexResourceDesc.Height = 1;
	vertexResourceDesc.DepthOrArraySize = 1;
	vertexResourceDesc.MipLevels = 1;
	vertexResourceDesc.SampleDesc.Count = 1;

	/// バッファの場合はこれにする決まり
	vertexResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	hr = p_directXCommon_->GetDevice()->CreateCommittedResource(
		&uploadHeapProperties,
		D3D12_HEAP_FLAG_NONE,
		&vertexResourceDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&vertexResource_)
	);
	assert(SUCCEEDED(hr));

}

ID3D12Resource* DXCompile::CreateBufferResource(size_t sizeInBytes) {
	HRESULT hr = S_FALSE;
	ID3D12Resource* resource = nullptr;

	/// 頂点リソース用のヒープの設定
	D3D12_HEAP_PROPERTIES uploadHeapProperties{};
	uploadHeapProperties.Type = D3D12_HEAP_TYPE_UPLOAD; // UploadHeapを使う

	/// 頂点リソースのの設定
	D3D12_RESOURCE_DESC vertexResourceDesc{};
	/// バッファリソース; テクスチャの場合はまた別の設定を行う
	vertexResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	vertexResourceDesc.Width = sizeInBytes; // リソースのサイズ

	/// バッファの場合はこれらは1にする決まり
	vertexResourceDesc.Height = 1;
	vertexResourceDesc.DepthOrArraySize = 1;
	vertexResourceDesc.MipLevels = 1;
	vertexResourceDesc.SampleDesc.Count = 1;

	/// バッファの場合はこれにする決まり
	vertexResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	hr = p_directXCommon_->GetDevice()->CreateCommittedResource(
		&uploadHeapProperties,
		D3D12_HEAP_FLAG_NONE,
		&vertexResourceDesc,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&resource)
	);
	assert(SUCCEEDED(hr));

	return resource;
}

void DXCompile::CreateVBV() {

	/// 頂点バッファビューを作成

	/// リソースの先頭アドレスから使う
	vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();

	/// 使用するリソースのサイズは頂点3つ分のサイズ
	vertexBufferView_.SizeInBytes = sizeof(VertexData) * 3;
	/// 1頂点あたりのサイズ
	vertexBufferView_.StrideInBytes = sizeof(VertexData);

}

void DXCompile::WriteVertexData(const Vector4& v1, const Vector4& v2, const Vector4& v3) {

	VertexData* vertexData = nullptr;
	vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));

	// 左下
	//vertexData[0] = { -0.5f,-0.5f,0.0f,1.0f };
	// 上
	//vertexData[1] = { 0.0f,0.5f,0.0f,1.0f };
	// 右下
	//vertexData[1] = { 0.5f,-0.5f,0.0f,1.0f };

	v1;
	v2;
	v3;

	vertexData[0].position = { -0.5f,-0.5f,0.0f,1.0f };
	vertexData[0].texcoord = Vec2f{ 0.0f,1.0f };

	vertexData[1].position = { 0.0f,0.5f,0.0f,1.0f };
	vertexData[1].texcoord = Vec2f{ 0.5f,0.0f };

	vertexData[2].position = { 0.5f,-0.5f,0.0f,1.0f };
	vertexData[2].texcoord = Vec2f{ 1.0f,1.0f };

}

void DXCompile::InitializeViewport() {

	/// viewport
	/*viewport_.Width = static_cast<float>(backBufferWidth_);
	viewport_.Height = static_cast<float>(backBufferHeight_);*/

	viewport_.Width = static_cast<float>(1280.0f);
	viewport_.Height = static_cast<float>(720.0f);

	viewport_.TopLeftX = 0;
	viewport_.TopLeftY = 0;
	viewport_.MinDepth = 0.0f;
	viewport_.MaxDepth = 1.0f;

	/// シザー矩形
	scissorRect_.left = 0;
	scissorRect_.right = static_cast<LONG>(1280.0f);
	scissorRect_.top = 0;
	scissorRect_.bottom = static_cast<LONG>(720.0f);

}


void DXCompile::CreateMaterialResource() {

	materialResource_ = CreateBufferResource(sizeof(Vector4));

	Vector4* materialData = nullptr;

	/// 書き込むためのアドレスを取得
	materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&materialData));

	/// 書き込み
	*materialData = Vector4(1.0f, 1.0f, 1.0f, 1.0f);

}


void DXCompile::CreateWVPResource(const Vec3f& scale, const Vec3f& rotate, const Vec3f& translate) {

	wvpResource_ = CreateBufferResource(sizeof(Matrix4x4));

	Matrix4x4* wvpData = nullptr;

	wvpResource_->Map(0, nullptr, reinterpret_cast<void**>(&wvpData));

	Matrix4x4 worldMatrix = Matrix4x4::MakeAffine(scale, rotate, translate);

	*wvpData = worldMatrix * Engine::GetCamera()->GetVpMatrix();

}

DirectX::ScratchImage DXCompile::LoadTexture(const std::string& filePath) {
	DirectX::ScratchImage image{};
	std::wstring filePathW = Engine::ConvertString(filePath);
	HRESULT hr = DirectX::LoadFromWICFile(filePathW.c_str(), DirectX::WIC_FLAGS_FORCE_SRGB, nullptr, image);
	assert(SUCCEEDED(hr));

	/// ミップマップの作成
	DirectX::ScratchImage mipImage{};
	hr = DirectX::GenerateMipMaps(image.GetImages(), image.GetImageCount(), image.GetMetadata(), DirectX::TEX_FILTER_SRGB, 0, mipImage);
	assert(SUCCEEDED(hr));

	return mipImage;
}

ID3D12Resource* DXCompile::CreateTextureResource(const DirectX::TexMetadata& metaData) {

	/// metaDataを基にResourceの設定
	D3D12_RESOURCE_DESC desc{};
	desc.Width = UINT(metaData.width);								//- Textureの幅
	desc.Height = UINT(metaData.height);							//- Textureの高さ
	desc.MipLevels = UINT16(metaData.mipLevels);					//- mipmapの数
	desc.DepthOrArraySize = UINT16(metaData.arraySize);				//- 奥行き or 配列Textureの配列数
	desc.Format = metaData.format;									//- Textureのformat
	desc.SampleDesc.Count = 1;										//- サンプリングカウント; 1固定
	desc.Dimension = D3D12_RESOURCE_DIMENSION(metaData.dimension);	//- Textureの次元数; 普段使っているのは2次元

	/// 利用するHeapの設定; 非常に特殊な運用; 02_04exで一般的なケース版がある
	D3D12_HEAP_PROPERTIES heapProperties{};
	heapProperties.Type = D3D12_HEAP_TYPE_CUSTOM;						//- 細かい設定を行う
	heapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_WRITE_BACK;	//- WriteBackポリシー
	heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_L0;			//- プロセッサの近くに配置

	/// Resourceの生成
	ID3D12Resource* resource = nullptr;
	HRESULT hr = DirectXCommon::GetInstance()->GetDevice()->CreateCommittedResource(
		&heapProperties,					//- Heapの設定
		D3D12_HEAP_FLAG_NONE,				//- Heapの特殊な設定; 特になし
		&desc,							//- Resourceの設定
		D3D12_RESOURCE_STATE_COPY_DEST,	//- データ転送される設定
		nullptr,							//- Clear最適解; 使わないのでnullptr
		IID_PPV_ARGS(&resource)			//- 作成するResourceポインタへのポインタ
	);
	assert(SUCCEEDED(hr));

	return resource;
}


/// 属性 [[nodiscard]] 戻り値を破棄しては行けないことを示す; 
[[nodiscard]]
ID3D12Resource* DXCompile::UploadTextureData(ID3D12Resource* texture, const DirectX::ScratchImage& mipImage) {

	std::vector<D3D12_SUBRESOURCE_DATA> subresources;

	/// 読み込んだデータからDirectX12用のSubresourceの配列を作成する
	DirectX::PrepareUpload(p_directXCommon_->GetDevice(), mipImage.GetImages(), mipImage.GetImageCount(), mipImage.GetMetadata(), subresources);
	/// Subresourceの数を基にコピー元となるIntermediateResourceに必要なサイズを計算する
	uint64_t intermediateSize = GetRequiredIntermediateSize(texture, 0, UINT(subresources.size()));
	/// 上記で計算した値を使って作成
	ID3D12Resource* intermediateResource = CreateBufferResource(intermediateSize);

	/// intermediateResourceにSubResourceのデータを書き込む; Textureに転送するコマンドを積む
	UpdateSubresources(p_directXCommon_->GetCommandList(), texture, intermediateResource, 0, 0, UINT(subresources.size()), subresources.data());

	//- CPUとGPUを取り持つためのResourceなので IntermediateResource(中間リソース)と呼ぶ


	/// Textureへの転送後は利用できるよう,D3D12_RESOURCE_STATE_COPY_DESCからD3D12_RESOURCE_STATE_GENERIC_READへResourceStateを変更する
	D3D12_RESOURCE_BARRIER barrier{};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = texture;
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_GENERIC_READ;
	p_directXCommon_->GetCommandList()->ResourceBarrier(1, &barrier);

	return intermediateResource;
}

void DXCompile::CreateShaderResourceView() {

	/// metaDataを基にSRVの設定
	srvDesc_.Format = metaData_.format;
	srvDesc_.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc_.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;		//- 2Dテクスチャ
	srvDesc_.Texture2D.MipLevels = UINT(metaData_.mipLevels);

	/// SRVを作成するDescriptorHeapの場所を決める
	textureSrvHandleCPU_ = srvDescriptorHeap_->GetCPUDescriptorHandleForHeapStart();
	textureSrvHandleGPU_ = srvDescriptorHeap_->GetGPUDescriptorHandleForHeapStart();

	/// 先頭はImGuiが使っているのでその次を使う
	ID3D12Device* device = DirectXCommon::GetInstance()->GetDevice();
	textureSrvHandleCPU_.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	textureSrvHandleGPU_.ptr += device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	/// SRVの生成
	device->CreateShaderResourceView(textureResource_, &srvDesc_, textureSrvHandleCPU_);

}

void DXCompile::InitializeDescriptorRange() {

	descriptorRange_[0].BaseShaderRegister = 0;							//- 0から始まる
	descriptorRange_[0].NumDescriptors = 1;								//- 数は1つ
	descriptorRange_[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;	//- SRVを使う
	descriptorRange_[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;	//- Offsetを自動計算

}

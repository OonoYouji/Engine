#include <DirectXCompile.h>

#include <d3d12.h>
#include <combaseapi.h>

#pragma comment (lib, "dxcompiler.lib")


DirectXCompile* DirectXCompile::GetInstance() {
	static DirectXCompile instance;
	return &instance;
}

void DirectXCompile::Initialize() {

	HRESULT hr = S_FALSE;

	//hr = DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&dxcUtils_));

}

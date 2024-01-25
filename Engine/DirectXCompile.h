#pragma once

#include <dxcapi.h>

class DirectXCompile final {
public:

	static DirectXCompile* GetInstance();

	void Initialize();


private:

	IDxcUtils* dxcUtils_ = nullptr;
	IDxcCompiler3* dxcCompiler_ = nullptr;


};
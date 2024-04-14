#include "TextureManager.h"

#include "DirectXCommon.h"
#include <DirectXTex.h>

#pragma comment(lib, "DirectXTex.lib")

TextureManager* TextureManager::GetInstance() {
	static TextureManager instance;
	return &instance;
}


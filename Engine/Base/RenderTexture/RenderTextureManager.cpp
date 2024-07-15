#include "RenderTextureManager.h"

#include <RenderTexture.h>


RenderTextureManager* RenderTextureManager::GetInstance() {
	static RenderTextureManager instance;
	return &instance;
}


void RenderTextureManager::Initialize() {

}


void RenderTextureManager::Finalize() {
	renderTextures_.clear();
}


void RenderTextureManager::AddRenderTexture(RenderTexture* renderTex) {
	RenderTexUni newRenTex(renderTex);
	renderTextures_.push_back(std::move(newRenTex));
}

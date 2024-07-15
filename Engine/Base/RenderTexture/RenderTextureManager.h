#pragma once


#include <list>
#include <memory>

class RenderTexture;

class RenderTextureManager final {
	RenderTextureManager() = default;
	~RenderTextureManager() = default;
public:

	static RenderTextureManager* GetInstance();

	void Initialize();

	void Finalize();

	void AddRenderTexture(RenderTexture* renderTex);

private:

	using RenderTexUni = std::unique_ptr<RenderTexture>;
	std::list<RenderTexUni> renderTextures_;

};
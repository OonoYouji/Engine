#pragma once 


class RenderTexture;

class IScene {
public:

	virtual ~IScene() = default;

	virtual void Init() = 0;
	virtual void Update() = 0;
	virtual void Draw() = 0;
	virtual void Finalize() = 0;

	void SaveFile();
	void LoadFile();

	RenderTexture* renderTex_ = nullptr;

};
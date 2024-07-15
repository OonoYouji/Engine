#pragma once 

#include <GameObject.h>

class RenderTexture;
class Sprite;

class IScene {
public: 
	enum Target{
		kBack = GameObject::Type::BackSprite,
		k3dObject = GameObject::Type::Object3d,
		kFront = GameObject::Type::FrontSprite,
		kScreen,
		Count,
	};
public:

	virtual ~IScene() = default;

	virtual void Init() = 0;
	virtual void Update() = 0;

	virtual void BackDraw() = 0;
	virtual void ObjectDraw() = 0;
	virtual void FrontDraw() = 0;

	void SaveFile();
	void LoadFile();

	void BeginRenderTarget(Target target);
	void EndRenderTarget(Target target);

	void ImGuiDraw();

protected:

	void InitializeRenderTex(IScene* thisScene);

protected:

	RenderTexture* renderTexs_[Count]{};

};
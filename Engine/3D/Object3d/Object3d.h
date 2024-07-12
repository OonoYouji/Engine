#pragma once

#include <string>
#include <list>

#include <GameObject.h>

class Model;
struct AABB;

/// <summary>
/// 3dモデルを表示するクラス
/// </summary>
class Object3d : public GameObject {
public:

	Object3d();
	~Object3d();

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="key"> : ModelManagerが持っているModelへのKey</param>
	void Initialize(const std::string& key);

	void Initialize() override;

	void Update() override;
	void Draw() override;

private:
	static int nextId_;

	int id_;
	Model* model_;
	std::string modelKey_;

	UvTransform uvTransform_;

	std::list<AABB> aabbs_;

};
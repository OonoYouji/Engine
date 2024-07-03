#pragma once

#include <string>

#include <GameObject.h>

class Model;

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

	void Update() override;
	void Draw() override;

private:
	static int nextId_;

	int id_;
	Model* model_;
	std::string modelKey_;
};
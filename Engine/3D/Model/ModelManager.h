#pragma once

#include <string>
#include <map>
#include <unordered_map>
#include <memory>

#include <TextureManager.h>

#include <Object2d.h>

class Model;
class RenderTexture;

/// <summary>
/// Modelの管理クラス
/// </summary>
class ModelManager {
	ModelManager() = default;
	~ModelManager() = default;
public:

	/// <summary>
	/// インスタンス確保
	/// </summary>
	/// <returns></returns>
	static ModelManager* GetInstance();

	void Initialize();

	void Finalize();

	/// <summary>
	/// 描画前処理
	/// </summary>
	void PreDraw();

	/// <summary>
	/// 描画後処理
	/// </summary>
	void PostDraw();

	void Update();

	Model* GetModelPtr(const std::string& key);

	void RTVClear();

	void CopySRV();

private:

	Model* Create(const std::string& fileName);
	void ImGuiDebug();
	void CreateModel(const std::string& directoryPath, const std::string& fileName, const std::string& key);

	const std::string directoryPath_ = "./Resources/Objects/";
	std::map<std::string, std::unique_ptr<Model>> models_;
	std::unordered_map<int, std::string> pairs_;

	RenderTexture* renderTex_;

	Object2d* screen_;

private:
	ModelManager& operator=(const ModelManager&) = delete;
	ModelManager(const ModelManager&) = delete;
	ModelManager(ModelManager&&) = delete;
};

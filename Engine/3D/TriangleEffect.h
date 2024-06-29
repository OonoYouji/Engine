#pragma once 

#include <wrl/client.h>
#include <d3d12.h>

#include <vector>
#include <memory>
#include <list>

#include <CBuffer.h>
#include <WorldTransform.h>
#include <Vector3.h>

#include <PerlinNoise.h>

using namespace Microsoft::WRL;

/// <summary>
/// 三角形演出用クラス
/// </summary>
class TriangleEffect {
public:

	TriangleEffect();
	~TriangleEffect();

	void Initialize();

	void Update();

	void Draw();

private:

	struct Element {

		void Initialize();

		void Update();

		void Draw(const Vec4f& color);

		std::unique_ptr<Object3dResources> resource_;
		WorldTransform worldTransform_;
		std::vector<VertexData> vertexData_;
		std::vector<uint32_t> indexData_;
		Vec3f velocity_;
		uint32_t deathTimer_ = 300;
		bool isEnd_ = false;
	};


	std::unique_ptr<PerlinNoise> perlinNoise_;

	std::list<Element> elements_;

	Vec4f color_;
	int respawnTime_;
	int frameCount_;

	float speed_;
	float rotateSpeed_ = 0.0f;
};


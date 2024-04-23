#pragma once

#include "Vector3.h"
#include "Vector4.h"

#include <d3d12.h>
#include <wrl/client.h>


using namespace Microsoft::WRL;


/// <summary>
/// 平行光源
/// </summary>
struct DirectionalLight {
	Vec4f color;     //- 光の色
	Vec3f direction; //- 向き
	float intensity; //- 輝度
};


class Light {
public:

	Light();
	~Light();

	static Light* GetInstance();

	void Init();

	void Update();

	void Finalize();


	void SetConstantBuffer(ID3D12GraphicsCommandList* commandList);

private:

	ComPtr<ID3D12Resource> resource_;

	DirectionalLight* directinalLight_;

	Vec3f nextDir_;

};
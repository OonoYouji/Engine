#include "Polygon3d.hlsli"

#include "../CBuffer/Material.hlsli"
#include "../CBuffer/DirectionalLight.hlsli"



///// -----------------------------------------
///// ↓ ConstantBuffers
///// -----------------------------------------
ConstantBuffer<Material> gMaterial : register(b0);
ConstantBuffer<DirectionalLight> gDirectionalLight : register(b1);



///// -----------------------------------------
///// ↓ main
///// -----------------------------------------
PSOutput main(VSOutput input) {
	PSOutput output;

	if (gMaterial.enableLighting) {
		float NdotL = dot(normalize(input.normal), -gDirectionalLight.direction);
		float cos = pow(NdotL * 0.5f + 0.5f, 2.0f);
		output.color = gMaterial.color * gDirectionalLight.color * cos * gDirectionalLight.intensity;
	} else {
		output = gMaterial.color;
	}

	return output;
}

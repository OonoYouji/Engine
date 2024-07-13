#include "Object3d.hlsli"

#include "../CBuffer/Material.hlsli"
#include "../CBuffer/DirectionalLight.hlsli"



///// -----------------------------------------
///// ↓ ConstantBuffers
///// -----------------------------------------
Texture2D<float4> gTexture : register(t0);
SamplerState gSampler : register(s0);
ConstantBuffer<Material> gMaterial : register(b0);
ConstantBuffer<DirectionalLight> gDirectionalLight : register(b1);



///// -----------------------------------------
///// ↓ main
///// -----------------------------------------
PSOutput main(VSOutput input) {
	PSOutput output;

	float3 texcoord = mul(float3(input.texcoord, 1.0f), (float3x3)gMaterial.uvTransform);
	float4 textureColor = gTexture.Sample(gSampler, texcoord.xy);

	///- Lightingする場合
	if (gMaterial.enableLighting != 0) {
		float NdotL = dot(normalize(input.normal), -gDirectionalLight.direction);
		float cos = pow(NdotL * 0.5f + 0.5f, 2.0f);
		output.color = gMaterial.color * textureColor * gDirectionalLight.color * cos * gDirectionalLight.intensity;
	} else {
		///- Lightingしない場合
		output.color = gMaterial.color * textureColor;
	}

	output.color.a = gMaterial.color.a * textureColor.a;
	if (output.color.a == 0.0f) {
		discard;
	}

	return output;
}




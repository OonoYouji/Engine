#include "Terrain.hlsli"

struct PSOutput {
	float4 color : SV_TARGET0;
};


/// ------------------------------------
/// ↓ 定数バッファ : TextureSampler
/// ------------------------------------
Texture2D<float4> gTexture : register(t0);
SamplerState gSampler : register(s0);
ConstantBuffer<Material> gMaterial : register(b0);
ConstantBuffer<DirectionalLight> gDirectionalLight : register(b1);


/// ------------------------------------
/// ↓ main
/// ------------------------------------
PSOutput main(VSOutput input) {
	PSOutput output;
	float4 textureColor = gTexture.Sample(gSampler, input.texcoord);

	///- マウスの座標に円を表示する
	if (gMousePoint.isActive) {

		float len = length(input.worldPos - gMousePoint.worldPos);
		float3 mousePos = gMousePoint.worldPos + (gMousePoint.rayDir * len);
		len = length(input.worldPos - mousePos);

		if (len < gMousePoint.brushSize) {
			textureColor = gMaterial.color * clamp((len / gMousePoint.brushSize), 0.2f, 1.0f);
		}
	}
	

	///- Lightingする場合
	if (gMaterial.enableLighting != 0) {
		float NdotL = dot(normalize(input.normal), -gDirectionalLight.direction);
		float cos = pow(NdotL * 0.5f + 0.5f, 2.0f);
		output.color = gMaterial.color * textureColor * gDirectionalLight.color * cos * gDirectionalLight.intensity;
	} else {
		///- Lightingしない場合
		output.color = gMaterial.color * textureColor;
	}
	return output;
}
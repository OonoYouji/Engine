#include "Object3d.hlsli"

struct Material {
	float4 color;
	int enableLighting;
};

struct PixelShaderOutput {
	float4 color : SV_TARGET0;
};




Texture2D<float4> gTexture : register(t0);
SamplerState gSampler : register(s0);

///- 定数バッファ
ConstantBuffer<Material> gMaterial : register(b0);
ConstantBuffer<DirectionalLight> gDirectionalLight : register(b1);


PixelShaderOutput main(VertexShaderOutput input) {
	PixelShaderOutput output;
	float4 textureColor = gTexture.Sample(gSampler, input.texcoord);


	float len = length(input.worldPos - gMousePoint.worldPos);
	float3 mousePos = gMousePoint.worldPos + (gMousePoint.rayDir * len);
	len = length(input.worldPos - mousePos);
	if (len < gMousePoint.brushSize) {
		textureColor.rgb = float3(0.5f, 0.5f, 0.5f) * clamp(1 - (len / gMousePoint.brushSize), 0.0f, 1.0f);
	}
	
	//float3 diff = normalize(input.worldPos - gMousePoint.worldPos);
	//float d = dot(diff, gMousePoint.layDir);
	//if (d > 0.5f) {
	//	output.color = float4(1.0f, 0.0f, 0.0f, 1.0f);
	//	return output;
	//}
	
	///- Lightingする場合
	if (gMaterial.enableLighting != 0) {
		float NdotL = dot(normalize(input.normal), -gDirectionalLight.direction);
		float cos = pow(NdotL * 0.5f + 0.5f, 2.0f);
		output.color = gMaterial.color * textureColor * gDirectionalLight.color * cos * gDirectionalLight.intensity;
	} else {
		///- Lightingしない場合

		output.color = gMaterial.color * textureColor;
	}


	///- mouseの座標を黒色にする
	//if ((input.position.x - gMousePoint.position.x) > -10
	//	&& (input.position.x - gMousePoint.position.x) < 10) {

	//	if ((input.position.y - gMousePoint.position.y) > -10
	//		&& (input.position.y - gMousePoint.position.y) < 10) {

	//		output.color = float4(1.0f, 0.0f, 0.0f, 1.0f);
	//	}
	//}

	return output;
}
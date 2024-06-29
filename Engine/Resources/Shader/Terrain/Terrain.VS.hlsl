#include "Terrain.hlsli"

#include "../CBuffer/TransformationMatrix.hlsli"

struct VSInput {
	float4 position : POSITION0;
	float2 texcoord : TEXCOORD0;
	float3 normal : NORMAL0;
};


/// ------------------------------------
/// ↓ 定数バッファ : TextureSampler
/// ------------------------------------
Texture2D<float4> gTexture : register(t0);
SamplerState gSampler : register(s1);
ConstantBuffer<TransformationMatrix> gTransformationMatrix : register(b0);


/// ------------------------------------
/// ↓ main
/// ------------------------------------
VSOutput main(VSInput input) {
	VSOutput output;
	
	///- 輝度から頂点のY座標を加算
	float4 clr = gTexture.SampleLevel(gSampler, input.texcoord, 0.0f);
	uint2 dimension;
	gOutputTexture.GetDimensions(dimension.x, dimension.y);

	float4 worldPos = mul(input.position, gTransformationMatrix.World);

	if (gMousePoint.isActive) {
		float len = length(worldPos.xyz - gMousePoint.worldPos);
		float3 mousePos = gMousePoint.worldPos + (gMousePoint.rayDir * len);
		len = length(worldPos.xyz - mousePos);
		if (len < gMousePoint.brushSize) {

			float power = ((1.0f - len / gMousePoint.brushSize) * 0.25f) * gMousePoint.power;
			if (gMousePoint.isUp == true) {
				gOutputTexture[input.texcoord * dimension].rgb += float3(power, power, power);
			}

			if (gMousePoint.isDown == true) {
				gOutputTexture[input.texcoord * dimension].rgb -= float3(power, power, power);
			}
		}
	}

	clr *= gOutputTexture[input.texcoord * dimension];

	// 輝度の計算
	float luminance = 0.2126f * clr.r + 0.7152f * clr.g + 0.0722f * clr.b;
    
	float4 pos = input.position + float4(0.0f, luminance, 0.0f, 0.0f);
	output.position = mul(pos, gTransformationMatrix.WVP);
	output.texcoord = input.texcoord;
	output.normal = normalize(mul(input.normal, (float3x3) gTransformationMatrix.World));
	output.worldPos = mul(pos, gTransformationMatrix.World).xyz;
	output.color = float4(luminance, luminance, luminance, luminance);

	return output;
}
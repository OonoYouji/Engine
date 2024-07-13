#include "Sprite.hlsli"
#include "../CBuffer/Material.hlsli"

Texture2D<float4> gTexture : register(t0);
SamplerState gSampler : register(s0);
StructuredBuffer<Material> gMaterial : register(t1);


PSOutput main(VSOutput input) {
	PSOutput output;

	float3x3 uvTransform = (float3x3)gMaterial[input.instanceId].uvTransform;

	float3 texcoord = mul(float3(input.texcoord, 1.0f), uvTransform);
	float4 textureColor = gTexture.Sample(gSampler, texcoord.xy);

	output.color = gMaterial[input.instanceId].color * textureColor;
	if (output.color.a == 0.0f) {
		discard;
	}

	return output;
}
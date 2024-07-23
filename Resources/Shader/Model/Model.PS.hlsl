#include "Model.hlsli"

#include "../CBuffer/Material.hlsli"
#include "../CBuffer/DirectionalLight.hlsli"



///// -----------------------------------------
///// ↓ ConstantBuffers
///// -----------------------------------------
Texture2D<float4> gTexture : register(t0);
SamplerState gSampler : register(s0);
StructuredBuffer<Material> gMaterial : register(t1);
ConstantBuffer<DirectionalLight> gDirectionalLight : register(b1);



///// -----------------------------------------
///// ↓ main
///// -----------------------------------------
PSOutput main(VSOutput input) {
	PSOutput output;

	float3 texcoord = mul(float3(input.texcoord, 1.0f), (float3x3) gMaterial[input.instanceId].uvTransform);
	float4 textureColor = gTexture.Sample(gSampler, texcoord.xy);

	if (gMaterial[input.instanceId].enableLighting == 0) {
		///- Lightingしない場合
		output.color = gMaterial[input.instanceId].color * textureColor;

	} else if (gMaterial[input.instanceId].enableLighting == 1) {
		///- Lambert
		float cos = saturate(dot(normalize(input.normal), -gDirectionalLight.direction));
		output.color = gMaterial[input.instanceId].color * textureColor * gDirectionalLight.color * cos * gDirectionalLight.intensity;

	} else if (gMaterial[input.instanceId].enableLighting == 2) {
		///- Half Lambert
		float NdotL = dot(normalize(input.normal), -gDirectionalLight.direction);
		float cos = pow(NdotL * 0.5f + 0.5f, 2.0f);
		output.color = gMaterial[input.instanceId].color * textureColor * gDirectionalLight.color * cos * gDirectionalLight.intensity;

	}
	
	output.color.a = gMaterial[input.instanceId].color.a * textureColor.a;
	if (output.color.a == 0.0f) {
		discard;
	}

	return output;
}




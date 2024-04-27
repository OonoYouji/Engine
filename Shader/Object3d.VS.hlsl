#include "Object3d.hlsli"

struct TransformationMatrix {
	float4x4 WVP;
	float4x4 World;
};

struct VertexShaderInput {
	float4 position : POSITION0;
	float2 texcoord : TEXCOORD0;
	float3 normal : NORMAL0;
};



Texture2D<float4> gTexture : register(t0);
SamplerState gSampler : register(s1);
ConstantBuffer<TransformationMatrix> gTransformationMatrix : register(b0);


VertexShaderOutput main(VertexShaderInput input) {
	VertexShaderOutput output;
	float4 clr = gTexture.SampleLevel(gSampler, input.texcoord, 0);
	float4 pos = input.position + float4(0.0f, clr.r, 0.0f, 0.0f);
	output.position = mul(pos, gTransformationMatrix.WVP);
	output.texcoord = input.texcoord;
	output.normal = normalize(mul(input.normal, (float3x3) gTransformationMatrix.World));
	return output;
}
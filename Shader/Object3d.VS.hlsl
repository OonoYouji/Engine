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
RWTexture2D<float4> gOutputTexture : register(u3);

ConstantBuffer<TransformationMatrix> gTransformationMatrix : register(b0);

VertexShaderOutput main(VertexShaderInput input) {
	VertexShaderOutput output;


	///- 輝度から頂点のY座標を加算
	float4 clr = gTexture.SampleLevel(gSampler, input.texcoord, 0);
	float4 worldPos = mul(input.position, gTransformationMatrix.World);

	if (gMousePoint.isActive) {
		float len = length(worldPos.xyz - gMousePoint.worldPos);
		float3 mousePos = gMousePoint.worldPos + (gMousePoint.rayDir * len);
		len = length(worldPos.xyz - mousePos);
		if (len < gMousePoint.brushSize) {
			if (gMousePoint.isUp == true) {
				clr.r += 1.0f;
			}

			if (gMousePoint.isDown == true) {
				clr.r -= 1.0f;
			}
		}
	}

	float4 pos = input.position + float4(0.0f, clr.r, 0.0f, 0.0f);
	//gOutputTexture[output.texcoord] = clr;
	output.position = mul(pos, gTransformationMatrix.WVP);
	output.texcoord = input.texcoord;
	output.normal = normalize(mul(input.normal, (float3x3) gTransformationMatrix.World));
	output.worldPos = mul(pos, gTransformationMatrix.World).xyz;
	
	return output;
}
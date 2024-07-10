#include "Sprite.hlsli"

StructuredBuffer<WorldMatrix> gWorldMatrix : register(t0);
ConstantBuffer<ViewProjectionMatrix> gViewProjectionMatrix : register(b0);

VSOutput main(VSInput input, uint instanceId : SV_InstanceID) {
	VSOutput output;

	float4x4 matWVP = mul(gWorldMatrix[instanceId].matWorld, gViewProjectionMatrix.matViewProjection);

	output.position = mul(input.position, matWVP);
	output.texcoord = input.texcoord;
	output.instanceId = instanceId;

	return output;
}
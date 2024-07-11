#include "Line.hlsli"

struct ViewProjection {
	float4x4 matViewProjection;
};

ConstantBuffer<ViewProjection> gViewProjection : register(b0);

VSOutput main(VSInput input) {
	VSOutput output;
	
	output.position = mul(input.position, gViewProjection.matViewProjection);
	output.color = input.color;

	return output;
}
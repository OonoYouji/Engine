#include "Object3d.hlsli"

#include "../CBuffer/TransformationMatrix.hlsli"


ConstantBuffer<TransformationMatrix> gTransformationMatrix : register(b0);

VSOutput main(VSInput input) {
	VSOutput output;

	output.position = mul(input.position, gTransformationMatrix.WVP);
	output.texcoord = input.texcoord;
	output.normal = normalize(mul(input.normal, (float3x3) gTransformationMatrix.World));
	
	return output;
}
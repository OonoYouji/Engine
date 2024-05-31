#include <SolidColor.hlsli>

#include "../CBuffer/TransformationMatrix.hlsli"



///// -----------------------------------------
///// ↓ ConstantBuffers
///// -----------------------------------------
ConstantBuffer<TransformationMatrix> gTransformationMatrix : register(b0);



///// -----------------------------------------
///// ↓ main
///// -----------------------------------------
VSOutput main(VSInput input) {
	VSOutput output;

	output.position = mul(input.position, gTransformationMatrix.WVP);
	output.normal = normalize(mul(input.normal, (float3x3) gTransformationMatrix.World));

	return output;
}

#include "Model.hlsli"

#include "../CBuffer/TransformationMatrix.hlsli"


StructuredBuffer<TransformationMatrix> gTransformationMatrix : register(t0);

VSOutput main(VSInput input, uint instanceId : SV_InstanceID) {
	VSOutput output;

	output.position = mul(input.position, gTransformationMatrix[instanceId].WVP);
	output.texcoord = input.texcoord;
	output.normal = normalize(mul(input.normal, (float3x3) gTransformationMatrix[instanceId].World));
	output.instanceId = instanceId;
	
	return output;
}

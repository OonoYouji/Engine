#include "Object3d.hlsli"

struct Material {
	float4 color;
	int enableLighting;
};

struct PixelShaderOutput {
	float4 color : SV_TARGET0;
};




Texture2D<float4> gTexture : register(t0);
SamplerState gSampler : register(s0);

///- 定数バッファ
ConstantBuffer<Material> gMaterial : register(b0);
ConstantBuffer<DirectionalLight> gDirectionalLight : register(b1);




float3 LinearToSRGB(float3 color) {

	float3 sq1 = sqrt(color);
	float3 sq2 = sqrt(sq1);
	float3 sq3 = sqrt(sq2);
	float3 result = 0.662002687 * sq1 + 0.684122060 * sq2 - 0.323583601 * sq3 - 0.0225411470 * color;
	return result;
}

PixelShaderOutput main(VertexShaderOutput input) {
	PixelShaderOutput output;
	float4 textureColor = gTexture.Sample(gSampler, input.texcoord);

	///- Lightingする場合
	if (gMaterial.enableLighting != 0) {
		float NdotL = dot(normalize(input.normal), -gDirectionalLight.direction);
		float cos = pow(NdotL * 0.5f + 0.5f, 2.0f);
		output.color = gMaterial.color * textureColor * gDirectionalLight.color * cos * gDirectionalLight.intensity;
	} else {
		///- Lightingしない場合
		output.color = gMaterial.color * textureColor;
	}

	//output.color.rgb = LinearToSRGB(output.color.rgb);

	return output;
}




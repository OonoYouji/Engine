#include "Object3d.hlsli"

struct Material
{
	float4 color;
	int enableLighting;
};

struct PixelShaderOutput
{
	float4 color : SV_TARGET0;
};



Texture2D<float4> gTexture : register(t0);
SamplerState gSampler : register(s0);

ConstantBuffer<Material> gMaterial : register(b0);
ConstantBuffer<DirectionalLight> gDirectionalLight : register(b1);

PixelShaderOutput main(VertexShaderOutput input)
{
	PixelShaderOutput output;
	float4 textureColor = gTexture.Sample(gSampler, input.texcoord);

	///- Lightingする場合
	if (gMaterial.enableLighting != 0)
	{
		float cos = saturate(dot(normalize(input.normal), -gDirectionalLight.direction));
		output.color = gMaterial.color * textureColor * gDirectionalLight.color * cos * gDirectionalLight.intensity;
	}
	else ///- Lightingしない場合
	{
		output.color = gMaterial.color * textureColor;
	}

	return output;
}

struct Texcoord {
	float2 uv;
};

ConstantBuffer<Texcoord> gTexcoord : register(b0);
Texture2D<float4> gTexture : register(t0);
SamplerState gSampler : register(s0);
RWStructuredBuffer<float4> outputBuffer : register(u0);
RWTexture2D<float4> gUavTexture : register(u1);

[numthreads(1, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID) {

	///- uv座標系
	float2 uv = gTexcoord.uv;
	
	///- Textureのサンプリング
	float4 textureColor = gTexture.Sample(gSampler, uv);

	///- uavTexture
	uint2 dimension;
	gUavTexture.GetDimensions(dimension.x, dimension.y);
	
	float4 uavTexColor = gUavTexture[uv * dimension];

	///- 結果をバッファに積む
	outputBuffer[DTid.x] = textureColor * uavTexColor;

}
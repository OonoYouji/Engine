

ConstantBuffer<float2> gTexcoord : register(b0);
Texture2D<float4> gTexture : register(t0);
SamplerState gSampler : register(s0);
RWStructuredBuffer<float4> outputBuffer : register(u0);

[numthreads(1, 1, 1)]
void csMain(uint3 DTid : SV_DispatchThreadID) {

	///- uv座標系
	float2 uv = gTexcoord;
	
	///- Textureのサンプリング
	float4 textureColor = gTexture.Sample(gSampler, uv);

	///- 結果をバッファに積む
	outputBuffer[DTid.x] = textureColor;

}
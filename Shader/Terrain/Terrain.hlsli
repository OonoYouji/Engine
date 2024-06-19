
struct VSOutput {
	float4 position : SV_POSITION;
	float2 texcoord : TEXCOORD0;
	float3 normal : NORMAL0;
	float3 worldPos : WORLDPOS0;
};

struct MousePoint {
	float2 position;
	float3 worldPos;
	float3 rayDir;
	float brushSize;
	int isUp;
	int isDown;
	int isActive;
	float power;
};

struct DirectionalLight {
	float4 color;
	float3 direction;
	float intensity;
};




/// ------------------------------------
/// ConstantBuffer
/// ------------------------------------
ConstantBuffer<MousePoint> gMousePoint : register(b3);
RWTexture2D<float4> gOutputTexture : register(u3);
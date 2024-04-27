struct VertexShaderOutput {
	float4 position : SV_POSITION;
	float2 texcoord : TEXCOORD0;
	float3 normal : NORMAL0;
	float3 worldPos : WORLDPOS0;
};

struct DirectionalLight {
	float4 color;
	float3 direction;
	float intensity;
};



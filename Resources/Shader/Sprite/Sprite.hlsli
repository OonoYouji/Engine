struct VSInput {
	float4 position : POSITION0;
	float2 texcoord : TEXCOORD0;
};


struct VSOutput {
	float4 position : SV_POSITION;
	float2 texcoord : TEXCOORD0;
	uint instanceId : SV_InstanceID;
};


struct PSOutput {
	float4 color : SV_TARGET0;
};

struct WorldMatrix {
	float4x4 matWorld;
};

struct ViewProjectionMatrix {
	float4x4 matViewProjection;
};


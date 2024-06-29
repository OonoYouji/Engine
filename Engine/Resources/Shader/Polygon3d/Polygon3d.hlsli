struct VSInput {
	float4 position : POSITION0;
	float3 normal : NORMAL0;
};


///// -----------------------------------------
///// PSInput = VSOutput
///// -----------------------------------------
struct VSOutput {
	float4 position : SV_POSITION;
	float3 normal : NORMAL;
};


struct PSOutput {
	float4 color : SV_TARGET0;
};



Texture2D<float4> gBackTex : register(t0);
Texture2D<float4> g3dObjTex : register(t1);
Texture2D<float4> gFrontTex : register(t2);

RWTexture2D<float4> gOutputTexture : register(u0);

[numthreads(16, 16, 1)]
void main(uint3 DTid : SV_DispatchThreadID) {
	
	float4 back = gBackTex[DTid.xy];
	float4 obj = g3dObjTex[DTid.xy];
	float4 front = gFrontTex[DTid.xy];

	//float4 outputColor = lerp(back, obj, obj.a);
	float4 outputColor = obj * obj.a + back * (1 - obj.a);
	outputColor = front * front.a + outputColor * (1 - front.a);

	
	gOutputTexture[DTid.xy] = outputColor;
}

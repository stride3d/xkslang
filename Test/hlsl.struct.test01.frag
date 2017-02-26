struct PS_STREAMS 
{
	float2 Position_id0;
	float4 ColorTarget_id1;
};
struct VS_STREAMS 
{
	float2 VertexPosition_id2;
	float2 Position_id0;
};
struct VS_OUTPUT 
{
	float2 Position_id0 : POSITION;
};
struct VS_INPUT 
{
	float2 VertexPosition_id2 : V_POSITION;
};

struct S1 {
    bool b;
    float4 f;
};

float process(S1 s1)
{
	return s1.f;
}

float4 PixelShaderFunction() : COLOR0
{
	return 1;
}

VS_OUTPUT VSMain(VS_INPUT __input__)
{
	VS_STREAMS _streams = (VS_STREAMS)0;
	_streams.VertexPosition_id2 = __input__.VertexPosition_id2;
	_streams.Position_id0 = float2(0.0f, 1.0f) + _streams.VertexPosition_id2;
	
	VS_OUTPUT __output__ = (VS_OUTPUT)0;
	__output__.Position_id0 = _streams.Position_id0;
	return __output__;
}
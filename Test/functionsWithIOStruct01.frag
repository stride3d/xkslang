
//struct VS_OUT 
//{
//    float2 Position;
//};

struct PS_STREAMS 
{
    float2 Position;
    float4 Color;
};

float4 Compute_id0(PS_STREAMS _streams)
{
    return float4(_streams.Position.xy, 0, 0);
}

void PSMain()
{
	PS_STREAMS _streams = (PS_STREAMS)0;
    _streams.Color = Compute_id0(_streams);
}



struct PS_STREAMS 
{
    float2 Position_id0;
    float4 ColorTarget_id1;
};
struct VS_STREAMS 
{
    float2 Position_id0;
};
struct VS_INPUT 
{
};
float4 Compute_id0(inout PS_STREAMS _streams)
{
    _streams.Position_id0 = float2(1, 1);
    return float4(_streams.Position_id0.xy, 0, 0);
}
void PSMain()
{
    PS_STREAMS _streams = (PS_STREAMS)0;
    _streams.ColorTarget_id1 = Compute_id0(_streams) + float4(_streams.Position_id0.xy, 0, 1);
}
void VSMain()
{
    VS_STREAMS _streams = (VS_STREAMS)0;
    _streams.Position_id0 = float2(0.0f, 1.0f);
}

// D:\Prgms\glslang\source\Test\hlsl.test.frag --keep-uncalled -D -V -o D:\Prgms\glslang\source\Test\hlsl.test.frag.latest.spv
//D:\Prgms\glslang\source\Test\hlsl.test.frag --keep-uncalled -D -V -H -o D:\Prgms\glslang\source\Test\hlsl.test.fragB.latest.spv.hr

/*struct PSInput
{
    float myfloat;
    int something;
};

//[maxvertexcount(4)]
void main(triangle in uint VertexID[3],
          inout LineStream<PSInput> OutputStream)
{
    PSInput Vert;

    Vert.myfloat    = 0;
    Vert.something  = VertexID[0];

    OutputStream.Append(Vert);
    OutputStream.Append(Vert);
    OutputStream.RestartStrip();
}
*/

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
void PSMain(VS_OUTPUT __input__)
{
    PS_STREAMS _streams = (PS_STREAMS)0;
    _streams.Position_id0 = __input__.Position_id0;
    _streams.ColorTarget_id1 = float4(_streams.Position_id0.xy, 0, 1);
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
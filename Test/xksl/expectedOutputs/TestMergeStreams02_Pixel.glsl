#version 450

struct PS_STREAMS
{
    vec4 outStreamA_id0;
};

layout(location = 0) out vec4 PS_OUT_outStreamA;

void ShaderA_computeA(inout PS_STREAMS _streams)
{
    _streams.outStreamA_id0 = vec4(2.0);
}

void ShaderB_computeB(inout PS_STREAMS _streams)
{
    _streams.outStreamA_id0 = vec4(2.0);
}

void main()
{
    PS_STREAMS _streams = PS_STREAMS(vec4(0.0));
    ShaderA_computeA(_streams);
    ShaderB_computeB(_streams);
    PS_OUT_outStreamA = _streams.outStreamA_id0;
}


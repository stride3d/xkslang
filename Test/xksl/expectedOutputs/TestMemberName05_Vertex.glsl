#version 410
#ifdef GL_ARB_shading_language_420pack
#extension GL_ARB_shading_language_420pack : require
#endif

struct VS_STREAMS
{
    vec4 aStream1_id0;
    vec4 outputStream_id1;
};

layout(std140) uniform Globals
{
    vec4 ShaderMain_aCol;
} Globals_var;

out vec4 VS_OUT_aStream1;
out vec4 VS_OUT_outputStream;

void ShaderMain_aStream1__Compute(inout VS_STREAMS _streams)
{
    _streams.aStream1_id0 = Globals_var.ShaderMain_aCol;
}

void main()
{
    VS_STREAMS _streams = VS_STREAMS(vec4(0.0), vec4(0.0));
    ShaderMain_aStream1__Compute(_streams);
    _streams.outputStream_id1 = _streams.aStream1_id0;
    VS_OUT_aStream1 = _streams.aStream1_id0;
    VS_OUT_outputStream = _streams.outputStream_id1;
    gl_Position.z = 2.0 * gl_Position.z - gl_Position.w;
    gl_Position.y = -gl_Position.y;
}


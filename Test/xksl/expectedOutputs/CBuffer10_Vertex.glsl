#version 410
#ifdef GL_ARB_shading_language_420pack
#extension GL_ARB_shading_language_420pack : require
#endif

struct VS_STREAMS
{
    float aStream1_id0;
    float aStream2_id1;
};

layout(std140) uniform PerLighting
{
    float ShaderMain_var1;
    float ShaderMain_var2;
} PerLighting_var;

in float VS_IN_STREAM1;
out float VS_OUT_aStream2;

void main()
{
    VS_STREAMS _streams = VS_STREAMS(0.0, 0.0);
    _streams.aStream1_id0 = VS_IN_STREAM1;
    _streams.aStream2_id1 = _streams.aStream1_id0 + PerLighting_var.ShaderMain_var1;
    VS_OUT_aStream2 = _streams.aStream2_id1;
    gl_Position.z = 2.0 * gl_Position.z - gl_Position.w;
    gl_Position.y = -gl_Position.y;
}


#version 410
#ifdef GL_ARB_shading_language_420pack
#extension GL_ARB_shading_language_420pack : require
#endif

struct VS_STREAMS
{
    float sbase1_id0;
};

layout(std140) uniform Globals
{
    int ShaderBase_Var0;
    int ShaderBase_Var1;
} Globals_var;

in float VS_IN_SBASE1;

void main()
{
    VS_STREAMS _streams = VS_STREAMS(0.0);
    _streams.sbase1_id0 = VS_IN_SBASE1;
    int i = int(((_streams.sbase1_id0 + float(Globals_var.ShaderBase_Var0)) + float(Globals_var.ShaderBase_Var1)) + 2.0);
    gl_Position.z = 2.0 * gl_Position.z - gl_Position.w;
    gl_Position.y = -gl_Position.y;
}


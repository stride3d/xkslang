#version 410
#ifdef GL_ARB_shading_language_420pack
#extension GL_ARB_shading_language_420pack : require
#endif

struct ShaderMain_StructType
{
    vec4 aF4;
    vec4 ColorArray[2];
    uint aBool;
    vec4 ColorArrayB[2];
    mat2x3 aMat23[2];
    mat2x3 aMat23_rm[2];
};

struct PS_STREAMS
{
    vec4 ColorTarget_id0;
};

layout(std140) uniform Globals
{
    ShaderMain_StructType ShaderMain_var1;
    vec4 ShaderMain_aCol;
} Globals_var;

out vec4 PS_OUT_ColorTarget;

void main()
{
    PS_STREAMS _streams = PS_STREAMS(vec4(0.0));
    _streams.ColorTarget_id0 = Globals_var.ShaderMain_aCol + Globals_var.ShaderMain_var1.aF4;
    PS_OUT_ColorTarget = _streams.ColorTarget_id0;
}


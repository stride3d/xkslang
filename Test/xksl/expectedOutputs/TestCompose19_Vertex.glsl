#version 410
#ifdef GL_ARB_shading_language_420pack
#extension GL_ARB_shading_language_420pack : require
#endif

struct VS_STREAMS
{
    float totoA_id0;
};

layout(std140) uniform Globals
{
    float o0S5C0_ShaderComp_varC;
    float o1S5C1_ShaderComp_varC;
    float o2S55C0_ShaderComp_varC;
    float o3S55C1_ShaderComp_varC;
} Globals_var;

out float VS_OUT_totoA;

float ShaderA_Compute()
{
    return 5.0;
}

float o0S5C0_ShaderComp_Compute()
{
    return Globals_var.o0S5C0_ShaderComp_varC;
}

float o1S5C1_ShaderComp_Compute()
{
    return Globals_var.o1S5C1_ShaderComp_varC;
}

void main()
{
    VS_STREAMS _streams = VS_STREAMS(0.0);
    float f = ShaderA_Compute();
    _streams.totoA_id0 = (f + o0S5C0_ShaderComp_Compute()) + o1S5C1_ShaderComp_Compute();
    VS_OUT_totoA = _streams.totoA_id0;
    gl_Position.z = 2.0 * gl_Position.z - gl_Position.w;
    gl_Position.y = -gl_Position.y;
}


#version 410
#ifdef GL_ARB_shading_language_420pack
#extension GL_ARB_shading_language_420pack : require
#endif

layout(std140) uniform Globals
{
    float o0S13C0_ShaderCompose_varCompose;
    float o0S13C0_ShaderComposeX_varComposeX;
} Globals_var;

float o0S13C0_ShaderComposeX_Compute()
{
    return Globals_var.o0S13C0_ShaderComposeX_varComposeX;
}

float o0S13C0_ShaderCompose_ComputeComp()
{
    return Globals_var.o0S13C0_ShaderCompose_varCompose + o0S13C0_ShaderComposeX_Compute();
}

void main()
{
    float f = o0S13C0_ShaderComposeX_Compute();
    f += o0S13C0_ShaderCompose_ComputeComp();
    gl_Position.z = 2.0 * gl_Position.z - gl_Position.w;
    gl_Position.y = -gl_Position.y;
}


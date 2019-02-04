#version 410
#ifdef GL_ARB_shading_language_420pack
#extension GL_ARB_shading_language_420pack : require
#endif

layout(std140) uniform Globals
{
    float ShaderMainX_varMainX;
    float o0S13C0_ShaderCompose_varCompose;
} Globals_var;

float ShaderMainX_Compute()
{
    return Globals_var.ShaderMainX_varMainX;
}

float o0S13C0_ShaderCompose_ComputeComp()
{
    return Globals_var.o0S13C0_ShaderCompose_varCompose + ShaderMainX_Compute();
}

void main()
{
    float f = ShaderMainX_Compute();
    f += o0S13C0_ShaderCompose_ComputeComp();
    gl_Position.z = 2.0 * gl_Position.z - gl_Position.w;
    gl_Position.y = -gl_Position.y;
}


#version 410
#ifdef GL_ARB_shading_language_420pack
#extension GL_ARB_shading_language_420pack : require
#endif

layout(std140) uniform Globals
{
    float ShaderMain_varMain;
    float o0S5C0_ShaderCompose_varCompose;
} Globals_var;

float ShaderMain_Compute()
{
    return Globals_var.ShaderMain_varMain;
}

float o0S5C0_ShaderCompose_ComputeComp()
{
    return Globals_var.o0S5C0_ShaderCompose_varCompose + ShaderMain_Compute();
}

void main()
{
    float f = ShaderMain_Compute();
    f += o0S5C0_ShaderCompose_ComputeComp();
    gl_Position.z = 2.0 * gl_Position.z - gl_Position.w;
    gl_Position.y = -gl_Position.y;
}


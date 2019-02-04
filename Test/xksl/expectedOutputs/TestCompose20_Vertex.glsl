#version 410
#ifdef GL_ARB_shading_language_420pack
#extension GL_ARB_shading_language_420pack : require
#endif

layout(std140) uniform Globals
{
    float o0S29C0_ShaderComp_varC;
    float o1S16C0_ShaderComp_varC;
} Globals_var;

float o0S29C0_ShaderComp_Compute()
{
    return Globals_var.o0S29C0_ShaderComp_varC;
}

float ShaderA_Compute()
{
    return 1.0 + o0S29C0_ShaderComp_Compute();
}

float o1S16C0_ShaderComp_Compute()
{
    return Globals_var.o1S16C0_ShaderComp_varC;
}

float ShaderB_Compute()
{
    return 2.0 + o1S16C0_ShaderComp_Compute();
}

float ShaderMain_Compute()
{
    return ShaderA_Compute() + ShaderB_Compute();
}

void main()
{
    float f = ShaderMain_Compute();
    gl_Position.z = 2.0 * gl_Position.z - gl_Position.w;
    gl_Position.y = -gl_Position.y;
}


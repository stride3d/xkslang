#version 410
#ifdef GL_ARB_shading_language_420pack
#extension GL_ARB_shading_language_420pack : require
#endif

layout(std140) uniform cbComposition
{
    float o0S2C0_ShaderComposition_var09;
} cbComposition_var;

float o0S2C0_ShaderComposition_Compute()
{
    return cbComposition_var.o0S2C0_ShaderComposition_var09;
}

void main()
{
    float f = 0.0;
    f += o0S2C0_ShaderComposition_Compute();
    gl_Position.z = 2.0 * gl_Position.z - gl_Position.w;
    gl_Position.y = -gl_Position.y;
}


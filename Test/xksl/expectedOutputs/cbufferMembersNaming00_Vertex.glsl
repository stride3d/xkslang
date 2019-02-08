#version 410
#ifdef GL_ARB_shading_language_420pack
#extension GL_ARB_shading_language_420pack : require
#endif

layout(std140) uniform cbUnstageCompositionLink
{
    float o0S2C0_ShaderCompositionWithLink_var11;
} cbUnstageCompositionLink_var;

layout(std140) uniform cbStageCompositionLink
{
    float o0S2C0_ShaderCompositionWithLink_var12;
} cbStageCompositionLink_var;

float o0S2C0_ShaderCompositionWithLink_Compute()
{
    return cbUnstageCompositionLink_var.o0S2C0_ShaderCompositionWithLink_var11 + cbStageCompositionLink_var.o0S2C0_ShaderCompositionWithLink_var12;
}

void main()
{
    float f = 0.0;
    f += o0S2C0_ShaderCompositionWithLink_Compute();
    gl_Position.z = 2.0 * gl_Position.z - gl_Position.w;
    gl_Position.y = -gl_Position.y;
}


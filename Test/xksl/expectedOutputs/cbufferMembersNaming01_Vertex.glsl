#version 410
#ifdef GL_ARB_shading_language_420pack
#extension GL_ARB_shading_language_420pack : require
#endif

layout(std140) uniform cbUnstageGenericWithLink
{
    float ShaderGenericWithLink_var07;
} cbUnstageGenericWithLink_var;

layout(std140) uniform cbStageGenericWithLink
{
    float ShaderGenericWithLink_var08;
} cbStageGenericWithLink_var;

layout(std140) uniform cbUnstageGeneric
{
    float ShaderGeneric_var05;
} cbUnstageGeneric_var;

layout(std140) uniform cbStageGeneric
{
    float ShaderGeneric_var06;
} cbStageGeneric_var;

layout(std140) uniform cbUnstageLink
{
    float ShaderWithLink_var03;
} cbUnstageLink_var;

layout(std140) uniform cbStageLink
{
    float ShaderWithLink_var04;
} cbStageLink_var;

layout(std140) uniform cbUnstage
{
    float ShaderSimple_var01;
} cbUnstage_var;

layout(std140) uniform cbStage
{
    float ShaderSimple_var02;
} cbStage_var;

layout(std140) uniform cbUnstageComposition
{
    float o0S2C0_ShaderComposition_var09;
} cbUnstageComposition_var;

layout(std140) uniform cbStageComposition
{
    float o0S2C0_ShaderComposition_var10;
} cbStageComposition_var;

layout(std140) uniform cbUnstageCompositionLink
{
    float o1S2C1_ShaderCompositionWithLink_var11;
} cbUnstageCompositionLink_var;

layout(std140) uniform cbStageCompositionLink
{
    float o1S2C1_ShaderCompositionWithLink_var12;
} cbStageCompositionLink_var;

layout(std140) uniform cbUnstageCompositionWithGeneric
{
    float o2S2C2_ShaderCompositionWithGeneric_var13;
} cbUnstageCompositionWithGeneric_var;

layout(std140) uniform cbStageCompositionWithGeneric
{
    float o2S2C2_ShaderCompositionWithGeneric_var14;
} cbStageCompositionWithGeneric_var;

float ShaderSimple_Compute()
{
    return cbUnstage_var.ShaderSimple_var01 + cbStage_var.ShaderSimple_var02;
}

float ShaderWithLink_Compute()
{
    return cbUnstageLink_var.ShaderWithLink_var03 + cbStageLink_var.ShaderWithLink_var04;
}

float ShaderGeneric_2__Compute()
{
    return cbUnstageGeneric_var.ShaderGeneric_var05 + cbStageGeneric_var.ShaderGeneric_var06;
}

float ShaderGenericWithLink_3__Compute()
{
    return cbUnstageGenericWithLink_var.ShaderGenericWithLink_var07 + cbStageGenericWithLink_var.ShaderGenericWithLink_var08;
}

float o0S2C0_ShaderComposition_Compute()
{
    return cbUnstageComposition_var.o0S2C0_ShaderComposition_var09 + cbStageComposition_var.o0S2C0_ShaderComposition_var10;
}

float o1S2C1_ShaderCompositionWithLink_Compute()
{
    return cbUnstageCompositionLink_var.o1S2C1_ShaderCompositionWithLink_var11 + cbStageCompositionLink_var.o1S2C1_ShaderCompositionWithLink_var12;
}

float o2S2C2_ShaderCompositionWithGeneric_1_2_3__Compute()
{
    return (((cbUnstageCompositionWithGeneric_var.o2S2C2_ShaderCompositionWithGeneric_var13 + cbStageCompositionWithGeneric_var.o2S2C2_ShaderCompositionWithGeneric_var14) + 1.0) + 2.0) + 3.0;
}

void main()
{
    float f = 0.0;
    f += ShaderSimple_Compute();
    f += ShaderWithLink_Compute();
    f += ShaderGeneric_2__Compute();
    f += ShaderGenericWithLink_3__Compute();
    f += o0S2C0_ShaderComposition_Compute();
    f += o1S2C1_ShaderCompositionWithLink_Compute();
    f += o2S2C2_ShaderCompositionWithGeneric_1_2_3__Compute();
    gl_Position.z = 2.0 * gl_Position.z - gl_Position.w;
    gl_Position.y = -gl_Position.y;
}


#version 450

layout(std140) uniform ShaderComp_globalCBuffer
{
    float varC;
} ShaderComp_globalCBuffer_var;

layout(std140) uniform cS5C0_ShaderComp_globalCBuffer
{
    float varC;
} cS5C0_ShaderComp_globalCBuffer_var;

layout(std140) uniform cS5C1_ShaderComp_globalCBuffer
{
    float varC;
} cS5C1_ShaderComp_globalCBuffer_var;

float ShaderA_Compute()
{
    return 5.0;
}

float cS5C0_ShaderComp_Compute()
{
    return cS5C0_ShaderComp_globalCBuffer_var.varC;
}

float cS5C1_ShaderComp_Compute()
{
    return cS5C1_ShaderComp_globalCBuffer_var.varC;
}

float main()
{
    float f = ShaderA_Compute();
    return (f + cS5C0_ShaderComp_Compute()) + cS5C1_ShaderComp_Compute();
}


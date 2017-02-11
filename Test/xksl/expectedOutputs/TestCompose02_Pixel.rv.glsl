#version 450

layout(std140) uniform ShaderComp_globalCBuffer
{
    float varC;
} ShaderComp_globalCBuffer_var;

layout(std140) uniform compS5C0_ShaderComp_globalCBuffer
{
    float varC;
} compS5C0_ShaderComp_globalCBuffer_var;

layout(std140) uniform compS5C1_ShaderComp_globalCBuffer
{
    float varC;
} compS5C1_ShaderComp_globalCBuffer_var;

float ShaderA_Compute()
{
    return 5.0;
}

float compS5C0_ShaderComp_Compute()
{
    return compS5C0_ShaderComp_globalCBuffer_var.varC;
}

float compS5C1_ShaderComp_Compute()
{
    return compS5C1_ShaderComp_globalCBuffer_var.varC;
}

float main()
{
    float f = ShaderA_Compute();
    return (f + compS5C0_ShaderComp_Compute()) + compS5C1_ShaderComp_Compute();
}


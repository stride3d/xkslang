#version 450

layout(std140) uniform ShaderComp_globalCBuffer
{
    float varC;
} ShaderComp_globalCBuffer_var;

layout(std140) uniform compS7C0_ShaderComp_globalCBuffer
{
    float varC;
} compS7C0_ShaderComp_globalCBuffer_var;

layout(std140) uniform compS7C1_ShaderComp_globalCBuffer
{
    float varC;
} compS7C1_ShaderComp_globalCBuffer_var;

float ShaderA_Compute()
{
    return 5.0;
}

float compS7C0_ShaderComp_Compute()
{
    return compS7C0_ShaderComp_globalCBuffer_var.varC;
}

float compS7C1_ShaderComp_Compute()
{
    return compS7C1_ShaderComp_globalCBuffer_var.varC;
}

float main()
{
    float f = ShaderA_Compute();
    return (f + compS7C0_ShaderComp_Compute()) + compS7C1_ShaderComp_Compute();
}


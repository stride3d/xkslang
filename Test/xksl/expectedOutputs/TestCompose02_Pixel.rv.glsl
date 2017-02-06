#version 450

layout(std140) uniform ShaderComp_globalCBuffer
{
    float varC;
} ShaderComp_globalCBuffer_var;

layout(std140) uniform compShaderAcolorA_ShaderComp_globalCBuffer
{
    float varC;
} compShaderAcolorA_ShaderComp_globalCBuffer_var;

layout(std140) uniform compShaderAcolorB_ShaderComp_globalCBuffer
{
    float varC;
} compShaderAcolorB_ShaderComp_globalCBuffer_var;

float ShaderA_Compute()
{
    return 5.0;
}

float compShaderAcolorA_ShaderComp_Compute()
{
    return compShaderAcolorA_ShaderComp_globalCBuffer_var.varC;
}

float compShaderAcolorB_ShaderComp_Compute()
{
    return compShaderAcolorB_ShaderComp_globalCBuffer_var.varC;
}

float main()
{
    float f = ShaderA_Compute();
    return (f + compShaderAcolorA_ShaderComp_Compute()) + compShaderAcolorB_ShaderComp_Compute();
}


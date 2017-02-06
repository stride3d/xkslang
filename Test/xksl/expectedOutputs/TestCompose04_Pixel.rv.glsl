#version 450

layout(std140) uniform CompA_globalCBuffer
{
    float varCA;
} CompA_globalCBuffer_var;

layout(std140) uniform compShaderAcolorB_CompA_globalCBuffer
{
    float varCA;
} compShaderAcolorB_CompA_globalCBuffer_var;

float compShaderAcolorB_CompA_Compute()
{
    return compShaderAcolorB_CompA_globalCBuffer_var.varCA;
}

float main()
{
    return compShaderAcolorB_CompA_Compute();
}


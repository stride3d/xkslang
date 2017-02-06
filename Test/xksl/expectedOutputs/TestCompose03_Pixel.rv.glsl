#version 450

layout(std140) uniform CompA_globalCBuffer
{
    float varCA;
} CompA_globalCBuffer_var;

layout(std140) uniform CompB_globalCBuffer
{
    float varCB;
} CompB_globalCBuffer_var;

layout(std140) uniform compShaderAcolorA_CompA_globalCBuffer
{
    float varCA;
} compShaderAcolorA_CompA_globalCBuffer_var;

layout(std140) uniform compShaderAcolorB_CompB_globalCBuffer
{
    float varCB;
} compShaderAcolorB_CompB_globalCBuffer_var;

float compShaderAcolorA_CompA_Compute()
{
    return compShaderAcolorA_CompA_globalCBuffer_var.varCA;
}

float compShaderAcolorB_CompB_Compute()
{
    return compShaderAcolorB_CompB_globalCBuffer_var.varCB;
}

float main()
{
    return compShaderAcolorA_CompA_Compute() + compShaderAcolorB_CompB_Compute();
}


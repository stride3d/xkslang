#version 450

layout(std140) uniform CompA_globalCBuffer
{
    float varCA;
} CompA_globalCBuffer_var;

layout(std140) uniform CompB_globalCBuffer
{
    float varCB;
} CompB_globalCBuffer_var;

layout(std140) uniform compShaderAcolor_CompA_globalCBuffer
{
    float varCA;
} compShaderAcolor_CompA_globalCBuffer_var;

layout(std140) uniform compShaderAcolor_CompB_globalCBuffer
{
    float varCB;
} compShaderAcolor_CompB_globalCBuffer_var;

float compShaderAcolor_CompB_Compute()
{
    return compShaderAcolor_CompB_globalCBuffer_var.varCB;
}

float main()
{
    return compShaderAcolor_CompB_Compute();
}


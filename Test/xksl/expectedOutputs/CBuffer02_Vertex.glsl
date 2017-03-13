#version 450

layout(std140) uniform ShaderMain_CBufferVertexStage_1
{
    float BleedingFactor;
} ShaderMain_CBufferVertexStage_1_var;

layout(std140) uniform ShaderMain_CBufferPixelStage_0
{
    float MinVariance;
} ShaderMain_CBufferPixelStage_0_var;

void main()
{
    float f = ShaderMain_CBufferVertexStage_1_var.BleedingFactor;
}


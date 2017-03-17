#version 450

layout(std140) uniform CBufferVertexStage
{
    float ShaderMain_BleedingFactor;
} CBufferVertexStage_var;

layout(std140) uniform globalCbuffer
{
    float ShaderMain_var2;
    float ShaderMain_var4[4];
    vec4 ShaderMain_var7;
} globalCbuffer_var;

layout(std140) uniform CBufferPixelStage
{
    float ShaderMain_MinVariance;
} CBufferPixelStage_var;

void main()
{
    float f = (CBufferVertexStage_var.ShaderMain_BleedingFactor + globalCbuffer_var.ShaderMain_var2) + globalCbuffer_var.ShaderMain_var4[2];
}


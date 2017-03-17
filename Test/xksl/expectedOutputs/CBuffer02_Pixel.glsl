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
    float f = CBufferPixelStage_var.ShaderMain_MinVariance + globalCbuffer_var.ShaderMain_var7.x;
}


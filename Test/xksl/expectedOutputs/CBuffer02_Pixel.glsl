#version 450

layout(std140) uniform CBufferVertexStage
{
    float BleedingFactor;
} CBufferVertexStage_var;

layout(std140) uniform CBufferPixelStage
{
    float MinVariance;
} CBufferPixelStage_var;

layout(std140) uniform globalCbuffer
{
    float var2;
    float var4[4];
    vec4 var7;
} globalCbuffer_var;

void main()
{
    float f = CBufferPixelStage_var.MinVariance + globalCbuffer_var.var7.x;
}


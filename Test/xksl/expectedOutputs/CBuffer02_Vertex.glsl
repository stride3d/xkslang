#version 450

layout(std140) uniform CBufferVertexStage
{
    float BleedingFactor;
} CBufferVertexStage_var;

layout(std140) uniform CBufferPixelStage
{
    float MinVariance;
} CBufferPixelStage_var;

layout(std140) uniform _globalCbuffer
{
    float var2;
    float var4[4];
    vec4 var7;
} _globalCbuffer_var;

void main()
{
    float f = (CBufferVertexStage_var.BleedingFactor + _globalCbuffer_var.var2) + _globalCbuffer_var.var4[2];
}


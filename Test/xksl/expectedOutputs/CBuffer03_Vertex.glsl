#version 450

layout(std140) uniform CBufferToto
{
    float BleedingFactor;
    float MinVariance;
} CBufferToto_var;

void main()
{
    float f = CBufferToto_var.BleedingFactor;
}


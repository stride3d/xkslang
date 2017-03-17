#version 450

layout(std140) uniform CBufferToto
{
    float ShaderMain_BleedingFactor;
    float ShaderMain_MinVariance;
} CBufferToto_var;

void main()
{
    float f = CBufferToto_var.ShaderMain_MinVariance;
}


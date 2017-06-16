#version 450

layout(std140) uniform CBufferVertexStage
{
    float ShaderMain_BleedingFactor;
} CBufferVertexStage_var;

layout(std140) uniform Globals
{
    float ShaderMain_var2;
    float ShaderMain_var4[4];
    vec4 ShaderMain_var7;
} Globals_var;

void main()
{
    float f = (CBufferVertexStage_var.ShaderMain_BleedingFactor + Globals_var.ShaderMain_var2) + Globals_var.ShaderMain_var4[2];
}


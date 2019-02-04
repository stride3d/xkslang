#version 410
#ifdef GL_ARB_shading_language_420pack
#extension GL_ARB_shading_language_420pack : require
#endif

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
    gl_Position.z = 2.0 * gl_Position.z - gl_Position.w;
    gl_Position.y = -gl_Position.y;
}


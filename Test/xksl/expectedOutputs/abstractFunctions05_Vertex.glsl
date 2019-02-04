#version 410
#ifdef GL_ARB_shading_language_420pack
#extension GL_ARB_shading_language_420pack : require
#endif

layout(std140) uniform Globals
{
    float ShaderMainB_varMaterialSurface;
} Globals_var;

float ShaderMainB_Compute()
{
    return Globals_var.ShaderMainB_varMaterialSurface;
}

void main()
{
    float f = ShaderMainB_Compute();
    gl_Position.z = 2.0 * gl_Position.z - gl_Position.w;
    gl_Position.y = -gl_Position.y;
}


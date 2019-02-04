#version 410
#ifdef GL_ARB_shading_language_420pack
#extension GL_ARB_shading_language_420pack : require
#endif

layout(std140) uniform Globals
{
    float ShaderCompose_varCompose;
    float o1S13C1_ShaderComposeBY_varComposeBY;
} Globals_var;

float o1S13C1_ShaderComposeBY_Compute()
{
    return Globals_var.o1S13C1_ShaderComposeBY_varComposeBY;
}

float ShaderCompose_ComputeComp()
{
    return Globals_var.ShaderCompose_varCompose + o1S13C1_ShaderComposeBY_Compute();
}

void main()
{
    float f = o1S13C1_ShaderComposeBY_Compute();
    f += ShaderCompose_ComputeComp();
    f += ShaderCompose_ComputeComp();
    gl_Position.z = 2.0 * gl_Position.z - gl_Position.w;
    gl_Position.y = -gl_Position.y;
}


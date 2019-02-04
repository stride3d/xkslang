#version 410
#ifdef GL_ARB_shading_language_420pack
#extension GL_ARB_shading_language_420pack : require
#endif

layout(std140) uniform Globals
{
    float Base_basef;
    vec4 Base_basef4;
} Globals_var;

vec4 Base_ComputeBase(float f)
{
    return vec4(f, f, f, f);
}

float main()
{
    float dist = length(Globals_var.Base_basef4);
    float param = Globals_var.Base_basef;
    vec2 direction = vec2(normalize(Base_ComputeBase(param)).xy);
    return direction.x + dist;
}


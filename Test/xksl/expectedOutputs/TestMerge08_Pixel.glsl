#version 450

layout(std140) uniform globalCbuffer
{
    float Base_basef;
    vec4 Base_basef4;
} globalCbuffer_var;

vec4 Base_ComputeBase(float f)
{
    return vec4(f, f, f, f);
}

float main()
{
    float dist = length(globalCbuffer_var.Base_basef4);
    float param = globalCbuffer_var.Base_basef;
    vec2 direction = vec2(normalize(Base_ComputeBase(param)).xy);
    return direction.x + dist;
}


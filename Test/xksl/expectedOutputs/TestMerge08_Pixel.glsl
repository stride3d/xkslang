#version 450

layout(std140) uniform globalCbuffer
{
    float basef;
    vec4 basef4;
} globalCbuffer_var;

vec4 Base_ComputeBase(float f)
{
    return vec4(f, f, f, f);
}

float main()
{
    float dist = length(globalCbuffer_var.basef4);
    float param = globalCbuffer_var.basef;
    vec2 direction = vec2(normalize(Base_ComputeBase(param)).xy);
    return direction.x + dist;
}


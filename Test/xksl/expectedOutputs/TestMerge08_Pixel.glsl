#version 450

layout(std140) uniform _globalCbuffer
{
    float basef;
    vec4 basef4;
} _globalCbuffer_var;

vec4 Base_ComputeBase(float f)
{
    return vec4(f, f, f, f);
}

float main()
{
    float dist = length(_globalCbuffer_var.basef4);
    float param = _globalCbuffer_var.basef;
    vec2 direction = vec2(normalize(Base_ComputeBase(param)).xy);
    return direction.x + dist;
}


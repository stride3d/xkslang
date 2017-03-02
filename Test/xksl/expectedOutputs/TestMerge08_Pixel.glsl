#version 450

layout(std140) uniform Base_globalCBuffer
{
    float basef;
    vec4 basef4;
} Base_globalCBuffer_var;

vec4 Base_ComputeBase(float f)
{
    return vec4(f, f, f, f);
}

float main()
{
    float dist = length(Base_globalCBuffer_var.basef4);
    float param = Base_globalCBuffer_var.basef;
    vec2 direction = vec2(normalize(Base_ComputeBase(param)).xy);
    return direction.x + dist;
}


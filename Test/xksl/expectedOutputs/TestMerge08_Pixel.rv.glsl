#version 450

layout(std140) uniform Base_globalCBuffer
{
    float basef;
    vec4 basef4;
} var_Base_globalCBuffer;

vec4 Base_ComputeBase(float f)
{
    return vec4(f, f, f, f);
}

float main()
{
    float dist = length(var_Base_globalCBuffer.basef4);
    float param = var_Base_globalCBuffer.basef;
    vec2 direction = vec2(normalize(Base_ComputeBase(param)).xy);
    return direction.x + dist;
}


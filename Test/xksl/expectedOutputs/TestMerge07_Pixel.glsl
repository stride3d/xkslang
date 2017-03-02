#version 450

float Base_ComputeBase(float distance)
{
    return clamp(1.0 - distance, 0.0, 1.0);
}

float main()
{
    vec4 aVec4 = vec4(1.0);
    float distance = length(aVec4);
    float param = distance;
    return Base_ComputeBase(param);
}


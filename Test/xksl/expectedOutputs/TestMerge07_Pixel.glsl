#version 410
#ifdef GL_ARB_shading_language_420pack
#extension GL_ARB_shading_language_420pack : require
#endif

float Base_ComputeBase(float _distance)
{
    return clamp(1.0 - _distance, 0.0, 1.0);
}

float main()
{
    vec4 aVec4 = vec4(1.0);
    float _distance = length(aVec4);
    float param = _distance;
    return Base_ComputeBase(param);
}


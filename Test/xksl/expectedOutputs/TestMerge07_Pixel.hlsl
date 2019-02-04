float Base_ComputeBase(float _distance)
{
    return clamp(1.0f - _distance, 0.0f, 1.0f);
}

float frag_main()
{
    float4 aVec4 = 1.0f.xxxx;
    float _distance = length(aVec4);
    float param = _distance;
    return Base_ComputeBase(param);
}

void main()
{
    frag_main();
}

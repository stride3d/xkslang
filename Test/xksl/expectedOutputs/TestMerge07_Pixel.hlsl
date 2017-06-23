float Base_ComputeBase(float distance)
{
    return clamp(1.0f - distance, 0.0f, 1.0f);
}

float frag_main()
{
    float4 aVec4 = float4(1.0f, 1.0f, 1.0f, 1.0f);
    float distance = length(aVec4);
    float param = distance;
    return Base_ComputeBase(param);
}

void main()
{
    frag_main();
}

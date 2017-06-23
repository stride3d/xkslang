cbuffer Globals
{
    float Base_basef;
    float4 Base_basef4;
};

float4 Base_ComputeBase(float f)
{
    return float4(f, f, f, f);
}

float frag_main()
{
    float dist = length(Base_basef4);
    float param = Base_basef;
    float2 direction = float2(normalize(Base_ComputeBase(param)).xy);
    return direction.x + dist;
}

void main()
{
    frag_main();
}

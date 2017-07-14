cbuffer PerDraw
{
    float4x4 Transform_WorldInverse;
    float4x4 Transform_WorldInverseTranspose;
    float4x4 Transform_WorldView;
    float4x4 Transform_WorldViewInverse;
    float4x4 Transform_WorldViewProjection;
    float3 Transform_WorldScale;
    float4 Transform_EyeMS;
};
cbuffer PerLight
{
    float4 ShaderComp_aFloat;
};

float4 o2S2C1_ShaderComp_Compute()
{
    return Transform_EyeMS + ShaderComp_aFloat;
}

float4 o2S2C1_o1S2C0_ShaderComp_Compute()
{
    return Transform_EyeMS + ShaderComp_aFloat;
}

float4 o2S2C1_ShaderComp2_Compute()
{
    return o2S2C1_ShaderComp_Compute() + o2S2C1_o1S2C0_ShaderComp_Compute();
}

void frag_main()
{
    float4 f = Transform_EyeMS + o2S2C1_ShaderComp2_Compute();
}

void main()
{
    frag_main();
}

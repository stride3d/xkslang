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
    float4 o0S2C0_ShaderComp_aFloat;
};

float4 o0S2C0_ShaderComp_Compute()
{
    return Transform_EyeMS + o0S2C0_ShaderComp_aFloat;
}

void vert_main()
{
    float4 f = Transform_EyeMS + o0S2C0_ShaderComp_Compute();
}

void main()
{
    vert_main();
}

Texture3D<uint4> ShaderComp_LightClusters;

void o0S2C0_ShaderComp_Compute()
{
    float2 uv2 = 0.5f.xx;
    uint2 lightData = uint2(ShaderComp_LightClusters.Load(int4(int4(int2(uv2), 0, 0).xyz, int4(int2(uv2), 0, 0).w)).xy);
}

void o1S2C1_ShaderComp_Compute()
{
    float2 uv2 = 0.5f.xx;
    uint2 lightData = uint2(ShaderComp_LightClusters.Load(int4(int4(int2(uv2), 0, 0).xyz, int4(int2(uv2), 0, 0).w)).xy);
}

void vert_main()
{
    o0S2C0_ShaderComp_Compute();
    o1S2C1_ShaderComp_Compute();
}

void main()
{
    vert_main();
}

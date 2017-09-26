Texture3D<uint4> ShaderMain_LightClusters;
Buffer<float4> ShaderMain_PointLights;
Buffer<float4> ShaderMain_SpotLights;

void vert_main()
{
    float2 texCoord = float2(0.0f, 0.0f);
    uint2 lightData = uint2(ShaderMain_LightClusters.Load(int4(int4(int2(texCoord), 0, 0).xyz, int4(int2(texCoord), 0, 0).w)).xy);
    int realLightIndex = 1;
    float4 pointLight1 = ShaderMain_PointLights.Load(realLightIndex * 2);
    float4 spotLight1 = ShaderMain_SpotLights.Load(realLightIndex * 4);
}

void main()
{
    vert_main();
}

cbuffer cb
{
    int ShaderMain_lightIndex;
};
Texture3D<uint4> ShaderMain_LightClusters;
Buffer<uint4> ShaderMain_LightIndices;
Buffer<float4> ShaderMain_PointLights;

void frag_main()
{
    int realLightIndex = int(ShaderMain_LightIndices.Load(ShaderMain_lightIndex).x);
    float4 pointLight1 = ShaderMain_PointLights.Load(realLightIndex * 2);
    uint2 lightData = uint2(ShaderMain_LightClusters.Load(int4(int4(0, 1, 2, 3).xyz, int4(0, 1, 2, 3).w)).xy);
}

void main()
{
    frag_main();
}

SamplerState ShaderMain_Sampler0;
TextureCube<float4> ShaderMain_CubeMap;

void frag_main()
{
    float3 direction = float3(1.0f, 0.0f, 0.0f);
    float4 color = ShaderMain_CubeMap.Sample(ShaderMain_Sampler0, direction);
}

void main()
{
    frag_main();
}

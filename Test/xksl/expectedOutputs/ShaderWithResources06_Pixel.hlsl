cbuffer cb
{
    float3 ShaderMain_direction;
    float2 ShaderMain_uv2;
};
TextureCube<float4> ShaderMain_CubeMap;
SamplerState ShaderMain_Sampler0;

void frag_main()
{
    float4 color = ShaderMain_CubeMap.Sample(ShaderMain_Sampler0, ShaderMain_direction);
}

void main()
{
    frag_main();
}

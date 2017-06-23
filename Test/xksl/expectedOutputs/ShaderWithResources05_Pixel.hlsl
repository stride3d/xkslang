cbuffer Globals
{
    float3 ShaderMain_direction;
    float2 ShaderMain_uv2;
};
SamplerState ShaderMain_Sampler0;
TextureCube<float4> ShaderMain_CubeMap;

void frag_main()
{
    float4 color = ShaderMain_CubeMap.Sample(ShaderMain_Sampler0, ShaderMain_direction);
}

void main()
{
    frag_main();
}

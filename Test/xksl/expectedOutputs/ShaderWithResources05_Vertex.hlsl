cbuffer Globals
{
    float3 ShaderMain_direction;
    float2 ShaderMain_uv2;
};
SamplerState ShaderMain_Sampler0;
Texture2D<float4> ShaderMain_Texture0;

void vert_main()
{
    float4 color = ShaderMain_Texture0.Sample(ShaderMain_Sampler0, ShaderMain_uv2);
}

void main()
{
    vert_main();
}

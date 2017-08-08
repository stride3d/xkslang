Texture2D<float4> ShaderMain_Texture0;
SamplerState ShaderMain_Sampler0;
Texture2D<float4> ShaderMain_Texture1;
SamplerState ShaderMain_Sampler1;

void frag_main()
{
    float4 color0 = ShaderMain_Texture0.Sample(ShaderMain_Sampler0, float2(0.5f, 0.5f));
    float4 color1 = ShaderMain_Texture1.Sample(ShaderMain_Sampler1, float2(0.5f, 0.5f));
}

void main()
{
    frag_main();
}

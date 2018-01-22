Texture2D<float4> ShaderMain_Texture0;
SamplerState ShaderMain_Sampler0;
Texture2D<float4> ShaderMain_Texture1;
SamplerState ShaderMain_Sampler1;

void frag_main()
{
    float4 color0 = ShaderMain_Texture0.Sample(ShaderMain_Sampler0, 0.5f.xx);
    float4 color1 = ShaderMain_Texture1.Sample(ShaderMain_Sampler1, 0.5f.xx);
}

void main()
{
    frag_main();
}

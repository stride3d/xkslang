cbuffer Globals
{
    float ShaderMain_var2;
    float ShaderMain_var4[4];
    float4 ShaderMain_var7;
};
cbuffer CBufferPixelStage
{
    float ShaderMain_MinVariance;
};

void frag_main()
{
    float f = ShaderMain_MinVariance + ShaderMain_var7.x;
}

void main()
{
    frag_main();
}

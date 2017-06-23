cbuffer CBufferVertexStage
{
    float ShaderMain_BleedingFactor;
};
cbuffer Globals
{
    float ShaderMain_var2;
    float ShaderMain_var4[4];
    float4 ShaderMain_var7;
};

void vert_main()
{
    float f = (ShaderMain_BleedingFactor + ShaderMain_var2) + ShaderMain_var4[2];
}

void main()
{
    vert_main();
}

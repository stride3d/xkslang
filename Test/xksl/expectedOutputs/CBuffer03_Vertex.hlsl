cbuffer CBufferToto
{
    float ShaderMain_BleedingFactor;
    float ShaderMain_MinVariance;
};

void vert_main()
{
    float f = ShaderMain_BleedingFactor;
}

void main()
{
    vert_main();
}

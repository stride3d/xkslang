cbuffer PerLighting
{
    float ShaderMain_BleedingFactor;
    float ShaderMain_MinVariance;
};

void frag_main()
{
    float f = ShaderMain_MinVariance;
}

void main()
{
    frag_main();
}

cbuffer Globals
{
    float o0S2C0_ShaderComp_varC;
    float o1S20C0_ShaderComp_varC;
    float o2S33C0_ShaderComp_varC;
    float o3S2C1_ShaderComp_varC;
};

float o0S2C0_ShaderComp_Compute()
{
    return o0S2C0_ShaderComp_varC;
}

float o3S2C1_ShaderComp_Compute()
{
    return o3S2C1_ShaderComp_varC;
}

float o2S33C0_ShaderComp_Compute()
{
    return o2S33C0_ShaderComp_varC;
}

float ShaderA_Compute()
{
    return 1.0f + o2S33C0_ShaderComp_Compute();
}

float o1S20C0_ShaderComp_Compute()
{
    return o1S20C0_ShaderComp_varC;
}

float ShaderB_Compute()
{
    return 2.0f + o1S20C0_ShaderComp_Compute();
}

float ShaderMain_Compute()
{
    return ((o0S2C0_ShaderComp_Compute() + o3S2C1_ShaderComp_Compute()) + ShaderA_Compute()) + ShaderB_Compute();
}

void frag_main()
{
    float f = ShaderMain_Compute();
}

void main()
{
    frag_main();
}

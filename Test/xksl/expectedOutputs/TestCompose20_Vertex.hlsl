cbuffer Globals
{
    float o0S29C0_ShaderComp_varC;
    float o1S16C0_ShaderComp_varC;
};

float o0S29C0_ShaderComp_Compute()
{
    return o0S29C0_ShaderComp_varC;
}

float ShaderA_Compute()
{
    return 1.0f + o0S29C0_ShaderComp_Compute();
}

float o1S16C0_ShaderComp_Compute()
{
    return o1S16C0_ShaderComp_varC;
}

float ShaderB_Compute()
{
    return 2.0f + o1S16C0_ShaderComp_Compute();
}

float ShaderMain_Compute()
{
    return ShaderA_Compute() + ShaderB_Compute();
}

void vert_main()
{
    float f = ShaderMain_Compute();
}

void main()
{
    vert_main();
}

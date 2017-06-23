cbuffer Globals
{
    float o0S5C0_ShaderComp_varC;
    float o1S5C1_ShaderComp_varC;
};

float ShaderA_Compute()
{
    return 5.0f;
}

float o0S5C0_ShaderComp_Compute()
{
    return o0S5C0_ShaderComp_varC;
}

float o1S5C1_ShaderComp_Compute()
{
    return o1S5C1_ShaderComp_varC;
}

float frag_main()
{
    float f = ShaderA_Compute();
    return (f + o0S5C0_ShaderComp_Compute()) + o1S5C1_ShaderComp_Compute();
}

void main()
{
    frag_main();
}

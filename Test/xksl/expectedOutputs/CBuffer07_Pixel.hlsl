cbuffer Globals
{
    float ShaderA_varA;
    float o0S5C0_ShaderComp_varC1;
    float o1S5C1_ShaderComp_varC1;
};
cbuffer PerDraw
{
    float o0S5C0_ShaderComp_varDraw1;
    float o1S5C1_ShaderComp_varDraw1;
};

float o0S5C0_ShaderComp_Compute()
{
    return o0S5C0_ShaderComp_varC1 + o0S5C0_ShaderComp_varDraw1;
}

float o1S5C1_ShaderComp_Compute()
{
    return o1S5C1_ShaderComp_varC1 + o1S5C1_ShaderComp_varDraw1;
}

float frag_main()
{
    return (ShaderA_varA + o0S5C0_ShaderComp_Compute()) + o1S5C1_ShaderComp_Compute();
}

void main()
{
    frag_main();
}

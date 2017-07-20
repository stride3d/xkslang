cbuffer PerToto
{
    float ShaderA_varToto;
    float o0S14C0_ShaderComp_varCompToto;
    float o1S14C1_ShaderComp_varCompToto;
};
cbuffer Globals
{
    float ShaderA_varG1;
    float ShaderA_varG2;
    float o0S14C0_ShaderComp_varG1;
    float o0S14C0_ShaderComp_varG2;
    float o1S14C1_ShaderComp_varG1;
};
cbuffer PerDraw
{
    float ShaderA_varDrawA;
    float ShaderA_varDraw3;
    float o0S14C0_ShaderComp_varDraw1;
    float o0S14C0_ShaderComp_varDraw2;
    float o0S14C0_ShaderComp_varDraw3;
    float o1S14C1_ShaderComp_varDraw3;
};

float o1S14C1_ShaderComp_Compute()
{
    return (((o0S14C0_ShaderComp_varDraw1 + o1S14C1_ShaderComp_varDraw3) + o1S14C1_ShaderComp_varG1) + o0S14C0_ShaderComp_varG2) + o1S14C1_ShaderComp_varCompToto;
}

float frag_main()
{
    return (ShaderA_varDrawA + o1S14C1_ShaderComp_Compute()) + ShaderA_varG2;
}

void main()
{
    frag_main();
}

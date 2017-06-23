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
    float ShaderComp_varG2;
    float o1S14C1_ShaderComp_varG1;
};
cbuffer PerDraw
{
    float ShaderA_varDrawA;
    float ShaderA_varDraw3;
    float ShaderComp_varDraw1;
    float ShaderComp_varDraw2;
    float o0S14C0_ShaderComp_varDraw3;
    float o1S14C1_ShaderComp_varDraw3;
};

float o0S14C0_ShaderComp_Compute()
{
    return (((ShaderComp_varDraw1 + o0S14C0_ShaderComp_varDraw3) + o0S14C0_ShaderComp_varG1) + ShaderComp_varG2) + o0S14C0_ShaderComp_varCompToto;
}

float vert_main()
{
    return (ShaderA_varToto + o0S14C0_ShaderComp_Compute()) + ShaderA_varG1;
}

void main()
{
    vert_main();
}

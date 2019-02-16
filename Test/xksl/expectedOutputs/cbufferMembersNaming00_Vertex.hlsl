cbuffer cbUnstageCompositionLink
{
    float o0S2C0_ShaderCompositionWithLink_vara1;
    float o0S2C0_ShaderCompositionWithLink_vara2;
};
cbuffer cbStageCompositionLink
{
    float o0S2C0_ShaderCompositionWithLink_varb1;
    float o0S2C0_ShaderCompositionWithLink_varb2;
};

float o0S2C0_ShaderCompositionWithLink_Compute()
{
    return o0S2C0_ShaderCompositionWithLink_vara1 + o0S2C0_ShaderCompositionWithLink_varb1;
}

void vert_main()
{
    float f = 0.0f;
    f += o0S2C0_ShaderCompositionWithLink_Compute();
}

void main()
{
    vert_main();
}

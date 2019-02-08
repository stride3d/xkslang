cbuffer cbUnstageCompositionLink
{
    float o0S2C0_ShaderCompositionWithLink_var11;
};
cbuffer cbStageCompositionLink
{
    float o0S2C0_ShaderCompositionWithLink_var12;
};

float o0S2C0_ShaderCompositionWithLink_Compute()
{
    return o0S2C0_ShaderCompositionWithLink_var11 + o0S2C0_ShaderCompositionWithLink_var12;
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

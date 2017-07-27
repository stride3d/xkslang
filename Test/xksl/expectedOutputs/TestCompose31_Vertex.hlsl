cbuffer cbUnstageComposition
{
    float o0S2C0_ShaderComposition_var09;
};
cbuffer cbStageComposition
{
    float o0S2C0_ShaderComposition_var10;
};

float o0S2C0_ShaderComposition_Compute()
{
    return o0S2C0_ShaderComposition_var09 + o0S2C0_ShaderComposition_var10;
}

void vert_main()
{
    float f = 0.0f;
    f += o0S2C0_ShaderComposition_Compute();
}

void main()
{
    vert_main();
}

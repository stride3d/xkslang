cbuffer cbComposition
{
    float o0S2C0_ShaderComposition_var09;
};

float o0S2C0_ShaderComposition_Compute()
{
    return o0S2C0_ShaderComposition_var09;
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

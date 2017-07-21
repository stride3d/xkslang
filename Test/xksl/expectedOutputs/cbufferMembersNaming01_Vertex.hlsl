cbuffer cbUnstageGenericWithLink
{
    float ShaderGenericWithLink_var07;
};
cbuffer cbStageGenericWithLink
{
    float ShaderGenericWithLink_var08;
};
cbuffer cbUnstageGeneric
{
    float ShaderGeneric_var05;
};
cbuffer cbStageGeneric
{
    float ShaderGeneric_var06;
};
cbuffer cbUnstageLink
{
    float ShaderWithLink_var03;
};
cbuffer cbStageLink
{
    float ShaderWithLink_var04;
};
cbuffer cbUnstage
{
    float ShaderSimple_var01;
};
cbuffer cbStage
{
    float ShaderSimple_var02;
};
cbuffer cbUnstageComposition
{
    float o0S2C0_ShaderComposition_var09;
};
cbuffer cbStageComposition
{
    float o0S2C0_ShaderComposition_var10;
};
cbuffer cbUnstageCompositionLink
{
    float o1S2C1_ShaderCompositionWithLink_var11;
};
cbuffer cbStageCompositionLink
{
    float o1S2C1_ShaderCompositionWithLink_var12;
};
cbuffer cbUnstageCompositionWithGeneric
{
    float o2S2C2_ShaderCompositionWithGeneric_var13;
};
cbuffer cbStageCompositionWithGeneric
{
    float o2S2C2_ShaderCompositionWithGeneric_var14;
};

float ShaderSimple_Compute()
{
    return ShaderSimple_var01 + ShaderSimple_var02;
}

float ShaderWithLink_Compute()
{
    return ShaderWithLink_var03 + ShaderWithLink_var04;
}

float ShaderGeneric_2__Compute()
{
    return ShaderGeneric_var05 + ShaderGeneric_var06;
}

float ShaderGenericWithLink_3__Compute()
{
    return ShaderGenericWithLink_var07 + ShaderGenericWithLink_var08;
}

float o0S2C0_ShaderComposition_Compute()
{
    return o0S2C0_ShaderComposition_var09 + o0S2C0_ShaderComposition_var10;
}

float o1S2C1_ShaderCompositionWithLink_Compute()
{
    return o1S2C1_ShaderCompositionWithLink_var11 + o1S2C1_ShaderCompositionWithLink_var12;
}

float o2S2C2_ShaderCompositionWithGeneric_1_2_3__Compute()
{
    return (((o2S2C2_ShaderCompositionWithGeneric_var13 + o2S2C2_ShaderCompositionWithGeneric_var14) + 1.0f) + 2.0f) + 3.0f;
}

void vert_main()
{
    float f = 0.0f;
    f += ShaderSimple_Compute();
    f += ShaderWithLink_Compute();
    f += ShaderGeneric_2__Compute();
    f += ShaderGenericWithLink_3__Compute();
    f += o0S2C0_ShaderComposition_Compute();
    f += o1S2C1_ShaderCompositionWithLink_Compute();
    f += o2S2C2_ShaderCompositionWithGeneric_1_2_3__Compute();
}

void main()
{
    vert_main();
}

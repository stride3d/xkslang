cbuffer Globals
{
    float ShaderBase_varBase;
    float o0S2C0_ShaderCompose_varCompose;
    float o0S2C0_ShaderBase_varBase;
};

float ShaderBase_Compute()
{
    return ShaderBase_varBase;
}

float o0S2C0_ShaderBase_Compute()
{
    return o0S2C0_ShaderBase_varBase;
}

float o0S2C0_ShaderCompose_ComputeComp()
{
    return o0S2C0_ShaderCompose_varCompose + o0S2C0_ShaderBase_Compute();
}

void vert_main()
{
    float f = ShaderBase_Compute();
    f += o0S2C0_ShaderCompose_ComputeComp();
}

void main()
{
    vert_main();
}

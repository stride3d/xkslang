cbuffer Globals
{
    float o0S15C0_ShaderCompose_varCompose;
    float o0S15C0_ShaderBase_varBase;
    float o0S15C0_ShaderComposeX_varComposeX;
};

float o0S15C0_ShaderBase_Compute()
{
    return o0S15C0_ShaderBase_varBase;
}

float o0S15C0_ShaderComposeX_Compute()
{
    return o0S15C0_ShaderBase_Compute() + o0S15C0_ShaderComposeX_varComposeX;
}

float o0S15C0_ShaderCompose_ComputeComp()
{
    return o0S15C0_ShaderCompose_varCompose + o0S15C0_ShaderComposeX_Compute();
}

void vert_main()
{
    float f = o0S15C0_ShaderComposeX_Compute();
    f += o0S15C0_ShaderCompose_ComputeComp();
}

void main()
{
    vert_main();
}

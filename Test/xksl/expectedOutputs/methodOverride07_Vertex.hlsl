cbuffer Globals
{
    float ShaderMainX_varMainX;
    float ShaderMain_varMain;
    float o0S15C0_ShaderCompose_varCompose;
    float o0S15C0_ShaderComposeX2_varComposeX2;
    float o0S15C0_ShaderComposeX1_varComposeX1;
};

float ShaderMain_Compute()
{
    return ShaderMain_varMain;
}

float ShaderMainX_Compute()
{
    return ShaderMain_Compute() + ShaderMainX_varMainX;
}

float o0S15C0_ShaderComposeX1_Compute()
{
    return ShaderMainX_Compute() + o0S15C0_ShaderComposeX1_varComposeX1;
}

float o0S15C0_ShaderComposeX2_Compute()
{
    return o0S15C0_ShaderComposeX1_Compute() + o0S15C0_ShaderComposeX2_varComposeX2;
}

float o0S15C0_ShaderCompose_ComputeComp()
{
    return o0S15C0_ShaderCompose_varCompose + o0S15C0_ShaderComposeX2_Compute();
}

void vert_main()
{
    float f = o0S15C0_ShaderComposeX2_Compute();
    f += o0S15C0_ShaderCompose_ComputeComp();
}

void main()
{
    vert_main();
}

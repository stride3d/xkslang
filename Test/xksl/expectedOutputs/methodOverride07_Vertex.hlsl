cbuffer Globals
{
    float ShaderMainX_varMainX;
    float ShaderMain_varMain;
    float o0S15C0_ShaderCompose_varCompose;
    float o0S15C0_ShaderComposeX2_varComposeX2;
    float o0S15C0_ShaderComposeX1_varComposeX1;
    float o1S15C1_ShaderCompose_varCompose;
    float o1S15C1_ShaderComposeY2_varComposeY2;
    float o1S15C1_ShaderComposeY1_varComposeY1;
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

float o1S15C1_ShaderComposeY1_Compute()
{
    return o0S15C0_ShaderComposeX2_Compute() + o1S15C1_ShaderComposeY1_varComposeY1;
}

float o1S15C1_ShaderComposeY2_Compute()
{
    return o1S15C1_ShaderComposeY1_Compute() + o1S15C1_ShaderComposeY2_varComposeY2;
}

float o0S15C0_ShaderCompose_ComputeComp()
{
    return o0S15C0_ShaderCompose_varCompose + o1S15C1_ShaderComposeY2_Compute();
}

float o1S15C1_ShaderCompose_ComputeComp()
{
    return o1S15C1_ShaderCompose_varCompose + o1S15C1_ShaderComposeY2_Compute();
}

void vert_main()
{
    float f = o1S15C1_ShaderComposeY2_Compute();
    f += o0S15C0_ShaderCompose_ComputeComp();
    f += o1S15C1_ShaderCompose_ComputeComp();
}

void main()
{
    vert_main();
}

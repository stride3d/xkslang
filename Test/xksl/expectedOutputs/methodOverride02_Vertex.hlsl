cbuffer Globals
{
    float ShaderMain_varMain;
    float o0S5C0_ShaderCompose_varCompose;
};

float ShaderMain_Compute()
{
    return ShaderMain_varMain;
}

float o0S5C0_ShaderCompose_ComputeComp()
{
    return o0S5C0_ShaderCompose_varCompose + ShaderMain_Compute();
}

void vert_main()
{
    float f = ShaderMain_Compute();
    f += o0S5C0_ShaderCompose_ComputeComp();
}

void main()
{
    vert_main();
}

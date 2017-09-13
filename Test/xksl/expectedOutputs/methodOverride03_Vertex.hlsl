cbuffer Globals
{
    float ShaderMainX_varMainX;
    float o0S13C0_ShaderCompose_varCompose;
};

float ShaderMainX_Compute()
{
    return ShaderMainX_varMainX;
}

float o0S13C0_ShaderCompose_ComputeComp()
{
    return o0S13C0_ShaderCompose_varCompose + ShaderMainX_Compute();
}

void vert_main()
{
    float f = ShaderMainX_Compute();
    f += o0S13C0_ShaderCompose_ComputeComp();
}

void main()
{
    vert_main();
}

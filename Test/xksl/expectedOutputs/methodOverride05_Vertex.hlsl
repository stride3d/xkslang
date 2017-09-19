cbuffer Globals
{
    float ShaderCompose_varCompose;
    float o1S13C1_ShaderComposeBY_varComposeBY;
};

float o1S13C1_ShaderComposeBY_Compute()
{
    return o1S13C1_ShaderComposeBY_varComposeBY;
}

float ShaderCompose_ComputeComp()
{
    return ShaderCompose_varCompose + o1S13C1_ShaderComposeBY_Compute();
}

void vert_main()
{
    float f = o1S13C1_ShaderComposeBY_Compute();
    f += ShaderCompose_ComputeComp();
    f += ShaderCompose_ComputeComp();
}

void main()
{
    vert_main();
}

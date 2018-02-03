cbuffer Globals
{
    float ShaderMain_varMain;
};

float ShaderMain_Compute()
{
    return ShaderMain_varMain;
}

void vert_main()
{
    float f = ShaderMain_Compute();
}

void main()
{
    vert_main();
}

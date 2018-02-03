cbuffer Globals
{
    float ShaderMainB_varMaterialSurface;
};

float ShaderMainB_Compute()
{
    return ShaderMainB_varMaterialSurface;
}

void vert_main()
{
    float f = ShaderMainB_Compute();
}

void main()
{
    vert_main();
}

cbuffer Globals
{
    float ShaderMainB_varMainB;
};

float ShaderMainB_stageCall()
{
    return ShaderMainB_varMainB;
}

void vert_main()
{
    float f = ShaderMainB_stageCall();
}

void main()
{
    vert_main();
}

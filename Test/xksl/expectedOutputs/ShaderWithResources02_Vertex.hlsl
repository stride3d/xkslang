cbuffer Globals
{
    float ShaderMain_aFloat;
};

void vert_main()
{
    float f = ShaderMain_aFloat;
}

void main()
{
    vert_main();
}

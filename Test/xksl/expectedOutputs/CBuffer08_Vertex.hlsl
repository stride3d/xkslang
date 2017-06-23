cbuffer Globals
{
    float ShaderPSMain_var5;
    float ShaderPSMain_var1;
    float ShaderPSMain_var2;
    float ShaderVSMain_var1;
    float ShaderVSMain_var8;
};

void vert_main()
{
    float f = ShaderVSMain_var1 + ShaderVSMain_var8;
}

void main()
{
    vert_main();
}

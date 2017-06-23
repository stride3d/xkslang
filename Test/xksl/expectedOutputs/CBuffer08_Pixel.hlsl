cbuffer Globals
{
    float ShaderPSMain_var5;
    float ShaderPSMain_var1;
    float ShaderPSMain_var2;
    float ShaderVSMain_var1;
    float ShaderVSMain_var8;
};

void frag_main()
{
    float f = (ShaderPSMain_var5 + ShaderPSMain_var2) + ShaderPSMain_var1;
}

void main()
{
    frag_main();
}

#version 450

layout(std140) uniform globalCbuffer
{
    float ShaderPSMain_var5;
    float ShaderPSMain_var1;
    float ShaderPSMain_var2;
    float ShaderVSMain_var1;
    float ShaderVSMain_var8;
} globalCbuffer_var;

void main()
{
    float f = (globalCbuffer_var.ShaderPSMain_var5 + globalCbuffer_var.ShaderPSMain_var2) + globalCbuffer_var.ShaderPSMain_var1;
}


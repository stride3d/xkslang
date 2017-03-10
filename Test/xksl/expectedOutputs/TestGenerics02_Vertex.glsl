#version 450

layout(std140) uniform ShaderMain_globalCBuffer
{
    float ColorAberrations[5];
} ShaderMain_globalCBuffer_var;

void main()
{
    float res = 0.0;
    for (int i = 0; i < 5; i++)
    {
        res += ShaderMain_globalCBuffer_var.ColorAberrations[i];
    }
}


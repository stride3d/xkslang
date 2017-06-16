#version 450

layout(std140) uniform Globals
{
    float ShaderMain_5__ColorAberrations[5];
} Globals_var;

void main()
{
    float res = 0.0;
    for (int i = 0; i < 5; i++)
    {
        res += Globals_var.ShaderMain_5__ColorAberrations[i];
    }
}


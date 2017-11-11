#version 450

layout(std140) uniform Globals
{
    float ShaderMain_anArray[25];
} Globals_var;

void main()
{
    float toto = 0.0;
    for (int i = 0; i < 25; i++)
    {
        toto += Globals_var.ShaderMain_anArray[i];
    }
}


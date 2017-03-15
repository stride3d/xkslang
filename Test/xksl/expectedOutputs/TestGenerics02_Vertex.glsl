#version 450

layout(std140) uniform _globalCbuffer
{
    float ColorAberrations[5];
} _globalCbuffer_var;

void main()
{
    float res = 0.0;
    for (int i = 0; i < 5; i++)
    {
        res += _globalCbuffer_var.ColorAberrations[i];
    }
}


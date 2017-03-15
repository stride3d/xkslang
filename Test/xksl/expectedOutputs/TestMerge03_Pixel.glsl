#version 450

struct globalStreams
{
    float sbase1_0;
    int sbase2_1;
};

layout(std140) uniform _globalCbuffer
{
    int Var0;
    int Var1;
} _globalCbuffer_var;

layout(std140) uniform _5
{
    float Var2;
} _var;

globalStreams globalStreams_var;

int main()
{
    return int(((globalStreams_var.sbase1_0 + float(_globalCbuffer_var.Var1)) + _var.Var2) + 2.0);
}


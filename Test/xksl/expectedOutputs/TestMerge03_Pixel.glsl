#version 450

struct globalStreams
{
    float sbase1_0;
    int sbase2_1;
};

layout(std140) uniform globalCbuffer
{
    int Var0;
    int Var1;
    float Var2;
} globalCbuffer_var;

globalStreams globalStreams_var;

int main()
{
    return int(((globalStreams_var.sbase1_0 + float(globalCbuffer_var.Var1)) + globalCbuffer_var.Var2) + 2.0);
}


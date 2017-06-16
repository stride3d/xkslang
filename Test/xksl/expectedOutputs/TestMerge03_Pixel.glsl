#version 450

struct globalStreams
{
    float sbase1_0;
    int sbase2_1;
};

layout(std140) uniform Globals
{
    int Base_Var1;
    float Base_Var2;
} Globals_var;

globalStreams globalStreams_var;

int main()
{
    return int(((globalStreams_var.sbase1_0 + float(Globals_var.Base_Var1)) + Globals_var.Base_Var2) + 2.0);
}


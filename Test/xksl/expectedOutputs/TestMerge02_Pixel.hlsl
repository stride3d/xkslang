struct globalStreams
{
    float sbase1_0;
    int sbase2_1;
    int sa1_2;
};

cbuffer Globals
{
    int shaderA_Var0;
};

static globalStreams globalStreams_var;

int frag_main()
{
    return ((shaderA_Var0 + globalStreams_var.sa1_2) + 2) + 3;
}

void main()
{
    frag_main();
}

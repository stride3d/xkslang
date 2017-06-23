struct globalStreams
{
    float sbase1_0;
    int sbase2_1;
    int sa1_2;
};

cbuffer Globals
{
    int Base_Var1;
};
cbuffer C1
{
    int shaderA_Var1;
};

static globalStreams globalStreams_var;

int frag_main()
{
    return int(((((2.0f + globalStreams_var.sbase1_0) + float(shaderA_Var1)) + float(Base_Var1)) + 5.0f) + float(globalStreams_var.sa1_2));
}

void main()
{
    frag_main();
}

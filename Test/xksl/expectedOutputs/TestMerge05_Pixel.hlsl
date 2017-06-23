struct globalStreams
{
    int sbase1_0;
};

cbuffer Globals
{
    int Base_Var1;
};

static globalStreams globalStreams_var;

int shaderA_Compute1()
{
    return Base_Var1 + globalStreams_var.sbase1_0;
}

int shaderA_Compute2()
{
    return shaderA_Compute1();
}

int frag_main()
{
    return shaderA_Compute1() + shaderA_Compute2();
}

void main()
{
    frag_main();
}

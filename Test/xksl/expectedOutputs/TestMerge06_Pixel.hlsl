struct globalStreams
{
    int sbase1_0;
};

cbuffer Globals
{
    int Base_Var1;
};

static globalStreams globalStreams_var;

int Base_ComputeBase()
{
    return Base_Var1 + globalStreams_var.sbase1_0;
}

int shaderA_f1()
{
    return Base_ComputeBase();
}

int frag_main()
{
    return ((Base_ComputeBase() + Base_Var1) + Base_ComputeBase()) + shaderA_f1();
}

void main()
{
    frag_main();
}

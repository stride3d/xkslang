struct globalStreams
{
    float sbase1_0;
    int sbase2_1;
};

cbuffer Globals
{
    int Base_Var1;
    float Base_Var2;
};

static globalStreams globalStreams_var;

int frag_main()
{
    return int(((globalStreams_var.sbase1_0 + float(Base_Var1)) + Base_Var2) + 2.0f);
}

void main()
{
    frag_main();
}

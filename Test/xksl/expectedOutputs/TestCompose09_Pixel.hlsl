struct globalStreams
{
    int s1_0;
    int s1_1;
    int s1_2;
    int s1_3;
    int s1_4;
    int s1_5;
};

cbuffer Globals
{
    int o1S2C0_Comp_var1;
    int o1S2C0_o0S8C0_SubComp_var1;
    int o2S2C1_SubComp_var1;
};

static globalStreams globalStreams_var;

int o1S2C0_o0S8C0_SubComp_Compute()
{
    return o1S2C0_o0S8C0_SubComp_var1 + globalStreams_var.s1_4;
}

int o1S2C0_Comp_Compute()
{
    return (o1S2C0_Comp_var1 + globalStreams_var.s1_3) + o1S2C0_o0S8C0_SubComp_Compute();
}

int o2S2C1_SubComp_Compute()
{
    return o2S2C1_SubComp_var1 + globalStreams_var.s1_5;
}

int frag_main()
{
    return o1S2C0_Comp_Compute() + o2S2C1_SubComp_Compute();
}

void main()
{
    frag_main();
}

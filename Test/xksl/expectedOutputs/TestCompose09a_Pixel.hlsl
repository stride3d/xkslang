cbuffer Globals
{
    int o2S2C0_Comp_varA;
    int o2S2C0_o1S5C0_SubComp2_var2;
    int o2S2C0_o0S5C0_SubComp1_var1;
    int o3S2C0_Comp_varA;
    int o3S2C0_o1S5C0_SubComp2_var2;
    int o3S2C0_o0S5C0_SubComp1_var1;
    int o4S2C1_SubComp3_var3;
    int o5S2C1_SubComp4_var4;
};

int o4S2C1_SubComp3_Compute()
{
    return o4S2C1_SubComp3_var3;
}

int o5S2C1_SubComp4_Compute()
{
    return o5S2C1_SubComp4_var4;
}

int o2S2C0_o0S5C0_SubComp1_Compute()
{
    return o2S2C0_o0S5C0_SubComp1_var1;
}

int o2S2C0_o1S5C0_SubComp2_Compute()
{
    return o2S2C0_o1S5C0_SubComp2_var2;
}

int o2S2C0_Comp_Compute()
{
    float f = float(o2S2C0_Comp_varA);
    f += float(o2S2C0_o0S5C0_SubComp1_Compute());
    f += float(o2S2C0_o1S5C0_SubComp2_Compute());
    return int(f);
}

int o3S2C0_o0S5C0_SubComp1_Compute()
{
    return o3S2C0_o0S5C0_SubComp1_var1;
}

int o3S2C0_o1S5C0_SubComp2_Compute()
{
    return o3S2C0_o1S5C0_SubComp2_var2;
}

int o3S2C0_Comp_Compute()
{
    float f = float(o3S2C0_Comp_varA);
    f += float(o3S2C0_o0S5C0_SubComp1_Compute());
    f += float(o3S2C0_o1S5C0_SubComp2_Compute());
    return int(f);
}

void frag_main()
{
    float f = 0.0f;
    f += float(o4S2C1_SubComp3_Compute());
    f += float(o5S2C1_SubComp4_Compute());
    f += float(o2S2C0_Comp_Compute());
    f += float(o3S2C0_Comp_Compute());
}

void main()
{
    frag_main();
}

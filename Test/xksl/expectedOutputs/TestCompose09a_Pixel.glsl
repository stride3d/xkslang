#version 450

layout(std140) uniform Globals
{
    int o2S2C0_Comp_varA;
    int o2S2C0_o1S5C0_SubComp2_var2;
    int o2S2C0_o0S5C0_SubComp1_var1;
    int o3S2C0_Comp_varA;
    int o3S2C0_o1S5C0_SubComp2_var2;
    int o3S2C0_o0S5C0_SubComp1_var1;
    int o4S2C1_SubComp3_var3;
    int o5S2C1_SubComp4_var4;
} Globals_var;

int o4S2C1_SubComp3_Compute()
{
    return Globals_var.o4S2C1_SubComp3_var3;
}

int o5S2C1_SubComp4_Compute()
{
    return Globals_var.o5S2C1_SubComp4_var4;
}

int o2S2C0_o0S5C0_SubComp1_Compute()
{
    return Globals_var.o2S2C0_o0S5C0_SubComp1_var1;
}

int o2S2C0_o1S5C0_SubComp2_Compute()
{
    return Globals_var.o2S2C0_o1S5C0_SubComp2_var2;
}

int o2S2C0_Comp_Compute()
{
    float f = float(Globals_var.o2S2C0_Comp_varA);
    f += float(o2S2C0_o0S5C0_SubComp1_Compute());
    f += float(o2S2C0_o1S5C0_SubComp2_Compute());
    return int(f);
}

int o3S2C0_o0S5C0_SubComp1_Compute()
{
    return Globals_var.o3S2C0_o0S5C0_SubComp1_var1;
}

int o3S2C0_o1S5C0_SubComp2_Compute()
{
    return Globals_var.o3S2C0_o1S5C0_SubComp2_var2;
}

int o3S2C0_Comp_Compute()
{
    float f = float(Globals_var.o3S2C0_Comp_varA);
    f += float(o3S2C0_o0S5C0_SubComp1_Compute());
    f += float(o3S2C0_o1S5C0_SubComp2_Compute());
    return int(f);
}

void main()
{
    float f = 0.0;
    f += float(o4S2C1_SubComp3_Compute());
    f += float(o5S2C1_SubComp4_Compute());
    f += float(o2S2C0_Comp_Compute());
    f += float(o3S2C0_Comp_Compute());
}


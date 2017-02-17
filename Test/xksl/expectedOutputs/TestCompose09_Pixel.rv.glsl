#version 450

struct o1S2C0_Comp_streamBuffer
{
    int Comp_s1;
};

struct o1S2C0_o0S8C0_SubComp_streamBuffer
{
    int SubComp_s1;
};

struct o2S2C1_SubComp_streamBuffer
{
    int SubComp_s1;
};

layout(std140) uniform o1S2C0_Comp_globalCBuffer
{
    int var1;
} o1S2C0_Comp_globalCBuffer_var;

layout(std140) uniform o1S2C0_o0S8C0_SubComp_globalCBuffer
{
    int var1;
} o1S2C0_o0S8C0_SubComp_globalCBuffer_var;

layout(std140) uniform o2S2C1_SubComp_globalCBuffer
{
    int var1;
} o2S2C1_SubComp_globalCBuffer_var;

o1S2C0_Comp_streamBuffer o1S2C0_Comp_streamBuffer_var;
o1S2C0_o0S8C0_SubComp_streamBuffer o1S2C0_o0S8C0_SubComp_streamBuffer_var;
o2S2C1_SubComp_streamBuffer o2S2C1_SubComp_streamBuffer_var;

int o1S2C0_o0S8C0_SubComp_Compute()
{
    return o1S2C0_o0S8C0_SubComp_globalCBuffer_var.var1 + o1S2C0_o0S8C0_SubComp_streamBuffer_var.SubComp_s1;
}

int o1S2C0_Comp_Compute()
{
    return (o1S2C0_Comp_globalCBuffer_var.var1 + o1S2C0_Comp_streamBuffer_var.Comp_s1) + o1S2C0_o0S8C0_SubComp_Compute();
}

int o2S2C1_SubComp_Compute()
{
    return o2S2C1_SubComp_globalCBuffer_var.var1 + o2S2C1_SubComp_streamBuffer_var.SubComp_s1;
}

int main()
{
    return o1S2C0_Comp_Compute() + o2S2C1_SubComp_Compute();
}


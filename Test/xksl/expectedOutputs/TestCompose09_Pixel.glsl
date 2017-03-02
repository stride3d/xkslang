#version 450

struct globalStreams
{
    int s1_0;
    int s1_1;
    int s1_2;
    int s1_3;
    int s1_4;
    int s1_5;
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

globalStreams globalStreams_var;

int o1S2C0_o0S8C0_SubComp_Compute()
{
    return o1S2C0_o0S8C0_SubComp_globalCBuffer_var.var1 + globalStreams_var.s1_4;
}

int o1S2C0_Comp_Compute()
{
    return (o1S2C0_Comp_globalCBuffer_var.var1 + globalStreams_var.s1_3) + o1S2C0_o0S8C0_SubComp_Compute();
}

int o2S2C1_SubComp_Compute()
{
    return o2S2C1_SubComp_globalCBuffer_var.var1 + globalStreams_var.s1_5;
}

int main()
{
    return o1S2C0_Comp_Compute() + o2S2C1_SubComp_Compute();
}


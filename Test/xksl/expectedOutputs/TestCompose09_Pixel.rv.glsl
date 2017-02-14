#version 450

struct cS2C0_Comp_streamBuffer
{
    int Comp_s1;
};

struct cS2C0_cS8C0_SubComp_streamBuffer
{
    int SubComp_s1;
};

struct cS2C1_SubComp_streamBuffer
{
    int SubComp_s1;
};

layout(std140) uniform cS2C0_Comp_globalCBuffer
{
    int var1;
} cS2C0_Comp_globalCBuffer_var;

layout(std140) uniform cS2C0_cS8C0_SubComp_globalCBuffer
{
    int var1;
} cS2C0_cS8C0_SubComp_globalCBuffer_var;

layout(std140) uniform cS2C1_SubComp_globalCBuffer
{
    int var1;
} cS2C1_SubComp_globalCBuffer_var;

cS2C0_Comp_streamBuffer cS2C0_Comp_streamBuffer_var;
cS2C0_cS8C0_SubComp_streamBuffer cS2C0_cS8C0_SubComp_streamBuffer_var;
cS2C1_SubComp_streamBuffer cS2C1_SubComp_streamBuffer_var;

int cS2C0_cS8C0_SubComp_Compute()
{
    return cS2C0_cS8C0_SubComp_globalCBuffer_var.var1 + cS2C0_cS8C0_SubComp_streamBuffer_var.SubComp_s1;
}

int cS2C0_Comp_Compute()
{
    return (cS2C0_Comp_globalCBuffer_var.var1 + cS2C0_Comp_streamBuffer_var.Comp_s1) + cS2C0_cS8C0_SubComp_Compute();
}

int cS2C1_SubComp_Compute()
{
    return cS2C1_SubComp_globalCBuffer_var.var1 + cS2C1_SubComp_streamBuffer_var.SubComp_s1;
}

int main()
{
    return cS2C0_Comp_Compute() + cS2C1_SubComp_Compute();
}


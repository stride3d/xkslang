#version 450

struct SubComp_streamBuffer
{
    int SubComp_sa;
};

struct Comp_streamBuffer
{
    int Comp_sb;
};

struct compS7C0_SubComp_streamBuffer
{
    int SubComp_sa;
};

struct compS8C0_Comp_streamBuffer
{
    int Comp_sb;
};

struct compS71C0_SubComp_streamBuffer
{
    int SubComp_sa;
};

layout(std140) uniform SubComp_globalCBuffer
{
    int ca;
} SubComp_globalCBuffer_var;

layout(std140) uniform Comp_globalCBuffer
{
    int cb;
} Comp_globalCBuffer_var;

layout(std140) uniform compS7C0_SubComp_globalCBuffer
{
    int ca;
} compS7C0_SubComp_globalCBuffer_var;

layout(std140) uniform compS8C0_Comp_globalCBuffer
{
    int cb;
} compS8C0_Comp_globalCBuffer_var;

layout(std140) uniform compS71C0_SubComp_globalCBuffer
{
    int ca;
} compS71C0_SubComp_globalCBuffer_var;

SubComp_streamBuffer SubComp_streamBuffer_var;
Comp_streamBuffer Comp_streamBuffer_var;
compS7C0_SubComp_streamBuffer compS7C0_SubComp_streamBuffer_var;
compS8C0_Comp_streamBuffer compS8C0_Comp_streamBuffer_var;
compS71C0_SubComp_streamBuffer compS71C0_SubComp_streamBuffer_var;

int compS71C0_SubComp_Compute()
{
    return compS71C0_SubComp_globalCBuffer_var.ca + compS71C0_SubComp_streamBuffer_var.SubComp_sa;
}

int compS8C0_Comp_Compute()
{
    return (compS8C0_Comp_globalCBuffer_var.cb + compS8C0_Comp_streamBuffer_var.Comp_sb) + compS71C0_SubComp_Compute();
}

int main()
{
    return compS8C0_Comp_Compute();
}


int o1S2C0_o0S2C0_SubComp_Compute()
{
    return 2;
}

int o1S2C0_Comp_Compute()
{
    return 3 + o1S2C0_o0S2C0_SubComp_Compute();
}

int frag_main()
{
    return o1S2C0_Comp_Compute();
}

void main()
{
    frag_main();
}

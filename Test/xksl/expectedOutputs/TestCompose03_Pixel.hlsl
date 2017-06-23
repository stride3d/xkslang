cbuffer Globals
{
    float o0S5C0_CompA_varCA;
    float o1S5C1_CompB_varCB;
};

float o0S5C0_CompA_Compute()
{
    return o0S5C0_CompA_varCA;
}

float o1S5C1_CompB_Compute()
{
    return o1S5C1_CompB_varCB;
}

float frag_main()
{
    return o0S5C0_CompA_Compute() + o1S5C1_CompB_Compute();
}

void main()
{
    frag_main();
}

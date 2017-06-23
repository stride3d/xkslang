cbuffer Globals
{
    float o0S2C0_CompA_varCA;
    float o1S2C1_CompA_varCA;
    float o1S2C1_CompB_varCB;
};

float o0S2C0_CompA_Compute()
{
    return o0S2C0_CompA_varCA;
}

float o1S2C1_CompA_Compute()
{
    return o1S2C1_CompA_varCA;
}

float o1S2C1_CompB_Compute()
{
    return o1S2C1_CompB_varCB + o1S2C1_CompA_Compute();
}

float frag_main()
{
    return o0S2C0_CompA_Compute() + o1S2C1_CompB_Compute();
}

void main()
{
    frag_main();
}

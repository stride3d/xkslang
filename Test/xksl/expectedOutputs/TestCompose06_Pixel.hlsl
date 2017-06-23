cbuffer Globals
{
    float o0S2C0_CompA_varCA;
    float o0S2C0_CompB_varCB;
};

float o0S2C0_CompA_Compute()
{
    return o0S2C0_CompA_varCA;
}

float o0S2C0_CompB_Compute()
{
    return o0S2C0_CompB_varCB + o0S2C0_CompA_Compute();
}

float frag_main()
{
    return o0S2C0_CompB_Compute();
}

void main()
{
    frag_main();
}

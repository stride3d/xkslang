cbuffer Globals
{
    float o0S2C0_CompB_varCB;
};

float o0S2C0_CompB_Compute()
{
    return o0S2C0_CompB_varCB;
}

float frag_main()
{
    return o0S2C0_CompB_Compute();
}

void main()
{
    frag_main();
}

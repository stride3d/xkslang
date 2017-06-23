cbuffer Globals
{
    float o0S5C0_CompA_varCA;
};

float o0S5C0_CompA_Compute()
{
    return o0S5C0_CompA_varCA;
}

float frag_main()
{
    return o0S5C0_CompA_Compute();
}

void main()
{
    frag_main();
}

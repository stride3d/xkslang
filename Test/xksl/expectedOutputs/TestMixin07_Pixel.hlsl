void A_X()
{
}

void B_X()
{
    A_X();
    A_X();
}

void C_X()
{
    B_X();
    A_X();
}

void D_X()
{
    C_X();
    A_X();
}

void E_X()
{
    D_X();
    B_X();
}

void frag_main()
{
    E_X();
}

void main()
{
    frag_main();
}

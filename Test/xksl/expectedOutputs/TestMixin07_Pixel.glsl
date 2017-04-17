#version 450

void A_X()
{
}

void B_X()
{
    A_X();
}

void C_X()
{
    B_X();
}

void D_X()
{
    C_X();
}

void E_X()
{
    D_X();
}

void main()
{
    E_X();
}


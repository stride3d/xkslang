#version 410
#ifdef GL_ARB_shading_language_420pack
#extension GL_ARB_shading_language_420pack : require
#endif

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

void main()
{
    E_X();
}


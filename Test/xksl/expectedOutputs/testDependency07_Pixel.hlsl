int Utils_prout()
{
    return 5;
}

int UtilsBase_compute()
{
    return Utils_prout();
}

int Utils_compute()
{
    return UtilsBase_compute();
}

int frag_main()
{
    return Utils_compute();
}

void main()
{
    frag_main();
}

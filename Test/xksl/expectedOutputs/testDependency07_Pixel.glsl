#version 450

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

int main()
{
    return Utils_compute();
}


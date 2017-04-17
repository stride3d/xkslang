#version 450

int UtilsA_compute()
{
    return 1;
}

int ShaderMain_function()
{
    return UtilsA_compute();
}

int UtilsB_compute()
{
    return 2;
}

int UtilsC_compute()
{
    return 4;
}

int UtilsD_compute()
{
    return 5;
}

int UtilsE_compute()
{
    return 6;
}

int UtilsF_compute()
{
    return 7;
}

int UtilsG_compute()
{
    return 8;
}

int UtilsH_compute()
{
    return 9;
}

int UtilsI_compute()
{
    return 0;
}

int UtilsJ_compute()
{
    return 0;
}

int UtilsK_compute()
{
    return 10;
}

int UtilsM_compute()
{
    return 1;
}

int UtilsL_compute()
{
    return 2;
}

int UtilsN_compute()
{
    return 3;
}

int UtilsO_compute()
{
    return 3;
}

int main()
{
    int res = 0;
    res += (ShaderMain_function() + (5 * UtilsB_compute()));
    int _8;
    if (UtilsC_compute() == 4)
    {
        _8 = UtilsD_compute();
    }
    else
    {
        _8 = UtilsE_compute();
    }
    res += _8;
    if (UtilsF_compute() != int(0u))
    {
        res += UtilsG_compute();
    }
    else
    {
        res += UtilsH_compute();
    }
    int indexable[6] = int[](0, 1, 2, 3, 4, 5);
    res += indexable[UtilsI_compute()];
    for (int i = UtilsJ_compute(); i < UtilsK_compute(); i += UtilsL_compute())
    {
        res += UtilsM_compute();
    }
    int k = 0;
    for (;;)
    {
        int _61 = k;
        int _62 = _61 + 1;
        k = _62;
        int _63 = UtilsN_compute();
        if (_61 < _63)
        {
            res += UtilsO_compute();
            continue;
        }
        else
        {
            break;
        }
    }
    return res;
}


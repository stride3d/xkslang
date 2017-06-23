struct ShaderB_TypeDeclaredInB
{
    int aInt;
};

cbuffer Globals
{
    ShaderB_TypeDeclaredInB ShaderMain_var1;
};

int ShaderA_function()
{
    return 1;
}

int ShaderE_staticFunction()
{
    return 2;
}

int ShaderMain_function()
{
    int res = ShaderA_function();
    res += ShaderMain_var1.aInt;
    res += 5;
    res += 4;
    res += ShaderE_staticFunction();
    return res;
}

int frag_main()
{
    return ShaderMain_function();
}

void main()
{
    frag_main();
}

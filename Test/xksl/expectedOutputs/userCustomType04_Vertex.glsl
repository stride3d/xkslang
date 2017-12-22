#version 450

struct ShaderBase_CustomStruct
{
    vec3 DirectionWS;
};

struct ShaderMain_CustomStruct
{
    vec3 DirectionWS;
    vec3 Color;
};

ShaderMain_CustomStruct ShaderMain_GetMainStruct1()
{
    ShaderMain_CustomStruct c1;
    return c1;
}

ShaderMain_CustomStruct ShaderMain_GetMainStruct2()
{
    ShaderMain_CustomStruct c1;
    return c1;
}

void ShaderMain_aFunctionTakingMainStruct(ShaderMain_CustomStruct c)
{
}

ShaderBase_CustomStruct ShaderMain_GetBaseStruct1()
{
    ShaderBase_CustomStruct c1;
    return c1;
}

ShaderBase_CustomStruct ShaderMain_GetBaseStruct2()
{
    ShaderBase_CustomStruct c1;
    return c1;
}

void ShaderMain_aFunctionTakingBaseStruct(ShaderBase_CustomStruct c)
{
}

ShaderBase_CustomStruct ShaderMain_ConvertStreamsToShaderBaseStreams(ShaderMain_CustomStruct c)
{
    ShaderBase_CustomStruct c1 = ShaderBase_CustomStruct(c.DirectionWS);
    return c1;
}

void main()
{
    ShaderMain_CustomStruct c1 = ShaderMain_GetMainStruct1();
    ShaderMain_CustomStruct c2 = ShaderMain_GetMainStruct2();
    c1 = c2;
    ShaderMain_CustomStruct param = c1;
    ShaderMain_aFunctionTakingMainStruct(param);
    ShaderBase_CustomStruct c3 = ShaderMain_GetBaseStruct1();
    ShaderBase_CustomStruct c4 = ShaderMain_GetBaseStruct2();
    c3 = c4;
    ShaderBase_CustomStruct param_1 = c3;
    ShaderMain_aFunctionTakingBaseStruct(param_1);
    ShaderMain_CustomStruct param_2 = c1;
    c3 = ShaderMain_ConvertStreamsToShaderBaseStreams(param_2);
}


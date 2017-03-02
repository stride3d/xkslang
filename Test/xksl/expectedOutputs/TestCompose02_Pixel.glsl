#version 450

layout(std140) uniform o0S5C0_ShaderComp_globalCBuffer
{
    float varC;
} o0S5C0_ShaderComp_globalCBuffer_var;

layout(std140) uniform o1S5C1_ShaderComp_globalCBuffer
{
    float varC;
} o1S5C1_ShaderComp_globalCBuffer_var;

float ShaderA_Compute()
{
    return 5.0;
}

float o0S5C0_ShaderComp_Compute()
{
    return o0S5C0_ShaderComp_globalCBuffer_var.varC;
}

float o1S5C1_ShaderComp_Compute()
{
    return o1S5C1_ShaderComp_globalCBuffer_var.varC;
}

float main()
{
    float f = ShaderA_Compute();
    return (f + o0S5C0_ShaderComp_Compute()) + o1S5C1_ShaderComp_Compute();
}


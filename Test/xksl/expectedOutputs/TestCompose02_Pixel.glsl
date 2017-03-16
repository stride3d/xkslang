#version 450

layout(std140) uniform globalCbuffer
{
    float varC;
    float varC_1;
} globalCbuffer_var;

float ShaderA_Compute()
{
    return 5.0;
}

float o0S5C0_ShaderComp_Compute()
{
    return globalCbuffer_var.varC;
}

float o1S5C1_ShaderComp_Compute()
{
    return globalCbuffer_var.varC_1;
}

float main()
{
    float f = ShaderA_Compute();
    return (f + o0S5C0_ShaderComp_Compute()) + o1S5C1_ShaderComp_Compute();
}


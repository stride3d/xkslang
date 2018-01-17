#version 450

float ShaderMain_functionWithDefaultParam(bool aParam)
{
    return 0.0;
}

void main()
{
    bool param = true;
    float f1 = ShaderMain_functionWithDefaultParam(param);
    bool param_1 = false;
    float f2 = ShaderMain_functionWithDefaultParam(param_1);
}


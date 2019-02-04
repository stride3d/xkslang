#version 410
#ifdef GL_ARB_shading_language_420pack
#extension GL_ARB_shading_language_420pack : require
#endif

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
    gl_Position.z = 2.0 * gl_Position.z - gl_Position.w;
    gl_Position.y = -gl_Position.y;
}


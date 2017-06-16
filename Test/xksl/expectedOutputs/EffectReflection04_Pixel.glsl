#version 450

struct ShaderMain_StructType
{
    float aFloat;
    int aInt;
    uint aBool;
};

struct PS_STREAMS
{
    vec4 ColorTarget_id0;
};

layout(std140) uniform Globals
{
    ShaderMain_StructType ShaderMain_var1;
    ShaderMain_StructType ShaderMain_var2;
} Globals_var;

layout(location = 0) out vec4 PS_OUT_ColorTarget;

void main()
{
    PS_STREAMS _streams = PS_STREAMS(vec4(0.0));
    _streams.ColorTarget_id0 = vec4(Globals_var.ShaderMain_var1.aFloat, Globals_var.ShaderMain_var2.aFloat, 0.0, 0.0);
    PS_OUT_ColorTarget = _streams.ColorTarget_id0;
}


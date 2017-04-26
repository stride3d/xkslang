#version 450

struct ShaderMain_Toto
{
    vec2 toto1;
    vec4 toto2;
    int toto3[2];
};

struct VS_STREAMS
{
    double aDouble_id0;
    int aBool_id1;
    vec4 aF4Tab_id2[4];
    vec2 aF2_id3;
    vec2 aF2b_id4;
    float aFloat_id5;
    ShaderMain_Toto aToto1_id6;
    ShaderMain_Toto aTotoTab_id7[2];
    mat4 aMat44_id8;
};

layout(location = 0) in double VS_IN_aDouble;
layout(location = 1) in int VS_IN_aBool;
layout(location = 2) in vec4 VS_IN_aF4Tab[4];
layout(location = 3) in vec2 VS_IN_aF2b;
layout(location = 4) in float VS_IN_aFloat;
layout(location = 5) in ShaderMain_Toto VS_IN_aToto1;
layout(location = 6) in ShaderMain_Toto VS_IN_aTotoTab[2];
layout(location = 7) in mat4 VS_IN_aMat44;
layout(location = 0) out double VS_OUT_aDouble;
layout(location = 1) out int VS_OUT_aBool;
layout(location = 2) out vec4 VS_OUT_aF4Tab[4];
layout(location = 3) out vec2 VS_OUT_aF2;
layout(location = 4) out ShaderMain_Toto VS_OUT_aToto1;
layout(location = 5) out ShaderMain_Toto VS_OUT_aTotoTab[2];
layout(location = 6) out mat4 VS_OUT_aMat44;

void main()
{
    VS_STREAMS _streams = VS_STREAMS(0.0lf, 0, vec4[](vec4(0.0), vec4(0.0), vec4(0.0), vec4(0.0)), vec2(0.0), vec2(0.0), 0.0, ShaderMain_Toto(vec2(0.0), vec4(0.0), int[](0, 0)), ShaderMain_Toto[](ShaderMain_Toto(vec2(0.0), vec4(0.0), int[](0, 0)), ShaderMain_Toto(vec2(0.0), vec4(0.0), int[](0, 0))), mat4(vec4(0.0), vec4(0.0), vec4(0.0), vec4(0.0)));
    _streams.aDouble_id0 = VS_IN_aDouble;
    _streams.aBool_id1 = VS_IN_aBool;
    _streams.aF4Tab_id2 = VS_IN_aF4Tab;
    _streams.aF2b_id4 = VS_IN_aF2b;
    _streams.aFloat_id5 = VS_IN_aFloat;
    _streams.aToto1_id6 = VS_IN_aToto1;
    _streams.aTotoTab_id7 = VS_IN_aTotoTab;
    _streams.aMat44_id8 = VS_IN_aMat44;
    _streams.aF2_id3 = vec2(0.0, 1.0 + _streams.aFloat_id5) + _streams.aF2b_id4;
    VS_OUT_aDouble = _streams.aDouble_id0;
    VS_OUT_aBool = _streams.aBool_id1;
    VS_OUT_aF4Tab = _streams.aF4Tab_id2;
    VS_OUT_aF2 = _streams.aF2_id3;
    VS_OUT_aToto1 = _streams.aToto1_id6;
    VS_OUT_aTotoTab = _streams.aTotoTab_id7;
    VS_OUT_aMat44 = _streams.aMat44_id8;
}


#version 450

struct ShaderMain_Toto
{
    vec2 toto1;
    vec4 toto2;
    int toto3[2];
};

struct PS_STREAMS
{
    double aDouble_id0;
    int aBool_id1;
    vec4 aF4Tab_id2[4];
    vec2 aF2_id3;
    vec4 aF4_id4;
    ShaderMain_Toto aToto1_id5;
    ShaderMain_Toto aTotoTab_id6[2];
    mat4 aMat44_id7;
};

layout(location = 0) in double PS_IN_aDouble;
layout(location = 1) in int PS_IN_aBool;
layout(location = 2) in vec4 PS_IN_aF4Tab[4];
layout(location = 3) in vec2 PS_IN_aF2;
layout(location = 4) in ShaderMain_Toto PS_IN_aToto1;
layout(location = 5) in ShaderMain_Toto PS_IN_aTotoTab[2];
layout(location = 6) in mat4 PS_IN_aMat44;
layout(location = 0) out vec4 PS_OUT_aF4;

void main()
{
    PS_STREAMS _streams = PS_STREAMS(0.0lf, 0, vec4[](vec4(0.0), vec4(0.0), vec4(0.0), vec4(0.0)), vec2(0.0), vec4(0.0), ShaderMain_Toto(vec2(0.0), vec4(0.0), int[](0, 0)), ShaderMain_Toto[](ShaderMain_Toto(vec2(0.0), vec4(0.0), int[](0, 0)), ShaderMain_Toto(vec2(0.0), vec4(0.0), int[](0, 0))), mat4(vec4(0.0), vec4(0.0), vec4(0.0), vec4(0.0)));
    _streams.aDouble_id0 = PS_IN_aDouble;
    _streams.aBool_id1 = PS_IN_aBool;
    _streams.aF4Tab_id2 = PS_IN_aF4Tab;
    _streams.aF2_id3 = PS_IN_aF2;
    _streams.aToto1_id5 = PS_IN_aToto1;
    _streams.aTotoTab_id6 = PS_IN_aTotoTab;
    _streams.aMat44_id7 = PS_IN_aMat44;
    double dd = 0.0lf;
    double d = _streams.aDouble_id0;
    vec4 aaa = _streams.aF4Tab_id2[0];
    ShaderMain_Toto toto = _streams.aToto1_id5;
    ShaderMain_Toto toto2 = _streams.aTotoTab_id6[1];
    mat4 aMat = _streams.aMat44_id7;
    bool b = _streams.aBool_id1 != int(0u);
    _streams.aF4_id4 = vec4(_streams.aF2_id3, 0.0, 1.0);
    PS_OUT_aF4 = _streams.aF4_id4;
}


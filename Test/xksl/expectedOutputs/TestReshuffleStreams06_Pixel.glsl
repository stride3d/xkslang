#version 450

struct ShaderMain_Toto
{
    vec2 toto1;
    vec4 toto2;
    int toto3[2];
};

struct VS_OUT
{
    double aDouble_id0;
    int aBool_id1;
    vec4 aF4Tab_id2[4];
    vec2 aF2_id3;
    ShaderMain_Toto aToto1_id4;
    ShaderMain_Toto aTotoTab_id5[2];
    mat4 aMat44_id6;
    mat4x1 aMat44Tab_id7[2];
};

struct PS_OUT
{
    vec4 aF4_id0;
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
    mat4x1 aMat44Tab_id8[2];
};

PS_OUT main(VS_OUT __input__)
{
    PS_STREAMS _streams = PS_STREAMS(0.0lf, 0, vec4[](vec4(0.0), vec4(0.0), vec4(0.0), vec4(0.0)), vec2(0.0), vec4(0.0), ShaderMain_Toto(vec2(0.0), vec4(0.0), int[](0, 0)), ShaderMain_Toto[](ShaderMain_Toto(vec2(0.0), vec4(0.0), int[](0, 0)), ShaderMain_Toto(vec2(0.0), vec4(0.0), int[](0, 0))), mat4(vec4(0.0), vec4(0.0), vec4(0.0), vec4(0.0)), mat4x1[](mat4x1(0.0, 0.0, 0.0, 0.0), mat4x1(0.0, 0.0, 0.0, 0.0)));
    _streams.aDouble_id0 = __input__.aDouble_id0;
    _streams.aBool_id1 = __input__.aBool_id1;
    _streams.aF4Tab_id2 = __input__.aF4Tab_id2;
    _streams.aF2_id3 = __input__.aF2_id3;
    _streams.aToto1_id5 = __input__.aToto1_id4;
    _streams.aTotoTab_id6 = __input__.aTotoTab_id5;
    _streams.aMat44_id7 = __input__.aMat44_id6;
    _streams.aMat44Tab_id8 = __input__.aMat44Tab_id7;
    double dd = 0.0lf;
    double d = _streams.aDouble_id0;
    vec4 aaa = _streams.aF4Tab_id2[0];
    ShaderMain_Toto toto = _streams.aToto1_id5;
    ShaderMain_Toto toto2 = _streams.aTotoTab_id6[1];
    mat4 aMat = _streams.aMat44_id7;
    bool b = _streams.aBool_id1 != int(0u);
    _streams.aF4_id4 = vec4(_streams.aF2_id3, 0.0, 1.0);
    mat4x1 amat2 = _streams.aMat44Tab_id8[2];
    PS_OUT __output__ = PS_OUT(vec4(0.0));
    __output__.aF4_id0 = _streams.aF4_id4;
    return __output__;
}


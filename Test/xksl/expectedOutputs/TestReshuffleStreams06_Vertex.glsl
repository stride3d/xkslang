#version 450

struct ShaderMain_Toto
{
    vec2 toto1;
    vec4 toto2;
    int toto3[2];
};

struct VS_IN
{
    double aDouble_id0;
    int aBool_id1;
    vec4 aF4Tab_id2[4];
    vec2 aF2b_id3;
    float aFloat_id4;
    ShaderMain_Toto aToto1_id5;
    ShaderMain_Toto aTotoTab_id6[2];
    mat4 aMat44_id7;
    mat4x1 aMat44Tab_id8[2];
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
    mat4x1 aMat44Tab_id9[2];
};

VS_OUT main(VS_IN __input__)
{
    VS_STREAMS _streams = VS_STREAMS(0.0lf, 0, vec4[](vec4(0.0), vec4(0.0), vec4(0.0), vec4(0.0)), vec2(0.0), vec2(0.0), 0.0, ShaderMain_Toto(vec2(0.0), vec4(0.0), int[](0, 0)), ShaderMain_Toto[](ShaderMain_Toto(vec2(0.0), vec4(0.0), int[](0, 0)), ShaderMain_Toto(vec2(0.0), vec4(0.0), int[](0, 0))), mat4(vec4(0.0), vec4(0.0), vec4(0.0), vec4(0.0)), mat4x1[](mat4x1(0.0, 0.0, 0.0, 0.0), mat4x1(0.0, 0.0, 0.0, 0.0)));
    _streams.aDouble_id0 = __input__.aDouble_id0;
    _streams.aBool_id1 = __input__.aBool_id1;
    _streams.aF4Tab_id2 = __input__.aF4Tab_id2;
    _streams.aF2b_id4 = __input__.aF2b_id3;
    _streams.aFloat_id5 = __input__.aFloat_id4;
    _streams.aToto1_id6 = __input__.aToto1_id5;
    _streams.aTotoTab_id7 = __input__.aTotoTab_id6;
    _streams.aMat44_id8 = __input__.aMat44_id7;
    _streams.aMat44Tab_id9 = __input__.aMat44Tab_id8;
    _streams.aF2_id3 = vec2(0.0, 1.0 + _streams.aFloat_id5) + _streams.aF2b_id4;
    VS_OUT __output__ = VS_OUT(0.0lf, 0, vec4[](vec4(0.0), vec4(0.0), vec4(0.0), vec4(0.0)), vec2(0.0), ShaderMain_Toto(vec2(0.0), vec4(0.0), int[](0, 0)), ShaderMain_Toto[](ShaderMain_Toto(vec2(0.0), vec4(0.0), int[](0, 0)), ShaderMain_Toto(vec2(0.0), vec4(0.0), int[](0, 0))), mat4(vec4(0.0), vec4(0.0), vec4(0.0), vec4(0.0)), mat4x1[](mat4x1(0.0, 0.0, 0.0, 0.0), mat4x1(0.0, 0.0, 0.0, 0.0)));
    __output__.aDouble_id0 = _streams.aDouble_id0;
    __output__.aBool_id1 = _streams.aBool_id1;
    __output__.aF4Tab_id2 = _streams.aF4Tab_id2;
    __output__.aF2_id3 = _streams.aF2_id3;
    __output__.aToto1_id4 = _streams.aToto1_id6;
    __output__.aTotoTab_id5 = _streams.aTotoTab_id7;
    __output__.aMat44_id6 = _streams.aMat44_id8;
    __output__.aMat44Tab_id7 = _streams.aMat44Tab_id9;
    return __output__;
}


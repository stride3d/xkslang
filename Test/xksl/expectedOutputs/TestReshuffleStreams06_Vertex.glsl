#version 410
#ifdef GL_ARB_shading_language_420pack
#extension GL_ARB_shading_language_420pack : require
#endif

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

in double VS_IN_ADOUBLE;
in int VS_IN_ABOOL;
in vec4 VS_IN_AF4Tab[4];
in vec2 VS_IN_AF2b;
in float VS_IN_AF;
in ShaderMain_Toto VS_IN_ATOTO;
in ShaderMain_Toto VS_IN_ATOTOTAB[2];
in mat4 VS_IN_AMAT44;
out double VS_OUT_aDouble;
out int VS_OUT_aBool;
out vec4 VS_OUT_aF4Tab[4];
out vec2 VS_OUT_aF2;
out ShaderMain_Toto VS_OUT_aToto1;
out ShaderMain_Toto VS_OUT_aTotoTab[2];
out mat4 VS_OUT_aMat44;

void main()
{
    VS_STREAMS _streams = VS_STREAMS(0.0lf, 0, vec4[](vec4(0.0), vec4(0.0), vec4(0.0), vec4(0.0)), vec2(0.0), vec2(0.0), 0.0, ShaderMain_Toto(vec2(0.0), vec4(0.0), int[](0, 0)), ShaderMain_Toto[](ShaderMain_Toto(vec2(0.0), vec4(0.0), int[](0, 0)), ShaderMain_Toto(vec2(0.0), vec4(0.0), int[](0, 0))), mat4(vec4(0.0), vec4(0.0), vec4(0.0), vec4(0.0)));
    _streams.aDouble_id0 = VS_IN_ADOUBLE;
    _streams.aBool_id1 = VS_IN_ABOOL;
    _streams.aF4Tab_id2 = VS_IN_AF4Tab;
    _streams.aF2b_id4 = VS_IN_AF2b;
    _streams.aFloat_id5 = VS_IN_AF;
    _streams.aToto1_id6 = VS_IN_ATOTO;
    _streams.aTotoTab_id7 = VS_IN_ATOTOTAB;
    _streams.aMat44_id8 = VS_IN_AMAT44;
    _streams.aF2_id3 = vec2(0.0, 1.0 + _streams.aFloat_id5) + _streams.aF2b_id4;
    VS_OUT_aDouble = _streams.aDouble_id0;
    VS_OUT_aBool = _streams.aBool_id1;
    VS_OUT_aF4Tab = _streams.aF4Tab_id2;
    VS_OUT_aF2 = _streams.aF2_id3;
    VS_OUT_aToto1 = _streams.aToto1_id6;
    VS_OUT_aTotoTab = _streams.aTotoTab_id7;
    VS_OUT_aMat44 = _streams.aMat44_id8;
    gl_Position.z = 2.0 * gl_Position.z - gl_Position.w;
    gl_Position.y = -gl_Position.y;
}


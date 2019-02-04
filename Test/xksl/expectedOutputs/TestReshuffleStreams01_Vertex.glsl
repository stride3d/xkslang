#version 410
#ifdef GL_ARB_shading_language_420pack
#extension GL_ARB_shading_language_420pack : require
#endif

struct VS_STREAMS
{
    vec2 Position_id0;
    vec2 VertexPosition_id1;
    float aFloat_id2;
};

in vec2 VS_IN_V_POSITION;
in float VS_IN_V_AFLOAT;
out vec2 VS_OUT_Position;

void main()
{
    VS_STREAMS _streams = VS_STREAMS(vec2(0.0), vec2(0.0), 0.0);
    _streams.VertexPosition_id1 = VS_IN_V_POSITION;
    _streams.aFloat_id2 = VS_IN_V_AFLOAT;
    _streams.Position_id0 = vec2(0.0, 1.0 + _streams.aFloat_id2) + _streams.VertexPosition_id1;
    VS_OUT_Position = _streams.Position_id0;
    gl_Position.z = 2.0 * gl_Position.z - gl_Position.w;
    gl_Position.y = -gl_Position.y;
}


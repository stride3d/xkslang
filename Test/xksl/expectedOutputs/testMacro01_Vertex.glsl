#version 410
#ifdef GL_ARB_shading_language_420pack
#extension GL_ARB_shading_language_420pack : require
#endif

struct VS_STREAMS
{
    int tabStream01_id0[2];
};

out int VS_OUT_tabStream01[2];

void main()
{
    VS_STREAMS _streams = VS_STREAMS(int[](0, 0));
    for (int i = 0; i < 2; i++)
    {
        _streams.tabStream01_id0[i] = i;
    }
    VS_OUT_tabStream01 = _streams.tabStream01_id0;
    gl_Position.z = 2.0 * gl_Position.z - gl_Position.w;
    gl_Position.y = -gl_Position.y;
}


#version 410
#ifdef GL_ARB_shading_language_420pack
#extension GL_ARB_shading_language_420pack : require
#endif

struct PS_STREAMS
{
    int tabStream01_id0[2];
    int tabStream02_id1[2];
};

in int PS_IN_TABSTREAM01[2];

void main()
{
    PS_STREAMS _streams = PS_STREAMS(int[](0, 0), int[](0, 0));
    _streams.tabStream01_id0 = PS_IN_TABSTREAM01;
    for (int i = 0; i < 2; i++)
    {
        _streams.tabStream02_id1[i] = _streams.tabStream01_id0[i];
    }
}


#version 450

struct PS_STREAMS
{
    int tabStream01_id0[2];
    int tabStream02_id1[2];
};

layout(location = 0) in int PS_IN_tabStream01[2];
layout(location = 0) out int PS_OUT_tabStream02[2];

void main()
{
    PS_STREAMS _streams = PS_STREAMS(int[](0, 0), int[](0, 0));
    _streams.tabStream01_id0 = PS_IN_tabStream01;
    for (int i = 0; i < 2; i++)
    {
        _streams.tabStream02_id1[i] = _streams.tabStream01_id0[i];
    }
    PS_OUT_tabStream02 = _streams.tabStream02_id1;
}


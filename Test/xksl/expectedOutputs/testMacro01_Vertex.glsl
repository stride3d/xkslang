#version 450

struct VS_STREAMS
{
    int tabStream01_id0[2];
};

layout(location = 0) out int VS_OUT_tabStream01[2];

void main()
{
    VS_STREAMS _streams = VS_STREAMS(int[](0, 0));
    for (int i = 0; i < 2; i++)
    {
        _streams.tabStream01_id0[i] = i;
    }
    VS_OUT_tabStream01 = _streams.tabStream01_id0;
}


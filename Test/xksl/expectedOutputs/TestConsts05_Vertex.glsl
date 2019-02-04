#version 410
#ifdef GL_ARB_shading_language_420pack
#extension GL_ARB_shading_language_420pack : require
#endif

struct VS_STREAMS
{
    float SHBaseValues_id0[4];
};

out float VS_OUT_SHBaseValues[4];

void main()
{
    VS_STREAMS _streams = VS_STREAMS(float[](0.0, 0.0, 0.0, 0.0));
    int k = 0;
    int _5 = k;
    k = _5 + 1;
    _streams.SHBaseValues_id0[0] = float(_5);
    if (true)
    {
        int _10 = k;
        k = _10 + 1;
        _streams.SHBaseValues_id0[1] = float(_10);
        int _14 = k;
        k = _14 + 1;
        _streams.SHBaseValues_id0[2] = float(_14);
        int _18 = k;
        k = _18 + 1;
        _streams.SHBaseValues_id0[3] = float(_18);
        if (false)
        {
            int _23 = k;
            k = _23 + 1;
            _streams.SHBaseValues_id0[4] = float(_23);
            int _27 = k;
            k = _27 + 1;
            _streams.SHBaseValues_id0[5] = float(_27);
            int _31 = k;
            k = _31 + 1;
            _streams.SHBaseValues_id0[6] = float(_31);
            int _35 = k;
            k = _35 + 1;
            _streams.SHBaseValues_id0[7] = float(_35);
            int _39 = k;
            k = _39 + 1;
            _streams.SHBaseValues_id0[8] = float(_39);
            if (false)
            {
                int _44 = k;
                k = _44 + 1;
                _streams.SHBaseValues_id0[9] = float(_44);
                int _48 = k;
                k = _48 + 1;
                _streams.SHBaseValues_id0[10] = float(_48);
                int _52 = k;
                k = _52 + 1;
                _streams.SHBaseValues_id0[11] = float(_52);
                int _56 = k;
                k = _56 + 1;
                _streams.SHBaseValues_id0[12] = float(_56);
                int _60 = k;
                k = _60 + 1;
                _streams.SHBaseValues_id0[13] = float(_60);
                int _64 = k;
                k = _64 + 1;
                _streams.SHBaseValues_id0[14] = float(_64);
                int _68 = k;
                k = _68 + 1;
                _streams.SHBaseValues_id0[15] = float(_68);
                if (false)
                {
                    int _73 = k;
                    k = _73 + 1;
                    _streams.SHBaseValues_id0[16] = float(_73);
                    int _77 = k;
                    k = _77 + 1;
                    _streams.SHBaseValues_id0[17] = float(_77);
                    int _81 = k;
                    k = _81 + 1;
                    _streams.SHBaseValues_id0[18] = float(_81);
                    int _85 = k;
                    k = _85 + 1;
                    _streams.SHBaseValues_id0[19] = float(_85);
                    int _89 = k;
                    k = _89 + 1;
                    _streams.SHBaseValues_id0[20] = float(_89);
                    int _93 = k;
                    k = _93 + 1;
                    _streams.SHBaseValues_id0[21] = float(_93);
                    int _97 = k;
                    k = _97 + 1;
                    _streams.SHBaseValues_id0[22] = float(_97);
                    int _101 = k;
                    k = _101 + 1;
                    _streams.SHBaseValues_id0[23] = float(_101);
                    int _105 = k;
                    k = _105 + 1;
                    _streams.SHBaseValues_id0[24] = float(_105);
                }
            }
        }
    }
    VS_OUT_SHBaseValues = _streams.SHBaseValues_id0;
    gl_Position.z = 2.0 * gl_Position.z - gl_Position.w;
    gl_Position.y = -gl_Position.y;
}


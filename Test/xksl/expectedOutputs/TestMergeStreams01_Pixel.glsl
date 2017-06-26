#version 450

struct PS_STREAMS
{
    int aStream2_id0;
    vec4 aStream3_id1;
    vec4 aStreamBis_id2;
    int aStream1_id3;
    int aStream1_id4;
};

layout(location = 0) in int PS_IN_aStream2;
layout(location = 1) in vec4 PS_IN_aStream3;
layout(location = 0) out vec4 PS_OUT_aStreamBis;
layout(location = 1) out int PS_OUT_aStream1;
layout(location = 2) out int PS_OUT_aStream1_1;

int o0S5C0_Color_Compute(inout PS_STREAMS _streams, int i)
{
    _streams.aStreamBis_id2 = vec4(float(i));
    _streams.aStream1_id3 = i + _streams.aStream2_id0;
    return ivec4(_streams.aStream3_id1 + vec4(float(i))).x;
}

int o1S5C0_Color_Compute(inout PS_STREAMS _streams, int i)
{
    _streams.aStreamBis_id2 = vec4(float(i));
    _streams.aStream1_id4 = i + _streams.aStream2_id0;
    return ivec4(_streams.aStream3_id1 + vec4(float(i))).x;
}

void main()
{
    PS_STREAMS _streams = PS_STREAMS(0, vec4(0.0), vec4(0.0), 0, 0);
    _streams.aStream2_id0 = PS_IN_aStream2;
    _streams.aStream3_id1 = PS_IN_aStream3;
    int res = 0;
    int param = 1;
    int _69 = o0S5C0_Color_Compute(_streams, param);
    res += _69;
    param = 1;
    int _72 = o1S5C0_Color_Compute(_streams, param);
    res += _72;
    _streams.aStreamBis_id2 += vec4(float(res));
    int i = ivec4(_streams.aStreamBis_id2).x;
    PS_OUT_aStreamBis = _streams.aStreamBis_id2;
    PS_OUT_aStream1 = _streams.aStream1_id3;
    PS_OUT_aStream1_1 = _streams.aStream1_id4;
}


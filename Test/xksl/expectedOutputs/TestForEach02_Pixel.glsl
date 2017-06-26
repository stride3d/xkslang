#version 450

struct PS_STREAMS
{
    int svar_id0;
};

layout(location = 0) out int PS_OUT_svar;

void main()
{
    PS_STREAMS _streams = PS_STREAMS(0);
    int res = 0;
    res++;
    _streams.svar_id0 = res;
    res++;
    _streams.svar_id0 = res;
    int i = res;
    PS_OUT_svar = _streams.svar_id0;
}


#version 410
#ifdef GL_ARB_shading_language_420pack
#extension GL_ARB_shading_language_420pack : require
#endif

struct PS_STREAMS
{
    int streamI_id0;
};

out int PS_OUT_streamI;

void main()
{
    PS_STREAMS _streams = PS_STREAMS(0);
    _streams.streamI_id0 = 0;
    PS_OUT_streamI = _streams.streamI_id0;
}


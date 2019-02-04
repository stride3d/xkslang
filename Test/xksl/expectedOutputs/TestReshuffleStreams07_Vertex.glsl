#version 410
#ifdef GL_ARB_shading_language_420pack
#extension GL_ARB_shading_language_420pack : require
#endif

struct VS_STREAMS
{
    mat2x3 aMat23_id0;
};

in mat2x3 VS_IN_AMAT23;
out mat2x3 VS_OUT_aMat23;

void main()
{
    VS_STREAMS _streams = VS_STREAMS(mat2x3(vec3(0.0), vec3(0.0)));
    _streams.aMat23_id0 = VS_IN_AMAT23;
    VS_OUT_aMat23 = _streams.aMat23_id0;
    gl_Position.z = 2.0 * gl_Position.z - gl_Position.w;
    gl_Position.y = -gl_Position.y;
}


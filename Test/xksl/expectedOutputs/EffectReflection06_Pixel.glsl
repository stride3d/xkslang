#version 410
#ifdef GL_ARB_shading_language_420pack
#extension GL_ARB_shading_language_420pack : require
#endif

struct PS_STREAMS
{
    vec4 aStream_id0;
    vec4 ColorTarget_id1;
};

uniform sampler2D SPIRV_Cross_CombinedShaderMain_Texture0ShaderMain_Sampler0;

in vec4 PS_IN_ASTREAM;
out vec4 PS_OUT_ColorTarget;

void main()
{
    PS_STREAMS _streams = PS_STREAMS(vec4(0.0), vec4(0.0));
    _streams.aStream_id0 = PS_IN_ASTREAM;
    _streams.ColorTarget_id1 = texture(SPIRV_Cross_CombinedShaderMain_Texture0ShaderMain_Sampler0, vec2(0.5)) + _streams.aStream_id0;
    PS_OUT_ColorTarget = _streams.ColorTarget_id1;
}


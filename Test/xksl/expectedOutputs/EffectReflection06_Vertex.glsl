#version 410
#ifdef GL_ARB_shading_language_420pack
#extension GL_ARB_shading_language_420pack : require
#endif

struct VS_STREAMS
{
    vec4 aStream_id0;
};

uniform sampler2D SPIRV_Cross_CombinedShaderMain_Texture0ShaderMain_Sampler0;

out vec4 VS_OUT_aStream;

void main()
{
    VS_STREAMS _streams = VS_STREAMS(vec4(0.0));
    _streams.aStream_id0 = textureLod(SPIRV_Cross_CombinedShaderMain_Texture0ShaderMain_Sampler0, vec2(0.4000000059604644775390625), 0.0);
    VS_OUT_aStream = _streams.aStream_id0;
    gl_Position.z = 2.0 * gl_Position.z - gl_Position.w;
    gl_Position.y = -gl_Position.y;
}


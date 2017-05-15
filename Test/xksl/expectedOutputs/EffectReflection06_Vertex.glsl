#version 450

struct VS_STREAMS
{
    vec4 aStream_id0;
};

uniform sampler2D SPIRV_Cross_CombinedShaderMain_Texture0ShaderMain_Sampler0;

layout(location = 0) out vec4 VS_OUT_aStream;

void main()
{
    VS_STREAMS _streams = VS_STREAMS(vec4(0.0));
    _streams.aStream_id0 = textureLod(SPIRV_Cross_CombinedShaderMain_Texture0ShaderMain_Sampler0, vec2(0.4000000059604644775390625), 0.0);
    VS_OUT_aStream = _streams.aStream_id0;
}


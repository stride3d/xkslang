#version 450

struct PS_STREAMS
{
    vec4 aStream_id0;
    vec4 ColorTarget_id1;
};

uniform sampler2D SPIRV_Cross_CombinedShaderMain_Texture0ShaderMain_Sampler0;

layout(location = 0) in vec4 PS_IN_aStream;
layout(location = 0) out vec4 PS_OUT_ColorTarget;

void main()
{
    PS_STREAMS _streams = PS_STREAMS(vec4(0.0), vec4(0.0));
    _streams.aStream_id0 = PS_IN_aStream;
    _streams.ColorTarget_id1 = texture(SPIRV_Cross_CombinedShaderMain_Texture0ShaderMain_Sampler0, vec2(0.5)) + _streams.aStream_id0;
    PS_OUT_ColorTarget = _streams.ColorTarget_id1;
}


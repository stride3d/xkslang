#version 410
#ifdef GL_ARB_shading_language_420pack
#extension GL_ARB_shading_language_420pack : require
#endif

struct PS_STREAMS
{
    vec4 Color_id0;
    vec4 ColorAdd_id1;
    float Swizzle_id2;
    vec4 ShadingPosition_id3;
    vec4 ColorTarget_id4;
    vec2 TexCoord_id5;
};

uniform sampler2D SPIRV_Cross_CombinedTexturing_Texture0Texturing_Sampler;

in vec4 PS_IN_COLOR;
in vec4 PS_IN_COLOR1;
in float PS_IN_BATCH_SWIZZLE;
in vec4 PS_IN_SV_Position;
in vec2 PS_IN_TEXCOORD0;
out vec4 PS_OUT_ColorTarget;

vec4 SpriteBase_Shading(PS_STREAMS _streams)
{
    return texture(SPIRV_Cross_CombinedTexturing_Texture0Texturing_Sampler, _streams.TexCoord_id5);
}

vec4 SpriteBatchShader_false__Shading(PS_STREAMS _streams)
{
    vec4 _15 = SpriteBase_Shading(_streams).xxxx;
    vec4 _16 = SpriteBase_Shading(_streams);
    bvec4 _17 = bvec4(abs(_streams.Swizzle_id2 - 1.0) <= 0.100000001490116119384765625);
    vec4 swizzleColor = vec4(_17.x ? _15.x : _16.x, _17.y ? _15.y : _16.y, _17.z ? _15.z : _16.z, _17.w ? _15.w : _16.w);
    if (abs(_streams.Swizzle_id2 - 2.0) <= 0.100000001490116119384765625)
    {
        float nX = (swizzleColor.x * 2.0) - 1.0;
        float nY = (swizzleColor.y * 2.0) - 1.0;
        swizzleColor.w = 1.0;
        float nZ = 1.0 - sqrt(clamp((nX * nX) + (nY * nY), 0.0, 1.0));
        swizzleColor.z = (nZ * 0.5) + 0.5;
    }
    if (abs(_streams.Swizzle_id2 - 3.0) <= 0.100000001490116119384765625)
    {
        swizzleColor = vec4(swizzleColor.x, swizzleColor.xx.x, swizzleColor.xx.y, swizzleColor.w);
        swizzleColor.w = 1.0;
    }
    vec4 finalColor = (swizzleColor * _streams.Color_id0) + _streams.ColorAdd_id1;
    return finalColor;
}

void main()
{
    PS_STREAMS _streams = PS_STREAMS(vec4(0.0), vec4(0.0), 0.0, vec4(0.0), vec4(0.0), vec2(0.0));
    _streams.Color_id0 = PS_IN_COLOR;
    _streams.ColorAdd_id1 = PS_IN_COLOR1;
    _streams.Swizzle_id2 = PS_IN_BATCH_SWIZZLE;
    _streams.ShadingPosition_id3 = PS_IN_SV_Position;
    _streams.TexCoord_id5 = PS_IN_TEXCOORD0;
    _streams.ColorTarget_id4 = SpriteBatchShader_false__Shading(_streams);
    PS_OUT_ColorTarget = _streams.ColorTarget_id4;
}


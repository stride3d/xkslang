#version 450

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

layout(location = 0) in vec4 PS_IN_Color;
layout(location = 1) in vec4 PS_IN_ColorAdd;
layout(location = 2) in float PS_IN_Swizzle;
layout(location = 3) in vec4 PS_IN_ShadingPosition;
layout(location = 4) in vec2 PS_IN_TexCoord;
layout(location = 0) out vec4 PS_OUT_ColorTarget;

vec4 SpriteBase_Shading(PS_STREAMS _streams)
{
    return texture(SPIRV_Cross_CombinedTexturing_Texture0Texturing_Sampler, _streams.TexCoord_id5);
}

vec4 SpriteBatchShader_false__Shading(PS_STREAMS _streams)
{
    vec4 swizzleColor;
    vec4 _5;
    if (abs(_streams.Swizzle_id2 - 1.0) <= 0.100000001490116119384765625)
    {
        _5 = SpriteBase_Shading(_streams).xxxx;
    }
    else
    {
        _5 = SpriteBase_Shading(_streams);
    }
    swizzleColor = _5;
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
    _streams.Color_id0 = PS_IN_Color;
    _streams.ColorAdd_id1 = PS_IN_ColorAdd;
    _streams.Swizzle_id2 = PS_IN_Swizzle;
    _streams.ShadingPosition_id3 = PS_IN_ShadingPosition;
    _streams.TexCoord_id5 = PS_IN_TexCoord;
    _streams.ColorTarget_id4 = SpriteBatchShader_false__Shading(_streams);
    PS_OUT_ColorTarget = _streams.ColorTarget_id4;
}


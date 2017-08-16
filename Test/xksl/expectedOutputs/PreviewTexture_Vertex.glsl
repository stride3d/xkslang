#version 450

struct VS_STREAMS
{
    vec4 Color_id0;
    vec4 ColorAdd_id1;
    float Swizzle_id2;
    vec4 ShadingPosition_id3;
    vec4 Position_id4;
    vec2 TexCoord_id5;
};

layout(std140) uniform PerDraw
{
    layout(row_major) mat4 SpriteBase_MatrixTransform;
} PerDraw_var;

layout(location = 0) in vec4 VS_IN_Color;
layout(location = 1) in vec4 VS_IN_ColorAdd;
layout(location = 2) in float VS_IN_Swizzle;
layout(location = 3) in vec4 VS_IN_Position;
layout(location = 4) in vec2 VS_IN_TexCoord;
layout(location = 0) out vec4 VS_OUT_Color;
layout(location = 1) out vec4 VS_OUT_ColorAdd;
layout(location = 2) out float VS_OUT_Swizzle;
layout(location = 3) out vec4 VS_OUT_ShadingPosition;
layout(location = 4) out vec2 VS_OUT_TexCoord;

void SpriteBase_VSMain(inout VS_STREAMS _streams)
{
    _streams.ShadingPosition_id3 = PerDraw_var.SpriteBase_MatrixTransform * _streams.Position_id4;
}

vec4 ColorUtility_ToLinear(vec4 sRGBa)
{
    vec3 sRGB = sRGBa.xyz;
    return vec4(sRGB * ((sRGB * ((sRGB * 0.305306017398834228515625) + vec3(0.6821711063385009765625))) + vec3(0.01252287812530994415283203125)), sRGBa.w);
}

void main()
{
    VS_STREAMS _streams = VS_STREAMS(vec4(0.0), vec4(0.0), 0.0, vec4(0.0), vec4(0.0), vec2(0.0));
    _streams.Color_id0 = VS_IN_Color;
    _streams.ColorAdd_id1 = VS_IN_ColorAdd;
    _streams.Swizzle_id2 = VS_IN_Swizzle;
    _streams.Position_id4 = VS_IN_Position;
    _streams.TexCoord_id5 = VS_IN_TexCoord;
    SpriteBase_VSMain(_streams);
    if (false)
    {
        vec4 param = _streams.Color_id0;
        _streams.Color_id0 = ColorUtility_ToLinear(param);
    }
    VS_OUT_Color = _streams.Color_id0;
    VS_OUT_ColorAdd = _streams.ColorAdd_id1;
    VS_OUT_Swizzle = _streams.Swizzle_id2;
    VS_OUT_ShadingPosition = _streams.ShadingPosition_id3;
    VS_OUT_TexCoord = _streams.TexCoord_id5;
}


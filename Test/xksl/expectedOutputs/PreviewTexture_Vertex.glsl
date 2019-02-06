#version 410
#ifdef GL_ARB_shading_language_420pack
#extension GL_ARB_shading_language_420pack : require
#endif

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

in vec4 VS_IN_COLOR;
in vec4 VS_IN_COLOR1;
in float VS_IN_BATCH_SWIZZLE;
in vec4 VS_IN_POSITION;
in vec2 VS_IN_TEXCOORD0;
out vec4 VS_OUT_Color;
out vec4 VS_OUT_ColorAdd;
out float VS_OUT_Swizzle;
out vec2 VS_OUT_TexCoord;

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
    _streams.Color_id0 = VS_IN_COLOR;
    _streams.ColorAdd_id1 = VS_IN_COLOR1;
    _streams.Swizzle_id2 = VS_IN_BATCH_SWIZZLE;
    _streams.Position_id4 = VS_IN_POSITION;
    _streams.TexCoord_id5 = VS_IN_TEXCOORD0;
    SpriteBase_VSMain(_streams);
    if (false)
    {
        vec4 param = _streams.Color_id0;
        _streams.Color_id0 = ColorUtility_ToLinear(param);
    }
    VS_OUT_Color = _streams.Color_id0;
    VS_OUT_ColorAdd = _streams.ColorAdd_id1;
    VS_OUT_Swizzle = _streams.Swizzle_id2;
    gl_Position = _streams.ShadingPosition_id3;
    VS_OUT_TexCoord = _streams.TexCoord_id5;
    gl_Position.z = 2.0 * gl_Position.z - gl_Position.w;
    gl_Position.y = -gl_Position.y;
}


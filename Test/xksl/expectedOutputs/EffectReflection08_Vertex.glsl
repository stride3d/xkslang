#version 410
#ifdef GL_ARB_shading_language_420pack
#extension GL_ARB_shading_language_420pack : require
#endif

layout(std140) uniform Globals
{
    vec4 ShaderMain_Color;
    float ShaderMain_IsOnlyChannelRed;
} Globals_var;

vec4 ShaderMain_Shading()
{
    vec4 color = vec4(0.0);
    if (Globals_var.ShaderMain_IsOnlyChannelRed != 0.0)
    {
        color = vec4(color.xxx, 1.0);
    }
    return color * Globals_var.ShaderMain_Color;
}

void main()
{
    vec4 f4 = ShaderMain_Shading();
    gl_Position.z = 2.0 * gl_Position.z - gl_Position.w;
    gl_Position.y = -gl_Position.y;
}


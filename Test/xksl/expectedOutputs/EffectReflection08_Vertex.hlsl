cbuffer Globals
{
    float4 ShaderMain_Color;
    float ShaderMain_IsOnlyChannelRed;
};

float4 ShaderMain_Shading()
{
    float4 color = float4(0.0f, 0.0f, 0.0f, 0.0f);
    if (ShaderMain_IsOnlyChannelRed != 0.0f)
    {
        color = float4(color.xxx, 1.0f);
    }
    return color * ShaderMain_Color;
}

void vert_main()
{
    float4 f4 = ShaderMain_Shading();
}

void main()
{
    vert_main();
}

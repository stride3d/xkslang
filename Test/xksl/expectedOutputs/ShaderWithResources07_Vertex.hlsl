struct _cb
{
    float3 ShaderA_direction;
    float2 ShaderA_uv2;
    int ShaderB_lightIndex;
};

cbuffer cb
{
    _cb cb_var;
};
SamplerState ShaderA_Sampler0;
Texture2D<float4> ShaderA_Texture0;
TextureCube<float4> ShaderA_CubeMap;

void vert_main()
{
    float4 color1 = ShaderA_Texture0.Sample(ShaderA_Sampler0, cb_var.ShaderA_uv2);
    float4 color2 = ShaderA_CubeMap.Sample(ShaderA_Sampler0, cb_var.ShaderA_direction);
}

void main()
{
    vert_main();
}

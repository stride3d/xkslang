cbuffer PerMaterial
{
    float2 ComputeColorTextureScaledOffsetDynamicSampler_id0_scale;
    float2 ComputeColorTextureScaledOffsetDynamicSampler_id0_offset;
};

float2 ShaderBase_Compute()
{
    return float2(1.0f, 1.0f);
}

float2 ComputeColorTextureScaledOffsetDynamicSampler_Material_DiffuseMap_TEXCOORD0_Material_Sampler_i0_rgba_Material_TextureScale_Material_TextureOffset_1__Compute()
{
    return (ShaderBase_Compute() + ComputeColorTextureScaledOffsetDynamicSampler_id0_scale) + ComputeColorTextureScaledOffsetDynamicSampler_id0_offset;
}

float2 ComputeColorTextureScaledOffsetDynamicSampler_Material_DiffuseMap_TEXCOORD0_Material_Sampler_i0_rgba_Material_TextureScale_Material_TextureOffset_2__Compute()
{
    return (ComputeColorTextureScaledOffsetDynamicSampler_Material_DiffuseMap_TEXCOORD0_Material_Sampler_i0_rgba_Material_TextureScale_Material_TextureOffset_1__Compute() + ComputeColorTextureScaledOffsetDynamicSampler_id0_scale) + ComputeColorTextureScaledOffsetDynamicSampler_id0_offset;
}

void frag_main()
{
    float2 f = ComputeColorTextureScaledOffsetDynamicSampler_Material_DiffuseMap_TEXCOORD0_Material_Sampler_i0_rgba_Material_TextureScale_Material_TextureOffset_2__Compute();
}

void main()
{
    frag_main();
}

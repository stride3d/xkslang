#version 450

layout(std140) uniform PerMaterial
{
    vec2 ComputeColorTextureScaledOffsetDynamicSampler_scale;
    vec2 ComputeColorTextureScaledOffsetDynamicSampler_offset;
} PerMaterial_var;

vec2 ShaderBase_Compute()
{
    return vec2(1.0);
}

vec2 ComputeColorTextureScaledOffsetDynamicSampler_Material_DiffuseMap_TEXCOORD0_Material_Sampler_i0_rgba_Material_TextureScale_Material_TextureOffset_1__Compute()
{
    return (ShaderBase_Compute() + PerMaterial_var.ComputeColorTextureScaledOffsetDynamicSampler_scale) + PerMaterial_var.ComputeColorTextureScaledOffsetDynamicSampler_offset;
}

vec2 ComputeColorTextureScaledOffsetDynamicSampler_Material_DiffuseMap_TEXCOORD0_Material_Sampler_i0_rgba_Material_TextureScale_Material_TextureOffset_2__Compute()
{
    return (ComputeColorTextureScaledOffsetDynamicSampler_Material_DiffuseMap_TEXCOORD0_Material_Sampler_i0_rgba_Material_TextureScale_Material_TextureOffset_1__Compute() + PerMaterial_var.ComputeColorTextureScaledOffsetDynamicSampler_scale) + PerMaterial_var.ComputeColorTextureScaledOffsetDynamicSampler_offset;
}

void main()
{
    vec2 f = ComputeColorTextureScaledOffsetDynamicSampler_Material_DiffuseMap_TEXCOORD0_Material_Sampler_i0_rgba_Material_TextureScale_Material_TextureOffset_2__Compute();
}


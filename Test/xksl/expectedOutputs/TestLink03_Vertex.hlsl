struct VS_STREAMS
{
    float4 s_in_id0;
    float4 s_int_id1;
};

cbuffer PerMaterial
{
    float4 ShaderMain_Material_DiffuseMap_Material_DiffuseSampler_Material_TextureScale_Material_TextureOffset__scale;
    float4 ShaderMain_Material_DiffuseMap_Material_DiffuseSampler_Material_TextureScale_Material_TextureOffset__offset;
    float4 ShaderMain_Material_DiffuseMap_Material_DiffuseSampler_Material_TextureScale_Material_TextureOffset__aCol;
    float2 ShaderMain_Material_DiffuseMap_Material_DiffuseSampler_Material_TextureScale_Material_TextureOffset__uv2;
};
Texture2D<float4> ShaderBase_Material_DiffuseSampler_Material_DiffuseMap__Texture0;
SamplerState ShaderBase_Material_DiffuseSampler_Material_DiffuseMap__Sampler0;

static float4 VS_IN_s_in;
static float4 VS_OUT_s_int;

struct SPIRV_Cross_Input
{
    float4 VS_IN_s_in : S_INPUT;
};

struct SPIRV_Cross_Output
{
    float4 VS_OUT_s_int : S_INT;
};

void vert_main()
{
    VS_STREAMS _streams = { float4(0.0f, 0.0f, 0.0f, 0.0f), float4(0.0f, 0.0f, 0.0f, 0.0f) };
    _streams.s_in_id0 = VS_IN_s_in;
    float4 color = ShaderBase_Material_DiffuseSampler_Material_DiffuseMap__Texture0.Sample(ShaderBase_Material_DiffuseSampler_Material_DiffuseMap__Sampler0, ShaderMain_Material_DiffuseMap_Material_DiffuseSampler_Material_TextureScale_Material_TextureOffset__uv2);
    _streams.s_int_id1 = _streams.s_in_id0 + ShaderMain_Material_DiffuseMap_Material_DiffuseSampler_Material_TextureScale_Material_TextureOffset__scale;
    VS_OUT_s_int = _streams.s_int_id1;
}

SPIRV_Cross_Output main(SPIRV_Cross_Input stage_input)
{
    VS_IN_s_in = stage_input.VS_IN_s_in;
    vert_main();
    SPIRV_Cross_Output stage_output;
    stage_output.VS_OUT_s_int = VS_OUT_s_int;
    return stage_output;
}

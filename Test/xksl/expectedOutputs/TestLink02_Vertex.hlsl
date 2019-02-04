struct VS_STREAMS
{
    float4 s_in_id0;
    float4 s_int_id1;
};

static const VS_STREAMS _42 = { 0.0f.xxxx, 0.0f.xxxx };

cbuffer PerMaterial
{
    float4 ShaderMain_scale;
    float4 ShaderMain_offset;
    float4 ShaderMain_aCol;
    float2 ShaderMain_uv2;
};
Texture2D<float4> ShaderMain_Texture0;
SamplerState ShaderMain_Sampler0;

static float4 VS_IN_S_INPUT;
static float4 VS_OUT_s_int;

struct SPIRV_Cross_Input
{
    float4 VS_IN_S_INPUT : S_INPUT;
};

struct SPIRV_Cross_Output
{
    float4 VS_OUT_s_int : S_INT;
};

void vert_main()
{
    VS_STREAMS _streams = _42;
    _streams.s_in_id0 = VS_IN_S_INPUT;
    float4 color = ShaderMain_Texture0.Sample(ShaderMain_Sampler0, ShaderMain_uv2);
    _streams.s_int_id1 = _streams.s_in_id0 + ShaderMain_scale;
    VS_OUT_s_int = _streams.s_int_id1;
}

SPIRV_Cross_Output main(SPIRV_Cross_Input stage_input)
{
    VS_IN_S_INPUT = stage_input.VS_IN_S_INPUT;
    vert_main();
    SPIRV_Cross_Output stage_output;
    stage_output.VS_OUT_s_int = VS_OUT_s_int;
    return stage_output;
}

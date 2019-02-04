struct ShaderMain_DirectionalLightData
{
    float3 DirectionWS;
    float3 Color;
};

struct VS_STREAMS
{
    ShaderMain_DirectionalLightData aStreamVar_id0;
};

static const ShaderMain_DirectionalLightData _20 = { 0.0f.xxx, 0.0f.xxx };
static const VS_STREAMS _24 = { { 0.0f.xxx, 0.0f.xxx } };

cbuffer aCBuffer
{
    ShaderMain_DirectionalLightData ShaderMain_toto;
};

static ShaderMain_DirectionalLightData VS_OUT_aStreamVar;

struct SPIRV_Cross_Output
{
    ShaderMain_DirectionalLightData VS_OUT_aStreamVar : ASTREAMVAR;
};

void vert_main()
{
    VS_STREAMS _streams = _24;
    _streams.aStreamVar_id0.Color = ShaderMain_toto.Color;
    VS_OUT_aStreamVar = _streams.aStreamVar_id0;
}

SPIRV_Cross_Output main()
{
    vert_main();
    SPIRV_Cross_Output stage_output;
    stage_output.VS_OUT_aStreamVar = VS_OUT_aStreamVar;
    return stage_output;
}

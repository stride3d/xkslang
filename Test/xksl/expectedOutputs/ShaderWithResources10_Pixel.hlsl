struct ShaderMain_SpotLightDataInternal
{
    float3 PositionWS;
    float3 DirectionWS;
    float3 AngleOffsetAndInvSquareRadius;
    float3 Color;
};

struct PS_STREAMS
{
    uint2 lightData_id0;
    int lightIndex_id1;
    float4 screenPosition_id2;
};

Buffer<uint4> ShaderMain_LightIndices;
Buffer<float4> ShaderMain_SpotLights;

static int PS_IN_lightIndex;

struct SPIRV_Cross_Input
{
    int PS_IN_lightIndex : LIGHTINDEX;
};

void ShaderMain_PrepareDirectLightCore(inout PS_STREAMS _streams, int lightIndexIgnored)
{
    int realLightIndex = int(ShaderMain_LightIndices.Load(_streams.lightIndex_id1).x);
    _streams.lightIndex_id1++;
    float4 spotLight1 = ShaderMain_SpotLights.Load(realLightIndex * 4);
    float4 spotLight2 = ShaderMain_SpotLights.Load((realLightIndex * 4) + 1);
    float4 spotLight3 = ShaderMain_SpotLights.Load((realLightIndex * 4) + 2);
    float4 spotLight4 = ShaderMain_SpotLights.Load((realLightIndex * 4) + 3);
    ShaderMain_SpotLightDataInternal spotLight;
    spotLight.PositionWS = spotLight1.xyz;
    spotLight.DirectionWS = spotLight2.xyz;
    spotLight.AngleOffsetAndInvSquareRadius = spotLight3.xyz;
    spotLight.Color = spotLight4.xyz;
}

void frag_main()
{
    PS_STREAMS _streams = { uint2(0u, 0u), 0, float4(0.0f, 0.0f, 0.0f, 0.0f) };
    _streams.lightIndex_id1 = PS_IN_lightIndex;
    _streams.lightData_id0 = uint2(0u, 0u);
    _streams.screenPosition_id2 = float4(1.0f, 2.0f, 3.0f, 4.0f);
    int param = 0;
    ShaderMain_PrepareDirectLightCore(_streams, param);
}

void main(SPIRV_Cross_Input stage_input)
{
    PS_IN_lightIndex = stage_input.PS_IN_lightIndex;
    frag_main();
}

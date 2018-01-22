struct LightDirectional_DirectionalLightData
{
    float3 DirectionWS;
    float3 Color;
};

struct LightPoint_PointLightDataInternal
{
    float3 PositionWS;
    float InvSquareRadius;
    float3 Color;
};

struct LightSpot_SpotLightDataInternal
{
    float3 PositionWS;
    float3 DirectionWS;
    float3 AngleOffsetAndInvSquareRadius;
    float3 Color;
};

struct VS_STREAMS
{
    float3 lightPositionWS_id0;
    float3 lightDirectionWS_id1;
    float3 lightColor_id2;
    float3 lightColorNdotL_id3;
    float NdotL_id4;
    float lightDirectAmbientOcclusion_id5;
    float3 normalWS_id6;
    float3 shadowColor_id7;
    float thicknessWS_id8;
    float4 PositionWS_id9;
    uint2 lightData_id10;
    int lightIndex_id11;
    float4 ShadingPosition_id12;
    float4 ScreenPosition_id13;
};

cbuffer PerView
{
    float o1S2C0_Camera_NearClipPlane;
    float o1S2C0_Camera_FarClipPlane;
    float2 o1S2C0_Camera_ZProjection;
    float2 o1S2C0_Camera_ViewSize;
    float o1S2C0_Camera_AspectRatio;
    float4 o0S2C0_LightDirectionalGroup__padding_PerView_Default;
    LightDirectional_DirectionalLightData o0S2C0_LightDirectionalGroup_Lights[8];
    int o0S2C0_DirectLightGroupPerView_LightCount;
    float o1S2C0_LightClustered_ClusterDepthScale;
    float o1S2C0_LightClustered_ClusterDepthBias;
    float2 o1S2C0_LightClustered_ClusterStride;
};
Buffer<float4> LightClusteredPointGroup_PointLights;
Texture3D<uint4> LightClustered_LightClusters;
Buffer<uint4> LightClustered_LightIndices;
Buffer<float4> LightClusteredSpotGroup_SpotLights;

static float VS_IN_lightDirectAmbientOcclusion;
static float3 VS_IN_normalWS;
static float4 VS_IN_PositionWS;
static float4 VS_IN_ShadingPosition;
static float4 VS_IN_ScreenPosition;
static float4 VS_OUT_ShadingPosition;
static float4 VS_OUT_ScreenPosition;

struct SPIRV_Cross_Input
{
    float VS_IN_lightDirectAmbientOcclusion : LIGHTDIRECTAMBIENTOCCLUSION;
    float3 VS_IN_normalWS : NORMALWS;
    float4 VS_IN_PositionWS : POSITION_WS;
    float4 VS_IN_ShadingPosition : SV_Position;
    float4 VS_IN_ScreenPosition : SCREENPOSITION;
};

struct SPIRV_Cross_Output
{
    float4 VS_OUT_ShadingPosition : SV_Position;
    float4 VS_OUT_ScreenPosition : SCREENPOSITION;
};

void o0S2C0_DirectLightGroup_PrepareDirectLights()
{
}

int o0S2C0_LightDirectionalGroup_8__GetMaxLightCount()
{
    return 8;
}

int o0S2C0_DirectLightGroupPerView_GetLightCount()
{
    return o0S2C0_DirectLightGroupPerView_LightCount;
}

void o0S2C0_LightDirectionalGroup_8__PrepareDirectLightCore(inout VS_STREAMS _streams, int lightIndex)
{
    _streams.lightColor_id2 = o0S2C0_LightDirectionalGroup_Lights[lightIndex].Color;
    _streams.lightDirectionWS_id1 = -o0S2C0_LightDirectionalGroup_Lights[lightIndex].DirectionWS;
}

float3 o0S2C0_ShadowGroup_ComputeShadow(inout VS_STREAMS _streams, float3 position, int lightIndex)
{
    _streams.thicknessWS_id8 = 0.0f;
    return 1.0f.xxx;
}

void o0S2C0_DirectLightGroup_PrepareDirectLight(inout VS_STREAMS _streams, int lightIndex)
{
    int param = lightIndex;
    o0S2C0_LightDirectionalGroup_8__PrepareDirectLightCore(_streams, param);
    _streams.NdotL_id4 = max(dot(_streams.normalWS_id6, _streams.lightDirectionWS_id1), 9.9999997473787516355514526367188e-05f);
    float3 param_1 = _streams.PositionWS_id9.xyz;
    int param_2 = lightIndex;
    float3 _52 = o0S2C0_ShadowGroup_ComputeShadow(_streams, param_1, param_2);
    _streams.shadowColor_id7 = _52;
    _streams.lightColorNdotL_id3 = ((_streams.lightColor_id2 * _streams.shadowColor_id7) * _streams.NdotL_id4) * _streams.lightDirectAmbientOcclusion_id5;
}

void o1S2C0_LightClustered_PrepareLightData(inout VS_STREAMS _streams)
{
    float projectedDepth = _streams.ShadingPosition_id12.z;
    float depth = o1S2C0_Camera_ZProjection.y / (projectedDepth - o1S2C0_Camera_ZProjection.x);
    float2 texCoord = float2(_streams.ScreenPosition_id13.x + 1.0f, 1.0f - _streams.ScreenPosition_id13.y) * 0.5f;
    int slice = int(max(log2((depth * o1S2C0_LightClustered_ClusterDepthScale) + o1S2C0_LightClustered_ClusterDepthBias), 0.0f));
    _streams.lightData_id10 = uint2(LightClustered_LightClusters.Load(int4(int4(int2(texCoord * o1S2C0_LightClustered_ClusterStride), slice, 0).xyz, int4(int2(texCoord * o1S2C0_LightClustered_ClusterStride), slice, 0).w)).xy);
    _streams.lightIndex_id11 = int(_streams.lightData_id10.x);
}

void o1S2C0_LightClusteredPointGroup_PrepareDirectLights(inout VS_STREAMS _streams)
{
    o1S2C0_LightClustered_PrepareLightData(_streams);
}

int o1S2C0_LightClusteredPointGroup_GetMaxLightCount(VS_STREAMS _streams)
{
    return int(_streams.lightData_id10.y & 65535u);
}

int o1S2C0_LightClusteredPointGroup_GetLightCount(VS_STREAMS _streams)
{
    return int(_streams.lightData_id10.y & 65535u);
}

float o1S2C0_LightUtil_SmoothDistanceAttenuation(float squaredDistance, float lightInvSquareRadius)
{
    float factor = squaredDistance * lightInvSquareRadius;
    float smoothFactor = clamp(1.0f - (factor * factor), 0.0f, 1.0f);
    return smoothFactor * smoothFactor;
}

float o1S2C0_LightUtil_GetDistanceAttenuation(float3 lightVector, float lightInvSquareRadius)
{
    float d2 = dot(lightVector, lightVector);
    float attenuation = 1.0f / max(d2, 9.9999997473787516355514526367188e-05f);
    float param = d2;
    float param_1 = lightInvSquareRadius;
    attenuation *= o1S2C0_LightUtil_SmoothDistanceAttenuation(param, param_1);
    return attenuation;
}

float o1S2C0_LightPoint_ComputeAttenuation(LightPoint_PointLightDataInternal light, float3 position, out float3 lightVectorNorm)
{
    float3 lightVector = light.PositionWS - position;
    float lightVectorLength = length(lightVector);
    lightVectorNorm = lightVector / lightVectorLength.xxx;
    float lightInvSquareRadius = light.InvSquareRadius;
    float3 param = lightVector;
    float param_1 = lightInvSquareRadius;
    return o1S2C0_LightUtil_GetDistanceAttenuation(param, param_1);
}

void o1S2C0_LightPoint_ProcessLight(inout VS_STREAMS _streams, LightPoint_PointLightDataInternal light)
{
    LightPoint_PointLightDataInternal param = light;
    float3 param_1 = _streams.PositionWS_id9.xyz;
    float3 lightVectorNorm;
    float3 param_2 = lightVectorNorm;
    float _199 = o1S2C0_LightPoint_ComputeAttenuation(param, param_1, param_2);
    lightVectorNorm = param_2;
    float attenuation = _199;
    _streams.lightPositionWS_id0 = light.PositionWS;
    _streams.lightColor_id2 = light.Color * attenuation;
    _streams.lightDirectionWS_id1 = lightVectorNorm;
}

void o1S2C0_LightClusteredPointGroup_PrepareDirectLightCore(inout VS_STREAMS _streams, int lightIndexIgnored)
{
    int realLightIndex = int(LightClustered_LightIndices.Load(_streams.lightIndex_id11).x);
    _streams.lightIndex_id11++;
    float4 pointLight1 = LightClusteredPointGroup_PointLights.Load(realLightIndex * 2);
    float4 pointLight2 = LightClusteredPointGroup_PointLights.Load((realLightIndex * 2) + 1);
    LightPoint_PointLightDataInternal pointLight;
    pointLight.PositionWS = pointLight1.xyz;
    pointLight.InvSquareRadius = pointLight1.w;
    pointLight.Color = pointLight2.xyz;
    LightPoint_PointLightDataInternal param = pointLight;
    o1S2C0_LightPoint_ProcessLight(_streams, param);
}

float3 o1S2C0_ShadowGroup_ComputeShadow(inout VS_STREAMS _streams, float3 position, int lightIndex)
{
    _streams.thicknessWS_id8 = 0.0f;
    return 1.0f.xxx;
}

void o1S2C0_DirectLightGroup_PrepareDirectLight(inout VS_STREAMS _streams, int lightIndex)
{
    int param = lightIndex;
    o1S2C0_LightClusteredPointGroup_PrepareDirectLightCore(_streams, param);
    _streams.NdotL_id4 = max(dot(_streams.normalWS_id6, _streams.lightDirectionWS_id1), 9.9999997473787516355514526367188e-05f);
    float3 param_1 = _streams.PositionWS_id9.xyz;
    int param_2 = lightIndex;
    float3 _115 = o1S2C0_ShadowGroup_ComputeShadow(_streams, param_1, param_2);
    _streams.shadowColor_id7 = _115;
    _streams.lightColorNdotL_id3 = ((_streams.lightColor_id2 * _streams.shadowColor_id7) * _streams.NdotL_id4) * _streams.lightDirectAmbientOcclusion_id5;
}

void o2S2C0_DirectLightGroup_PrepareDirectLights()
{
}

int o2S2C0_LightClusteredSpotGroup_GetMaxLightCount(VS_STREAMS _streams)
{
    return int(_streams.lightData_id10.y >> uint(16));
}

int o2S2C0_LightClusteredSpotGroup_GetLightCount(VS_STREAMS _streams)
{
    return int(_streams.lightData_id10.y >> uint(16));
}

float o2S2C0_LightUtil_SmoothDistanceAttenuation(float squaredDistance, float lightInvSquareRadius)
{
    float factor = squaredDistance * lightInvSquareRadius;
    float smoothFactor = clamp(1.0f - (factor * factor), 0.0f, 1.0f);
    return smoothFactor * smoothFactor;
}

float o2S2C0_LightUtil_GetDistanceAttenuation(float3 lightVector, float lightInvSquareRadius)
{
    float d2 = dot(lightVector, lightVector);
    float attenuation = 1.0f / max(d2, 9.9999997473787516355514526367188e-05f);
    float param = d2;
    float param_1 = lightInvSquareRadius;
    attenuation *= o2S2C0_LightUtil_SmoothDistanceAttenuation(param, param_1);
    return attenuation;
}

float o2S2C0_LightUtil_GetAngleAttenuation(float3 lightVector, float3 lightDirection, float lightAngleScale, float lightAngleOffset)
{
    float cd = dot(lightDirection, lightVector);
    float attenuation = clamp((cd * lightAngleScale) + lightAngleOffset, 0.0f, 1.0f);
    attenuation *= attenuation;
    return attenuation;
}

float o2S2C0_LightSpot_ComputeAttenuation(LightSpot_SpotLightDataInternal light, float3 position, inout float3 lightVectorNorm)
{
    float3 lightVector = light.PositionWS - position;
    float lightVectorLength = length(lightVector);
    lightVectorNorm = lightVector / lightVectorLength.xxx;
    float3 lightAngleOffsetAndInvSquareRadius = light.AngleOffsetAndInvSquareRadius;
    float2 lightAngleAndOffset = lightAngleOffsetAndInvSquareRadius.xy;
    float lightInvSquareRadius = lightAngleOffsetAndInvSquareRadius.z;
    float3 lightDirection = -light.DirectionWS;
    float attenuation = 1.0f;
    float3 param = lightVector;
    float param_1 = lightInvSquareRadius;
    attenuation *= o2S2C0_LightUtil_GetDistanceAttenuation(param, param_1);
    float3 param_2 = lightVectorNorm;
    float3 param_3 = lightDirection;
    float param_4 = lightAngleAndOffset.x;
    float param_5 = lightAngleAndOffset.y;
    attenuation *= o2S2C0_LightUtil_GetAngleAttenuation(param_2, param_3, param_4, param_5);
    return attenuation;
}

void o2S2C0_LightSpot_ProcessLight(inout VS_STREAMS _streams, LightSpot_SpotLightDataInternal light)
{
    LightSpot_SpotLightDataInternal param = light;
    float3 param_1 = _streams.PositionWS_id9.xyz;
    float3 lightVectorNorm;
    float3 param_2 = lightVectorNorm;
    float _482 = o2S2C0_LightSpot_ComputeAttenuation(param, param_1, param_2);
    lightVectorNorm = param_2;
    float attenuation = _482;
    _streams.lightColor_id2 = light.Color * attenuation;
    _streams.lightDirectionWS_id1 = lightVectorNorm;
}

void o2S2C0_LightClusteredSpotGroup_PrepareDirectLightCore(inout VS_STREAMS _streams, int lightIndexIgnored)
{
    int realLightIndex = int(LightClustered_LightIndices.Load(_streams.lightIndex_id11).x);
    _streams.lightIndex_id11++;
    float4 spotLight1 = LightClusteredSpotGroup_SpotLights.Load(realLightIndex * 4);
    float4 spotLight2 = LightClusteredSpotGroup_SpotLights.Load((realLightIndex * 4) + 1);
    float4 spotLight3 = LightClusteredSpotGroup_SpotLights.Load((realLightIndex * 4) + 2);
    float4 spotLight4 = LightClusteredSpotGroup_SpotLights.Load((realLightIndex * 4) + 3);
    LightSpot_SpotLightDataInternal spotLight;
    spotLight.PositionWS = spotLight1.xyz;
    spotLight.DirectionWS = spotLight2.xyz;
    spotLight.AngleOffsetAndInvSquareRadius = spotLight3.xyz;
    spotLight.Color = spotLight4.xyz;
    LightSpot_SpotLightDataInternal param = spotLight;
    o2S2C0_LightSpot_ProcessLight(_streams, param);
}

float3 o2S2C0_ShadowGroup_ComputeShadow(inout VS_STREAMS _streams, float3 position, int lightIndex)
{
    _streams.thicknessWS_id8 = 0.0f;
    return 1.0f.xxx;
}

void o2S2C0_DirectLightGroup_PrepareDirectLight(inout VS_STREAMS _streams, int lightIndex)
{
    int param = lightIndex;
    o2S2C0_LightClusteredSpotGroup_PrepareDirectLightCore(_streams, param);
    _streams.NdotL_id4 = max(dot(_streams.normalWS_id6, _streams.lightDirectionWS_id1), 9.9999997473787516355514526367188e-05f);
    float3 param_1 = _streams.PositionWS_id9.xyz;
    int param_2 = lightIndex;
    float3 _386 = o2S2C0_ShadowGroup_ComputeShadow(_streams, param_1, param_2);
    _streams.shadowColor_id7 = _386;
    _streams.lightColorNdotL_id3 = ((_streams.lightColor_id2 * _streams.shadowColor_id7) * _streams.NdotL_id4) * _streams.lightDirectAmbientOcclusion_id5;
}

void vert_main()
{
    VS_STREAMS _streams = { 0.0f.xxx, 0.0f.xxx, 0.0f.xxx, 0.0f.xxx, 0.0f, 0.0f, 0.0f.xxx, 0.0f.xxx, 0.0f, 0.0f.xxxx, uint2(0u, 0u), 0, 0.0f.xxxx, 0.0f.xxxx };
    _streams.lightDirectAmbientOcclusion_id5 = VS_IN_lightDirectAmbientOcclusion;
    _streams.normalWS_id6 = VS_IN_normalWS;
    _streams.PositionWS_id9 = VS_IN_PositionWS;
    _streams.ShadingPosition_id12 = VS_IN_ShadingPosition;
    _streams.ScreenPosition_id13 = VS_IN_ScreenPosition;
    o0S2C0_DirectLightGroup_PrepareDirectLights();
    int maxLightCount = o0S2C0_LightDirectionalGroup_8__GetMaxLightCount();
    int count = o0S2C0_DirectLightGroupPerView_GetLightCount();
    int i = 0;
    int param;
    for (; i < maxLightCount; i++)
    {
        if (i >= count)
        {
            break;
        }
        param = i;
        o0S2C0_DirectLightGroup_PrepareDirectLight(_streams, param);
    }
    o1S2C0_LightClusteredPointGroup_PrepareDirectLights(_streams);
    maxLightCount = o1S2C0_LightClusteredPointGroup_GetMaxLightCount(_streams);
    count = o1S2C0_LightClusteredPointGroup_GetLightCount(_streams);
    i = 0;
    for (; i < maxLightCount; i++)
    {
        if (i >= count)
        {
            break;
        }
        param = i;
        o1S2C0_DirectLightGroup_PrepareDirectLight(_streams, param);
    }
    o2S2C0_DirectLightGroup_PrepareDirectLights();
    maxLightCount = o2S2C0_LightClusteredSpotGroup_GetMaxLightCount(_streams);
    count = o2S2C0_LightClusteredSpotGroup_GetLightCount(_streams);
    i = 0;
    for (; i < maxLightCount; i++)
    {
        if (i >= count)
        {
            break;
        }
        param = i;
        o2S2C0_DirectLightGroup_PrepareDirectLight(_streams, param);
    }
    VS_OUT_ShadingPosition = _streams.ShadingPosition_id12;
    VS_OUT_ScreenPosition = _streams.ScreenPosition_id13;
}

SPIRV_Cross_Output main(SPIRV_Cross_Input stage_input)
{
    VS_IN_lightDirectAmbientOcclusion = stage_input.VS_IN_lightDirectAmbientOcclusion;
    VS_IN_normalWS = stage_input.VS_IN_normalWS;
    VS_IN_PositionWS = stage_input.VS_IN_PositionWS;
    VS_IN_ShadingPosition = stage_input.VS_IN_ShadingPosition;
    VS_IN_ScreenPosition = stage_input.VS_IN_ScreenPosition;
    vert_main();
    SPIRV_Cross_Output stage_output;
    stage_output.VS_OUT_ShadingPosition = VS_OUT_ShadingPosition;
    stage_output.VS_OUT_ScreenPosition = VS_OUT_ScreenPosition;
    return stage_output;
}

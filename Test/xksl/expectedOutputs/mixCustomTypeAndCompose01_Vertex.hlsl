struct LightDirectional_DirectionalLightData
{
    float4 DirectionWS;
    float4 Color;
};

cbuffer PerView
{
    float4x4 ShaderMain_View;
    float4x4 ShaderMain_ViewInverse;
    float4x4 ShaderMain_Projection;
    float4x4 ShaderMain_ProjectionInverse;
    float4x4 ShaderMain_ViewProjection;
    float2 ShaderMain_ProjScreenRay;
    float4 ShaderMain_Eye;
    LightDirectional_DirectionalLightData o0S5C0_LightDirectionalGroup_id0_Lights[8];
};

float4 o0S5C0_LightDirectionalGroup_8__GetMaxLightCount()
{
    return float4(8.0f, 0.0f, 0.0f, 0.0f) + o0S5C0_LightDirectionalGroup_id0_Lights[0].Color;
}

float4 o1S5C1_LightDirectionalGroup_8__GetMaxLightCount()
{
    return float4(8.0f, 0.0f, 0.0f, 0.0f) + o0S5C0_LightDirectionalGroup_id0_Lights[0].Color;
}

float4 ShaderMain_ComputeShadingPosition(float4 world)
{
    return mul(world, ShaderMain_ViewProjection);
}

void vert_main()
{
    float4 param = o0S5C0_LightDirectionalGroup_8__GetMaxLightCount() + o1S5C1_LightDirectionalGroup_8__GetMaxLightCount();
    float4 f = ShaderMain_ComputeShadingPosition(param);
}

void main()
{
    vert_main();
}

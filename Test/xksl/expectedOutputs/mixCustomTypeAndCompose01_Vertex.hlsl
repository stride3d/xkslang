struct LightDirectional_DirectionalLightData
{
    float4 DirectionWS;
    float4 Color;
};

cbuffer PerView
{
    column_major float4x4 ShaderMain_View;
    column_major float4x4 ShaderMain_ViewInverse;
    column_major float4x4 ShaderMain_Projection;
    column_major float4x4 ShaderMain_ProjectionInverse;
    column_major float4x4 ShaderMain_ViewProjection;
    float2 ShaderMain_ProjScreenRay;
    float4 ShaderMain_Eye;
    float4 o0S5C0_LightDirectionalGroup__padding_PerView_Default;
    LightDirectional_DirectionalLightData o0S5C0_LightDirectionalGroup_Lights[8];
    float4 o1S5C1_LightDirectionalGroup__padding_PerView_Lighting;
};

float4 o0S5C0_LightDirectionalGroup_8__GetMaxLightCount()
{
    return float4(8.0f, 0.0f, 0.0f, 0.0f) + o0S5C0_LightDirectionalGroup_Lights[0].Color;
}

float4 o1S5C1_LightDirectionalGroup_8__GetMaxLightCount()
{
    return float4(8.0f, 0.0f, 0.0f, 0.0f) + o0S5C0_LightDirectionalGroup_Lights[0].Color;
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

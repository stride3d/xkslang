#version 450

struct LightDirectional_DirectionalLightData
{
    vec4 DirectionWS;
    vec4 Color;
};

layout(std140) uniform PerView
{
    layout(row_major) mat4 ShaderMain_View;
    layout(row_major) mat4 ShaderMain_ViewInverse;
    layout(row_major) mat4 ShaderMain_Projection;
    layout(row_major) mat4 ShaderMain_ProjectionInverse;
    layout(row_major) mat4 ShaderMain_ViewProjection;
    vec2 ShaderMain_ProjScreenRay;
    vec4 ShaderMain_Eye;
    LightDirectional_DirectionalLightData LightDirectionalGroup_Lights[8];
    LightDirectional_DirectionalLightData LightDirectionalGroup_Lights_1[7];
} PerView_var;

vec4 o0S5C0_LightDirectionalGroup_8__GetMaxLightCount()
{
    return vec4(8.0, 0.0, 0.0, 0.0) + PerView_var.LightDirectionalGroup_Lights[0].Color;
}

vec4 o1S5C1_LightDirectionalGroup_7__GetMaxLightCount()
{
    return vec4(7.0, 0.0, 0.0, 0.0) + PerView_var.LightDirectionalGroup_Lights_1[0].Color;
}

vec4 ShaderMain_ComputeShadingPosition(vec4 world)
{
    return PerView_var.ShaderMain_ViewProjection * world;
}

void main()
{
    vec4 param = o0S5C0_LightDirectionalGroup_8__GetMaxLightCount() + o1S5C1_LightDirectionalGroup_7__GetMaxLightCount();
    vec4 f = ShaderMain_ComputeShadingPosition(param);
}


#version 450

void o0S2C0_ShaderComp_Compute()
{
    vec2 uv2 = vec2(0.5);
    uvec2 lightData = uvec2(texelFetch(ShaderComp_LightClusters, ivec4(ivec2(uv2), 0, 0).xyz, ivec4(ivec2(uv2), 0, 0).w).xy);
}

void main()
{
    o0S2C0_ShaderComp_Compute();
}


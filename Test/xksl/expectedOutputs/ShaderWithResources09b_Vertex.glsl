#version 410
#ifdef GL_ARB_shading_language_420pack
#extension GL_ARB_shading_language_420pack : require
#endif

uniform usampler3D SPIRV_Cross_CombinedShaderComp_LightClustersSPIRV_Cross_DummySampler;

void o0S2C0_ShaderComp_Compute()
{
    vec2 uv2 = vec2(0.5);
    uvec2 lightData = uvec2(texelFetch(SPIRV_Cross_CombinedShaderComp_LightClustersSPIRV_Cross_DummySampler, ivec4(ivec2(uv2), 0, 0).xyz, ivec4(ivec2(uv2), 0, 0).w).xy);
}

void main()
{
    o0S2C0_ShaderComp_Compute();
    gl_Position.z = 2.0 * gl_Position.z - gl_Position.w;
    gl_Position.y = -gl_Position.y;
}


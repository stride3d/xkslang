#version 410
#ifdef GL_ARB_shading_language_420pack
#extension GL_ARB_shading_language_420pack : require
#endif

vec3 ShaderMain_compute(vec3 f)
{
    return f;
}

void main()
{
    vec3 param = vec3(1.0);
    vec3 f3 = ShaderMain_compute(param);
    gl_Position.z = 2.0 * gl_Position.z - gl_Position.w;
    gl_Position.y = -gl_Position.y;
}


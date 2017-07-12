#version 450

vec3 ShaderMain_compute(vec3 f)
{
    return f;
}

void main()
{
    vec3 param = vec3(1.0);
    vec3 f3 = ShaderMain_compute(param);
}


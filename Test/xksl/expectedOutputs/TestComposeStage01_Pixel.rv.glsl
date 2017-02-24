#version 450

struct globalStreams
{
    int aStream1_0;
    int aStream2_s1;
    vec4 SV_SEMANTIC_s2;
    vec4 SV_OUTPUT_s3;
    vec4 SV_TOTO_s4;
    vec2 SV_TOTO_s5;
    int aStream1_6;
    int aStream1_7;
};

globalStreams globalStreams_var;

int o0S5C0_Color_Compute(int i)
{
    globalStreams_var.SV_OUTPUT_s3 = vec4(float(i));
    globalStreams_var.aStream1_6 = i + globalStreams_var.aStream2_s1;
    int _35 = ivec4(globalStreams_var.SV_SEMANTIC_s2 + vec4(float(i)));
    return _35;
}

int o1S5C0_Color_Compute(int i)
{
    globalStreams_var.SV_OUTPUT_s3 = vec4(float(i));
    globalStreams_var.aStream1_7 = i + globalStreams_var.aStream2_s1;
    int _56 = ivec4(globalStreams_var.SV_SEMANTIC_s2 + vec4(float(i)));
    return _56;
}

int main()
{
    int res = 0;
    int param = 1;
    int _74 = o0S5C0_Color_Compute(param);
    res += _74;
    param = 1;
    int _77 = o1S5C0_Color_Compute(param);
    res += _77;
    globalStreams_var.SV_OUTPUT_s3 += vec4(float(res));
    int _6 = ivec4(globalStreams_var.SV_OUTPUT_s3);
    return _6;
}


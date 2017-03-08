#version 450

struct globalStreams
{
    int aStream1_0;
    int aStream2_s1;
    vec4 aStream3_s2;
    vec4 aStreamBis_s3;
    int aStream1_4;
    int aStream1_5;
};

globalStreams globalStreams_var;

int o0S5C0_Color_Compute(int i)
{
    globalStreams_var.aStreamBis_s3 = vec4(float(i));
    globalStreams_var.aStream1_4 = i + globalStreams_var.aStream2_s1;
    int _34 = ivec4(globalStreams_var.aStream3_s2 + vec4(float(i)));
    return _34;
}

int o1S5C0_Color_Compute(int i)
{
    globalStreams_var.aStreamBis_s3 = vec4(float(i));
    globalStreams_var.aStream1_5 = i + globalStreams_var.aStream2_s1;
    int _55 = ivec4(globalStreams_var.aStream3_s2 + vec4(float(i)));
    return _55;
}

int main()
{
    int res = 0;
    int param = 1;
    int _73 = o0S5C0_Color_Compute(param);
    res += _73;
    param = 1;
    int _76 = o1S5C0_Color_Compute(param);
    res += _76;
    globalStreams_var.aStreamBis_s3 += vec4(float(res));
    int _6 = ivec4(globalStreams_var.aStreamBis_s3);
    return _6;
}


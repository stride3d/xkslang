struct globalStreams
{
    int aStream1_0;
    int aStream2_s1;
    float4 aStream3_s2;
    float4 aStreamBis_s3;
    int aStream1_4;
    int aStream1_5;
};

static globalStreams globalStreams_var;

int o0S5C0_Color_Compute(int i)
{
    float _34 = float(i);
    globalStreams_var.aStreamBis_s3 = float4(_34, _34, _34, _34);
    globalStreams_var.aStream1_4 = i + globalStreams_var.aStream2_s1;
    float _45 = float(i);
    return int4(globalStreams_var.aStream3_s2 + float4(_45, _45, _45, _45)).x;
}

int o1S5C0_Color_Compute(int i)
{
    float _54 = float(i);
    globalStreams_var.aStreamBis_s3 = float4(_54, _54, _54, _54);
    globalStreams_var.aStream1_5 = i + globalStreams_var.aStream2_s1;
    float _65 = float(i);
    return int4(globalStreams_var.aStream3_s2 + float4(_65, _65, _65, _65)).x;
}

int frag_main()
{
    int res = 0;
    int param = 1;
    int _70 = o0S5C0_Color_Compute(param);
    res += _70;
    param = 1;
    int _73 = o1S5C0_Color_Compute(param);
    res += _73;
    float _7 = float(res);
    globalStreams_var.aStreamBis_s3 += float4(_7, _7, _7, _7);
    return int4(globalStreams_var.aStreamBis_s3).x;
}

void main()
{
    frag_main();
}

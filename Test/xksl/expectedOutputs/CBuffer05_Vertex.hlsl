cbuffer PerDraw
{
    int ShaderMain_aInt;
    float ShaderMain_aFloat;
    float ShaderMain_aFloatB;
    column_major float4x4 ShaderMain_arrayA[3];
    float ShaderMain_varf;
    column_major float3x4 ShaderMain_arrayB[4];
    column_major float3x4 ShaderMain_af3x4;
    column_major int4x4 ShaderMain_i4x4;
    column_major int4x4 ShaderMain_i4x4A[2];
    column_major float4x4 ShaderMain_arrayC[5];
    float4 ShaderMain_arrayD[2];
    float ShaderMain_BleedingFactor;
    uint ShaderMain_aB320;
    uint ShaderMain_aB321;
    column_major float4x2 ShaderMain_arrayCC[1];
    uint ShaderMain_aBA[2];
    float4 ShaderMain_arrayAA4[4];
    float2 ShaderMain_arrayAA2[4];
    float4 ShaderMain_af02;
};
cbuffer ReferenceCBuffer
{
    int ShaderMain__aInt;
    float ShaderMain__aFloat;
    float ShaderMain__aFloatB;
    column_major float4x4 ShaderMain__arrayA[3];
    float ShaderMain__varf;
    column_major float3x4 ShaderMain__arrayB[4];
    column_major float3x4 ShaderMain__af3x4;
    column_major int4x4 ShaderMain__i4x4;
    column_major int4x4 ShaderMain__i4x4A[2];
    column_major float4x4 ShaderMain__arrayC[5];
    float4 ShaderMain__arrayD[2];
    float ShaderMain__BleedingFactor;
    uint ShaderMain__aB320;
    uint ShaderMain__aB321;
    column_major float4x2 ShaderMain__arrayCC[1];
    uint ShaderMain__aBA[2];
    float4 ShaderMain__arrayAA4[4];
    float2 ShaderMain__arrayAA2[4];
    float4 ShaderMain__af02;
};

void vert_main()
{
    float _9 = float(ShaderMain__aInt);
    float4 _10 = float4(_9, _9, _9, _9);
    float4x4 f44 = float4x4(ShaderMain_arrayA[0][0] + _10, ShaderMain_arrayA[0][1] + _10, ShaderMain_arrayA[0][2] + _10, ShaderMain_arrayA[0][3] + _10);
}

void main()
{
    vert_main();
}

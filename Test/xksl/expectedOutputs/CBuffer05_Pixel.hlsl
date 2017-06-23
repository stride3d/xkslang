cbuffer PerDraw
{
    int ShaderMain_aInt;
    float ShaderMain_aFloat;
    float ShaderMain_aFloatB;
    float4x4 ShaderMain_arrayA[3];
    float ShaderMain_varf;
    float3x4 ShaderMain_arrayB[4];
    float3x4 ShaderMain_af3x4;
    int4x4 ShaderMain_i4x4;
    int4x4 ShaderMain_i4x4A[2];
    float4x4 ShaderMain_arrayC[5];
    float4 ShaderMain_arrayD[2];
    float ShaderMain_BleedingFactor;
    uint ShaderMain_aB320;
    uint ShaderMain_aB321;
    float4x2 ShaderMain_arrayCC[1];
    uint ShaderMain_aBA[2];
    float4 ShaderMain_arrayAA4[4];
    float2 ShaderMain_arrayAA2[4];
    float4 ShaderMain_af02;
};

void frag_main()
{
    float4 f4 = ShaderMain_arrayD[1] + ShaderMain_arrayAA4[0];
}

void main()
{
    frag_main();
}

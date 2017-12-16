struct VS_STREAMS
{
    float4 meshTangent_id0;
};

static float4 VS_IN_meshTangent;

struct SPIRV_Cross_Input
{
    float4 VS_IN_meshTangent : TANGENT;
};

float3x3 ShaderTest_GetTangentMatrix()
{
    return float3x3(float3(1.0f, 0.0f, 0.0f), float3(0.0f, 1.0f, 0.0f), float3(0.0f, 0.0f, 1.0f));
}

void vert_main()
{
    VS_STREAMS _streams = { float4(0.0f, 0.0f, 0.0f, 0.0f) };
    _streams.meshTangent_id0 = VS_IN_meshTangent;
    float3x3 f3x3 = ShaderTest_GetTangentMatrix();
    float3 f3 = _streams.meshTangent_id0.xyz;
    int k = 10;
    k = int4(_streams.meshTangent_id0).x;
}

void main(SPIRV_Cross_Input stage_input)
{
    VS_IN_meshTangent = stage_input.VS_IN_meshTangent;
    vert_main();
}

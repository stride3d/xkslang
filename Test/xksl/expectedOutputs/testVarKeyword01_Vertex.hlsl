struct VS_STREAMS
{
    float4 meshTangent_id0;
};

static const VS_STREAMS _45 = { 0.0f.xxxx };

static float4 VS_IN_TANGENT;

struct SPIRV_Cross_Input
{
    float4 VS_IN_TANGENT : TANGENT;
};

float3x3 ShaderTest_GetTangentMatrix()
{
    float3x3 tangent = float3x3(float3(1.0f, 0.0f, 0.0f), float3(0.0f, 1.0f, 0.0f), float3(0.0f, 0.0f, 1.0f));
    return tangent;
}

void vert_main()
{
    VS_STREAMS _streams = _45;
    _streams.meshTangent_id0 = VS_IN_TANGENT;
    float3x3 f3x3 = ShaderTest_GetTangentMatrix();
    float3 f3 = _streams.meshTangent_id0.xyz;
    int k = 10;
    k = int4(_streams.meshTangent_id0).x;
}

void main(SPIRV_Cross_Input stage_input)
{
    VS_IN_TANGENT = stage_input.VS_IN_TANGENT;
    vert_main();
}

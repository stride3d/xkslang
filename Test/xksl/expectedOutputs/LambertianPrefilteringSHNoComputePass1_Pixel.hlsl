struct PS_STREAMS
{
    float2 TexCoord_id0;
    float SHBaseValues_id1[4];
    float4 ShadingPosition_id2;
    float4 ColorTarget_id3;
};

cbuffer globalRGroup
{
    int LambertianPrefilteringSHNoComputePass1_CoefficientIndex;
};
TextureCube<float4> LambertianPrefilteringSHNoComputePass1_RadianceMap;
SamplerState Texturing_PointSampler;

static float2 PS_IN_TexCoord;
static float4 PS_IN_ShadingPosition;
static float4 PS_OUT_ColorTarget;

struct SPIRV_Cross_Input
{
    float2 PS_IN_TexCoord : TEXCOORD0;
    float4 PS_IN_ShadingPosition : SV_Position;
};

struct SPIRV_Cross_Output
{
    float4 PS_OUT_ColorTarget : SV_Target0;
};

float3 LambertianPrefilteringSHNoComputePass1_2__uvToDirectionVS(float u, float v, int viewIndex)
{
    if (viewIndex == 0)
    {
        return float3(1.0f, -v, -u);
    }
    if (viewIndex == 1)
    {
        return float3(-1.0f, -v, u);
    }
    if (viewIndex == 2)
    {
        return float3(u, 1.0f, v);
    }
    if (viewIndex == 3)
    {
        return float3(u, -1.0f, -v);
    }
    if (viewIndex == 4)
    {
        return float3(u, -v, 1.0f);
    }
    if (viewIndex == 5)
    {
        return float3(-u, -v, -1.0f);
    }
    return 0.0f.xxx;
}

void SphericalHarmonicsBase_2__EvaluateSHBases(inout PS_STREAMS _streams, float3 direction)
{
    float x = direction.x;
    float y = direction.y;
    float z = direction.z;
    float x2 = x * x;
    float y2 = y * y;
    float z2 = z * z;
    _streams.SHBaseValues_id1[0] = 0.2820948064327239990234375f;
    if (true)
    {
        _streams.SHBaseValues_id1[1] = (-0.48860251903533935546875f) * y;
        _streams.SHBaseValues_id1[2] = 0.48860251903533935546875f * z;
        _streams.SHBaseValues_id1[3] = (-0.48860251903533935546875f) * x;
        if (false)
        {
            _streams.SHBaseValues_id1[4] = (1.09254848957061767578125f * y) * x;
            _streams.SHBaseValues_id1[5] = ((-1.09254848957061767578125f) * y) * z;
            _streams.SHBaseValues_id1[6] = 0.3153915703296661376953125f * ((3.0f * z2) - 1.0f);
            _streams.SHBaseValues_id1[7] = ((-1.09254848957061767578125f) * x) * z;
            _streams.SHBaseValues_id1[8] = 0.546274244785308837890625f * (x2 - y2);
            if (false)
            {
                float z3 = pow(z, 3.0f);
                float x4 = pow(x, 4.0f);
                float y4 = pow(y, 4.0f);
                float z4 = pow(z, 4.0f);
                _streams.SHBaseValues_id1[9] = ((-0.590043604373931884765625f) * y) * ((3.0f * x2) - y2);
                _streams.SHBaseValues_id1[10] = ((2.8906114101409912109375f * y) * x) * z;
                _streams.SHBaseValues_id1[11] = ((-0.6463603973388671875f) * y) * ((-1.0f) + (5.0f * z2));
                _streams.SHBaseValues_id1[12] = 0.3731763362884521484375f * ((5.0f * z3) - (3.0f * z));
                _streams.SHBaseValues_id1[13] = ((-0.4570457935333251953125f) * x) * ((-1.0f) + (5.0f * z2));
                _streams.SHBaseValues_id1[14] = (1.44530570507049560546875f * (x2 - y2)) * z;
                _streams.SHBaseValues_id1[15] = ((-0.590043604373931884765625f) * x) * (x2 - (3.0f * y2));
                if (false)
                {
                    _streams.SHBaseValues_id1[16] = ((2.5033428668975830078125f * x) * y) * (x2 - y2);
                    _streams.SHBaseValues_id1[17] = (((-1.77013075351715087890625f) * y) * z) * ((3.0f * x2) - y2);
                    _streams.SHBaseValues_id1[18] = ((0.946174681186676025390625f * y) * x) * ((-1.0f) + (7.0f * z2));
                    _streams.SHBaseValues_id1[19] = (((-0.66904652118682861328125f) * y) * z) * ((-3.0f) + (7.0f * z2));
                    _streams.SHBaseValues_id1[20] = (((105.0f * z4) - (90.0f * z2)) + 9.0f) / 28.3592624664306640625f;
                    _streams.SHBaseValues_id1[21] = (((-0.66904652118682861328125f) * x) * z) * ((-3.0f) + (7.0f * z2));
                    _streams.SHBaseValues_id1[22] = (0.4730873405933380126953125f * (x2 - y2)) * ((-1.0f) + (7.0f * z2));
                    _streams.SHBaseValues_id1[23] = (((-1.77013075351715087890625f) * x) * z) * (x2 - (3.0f * y2));
                    _streams.SHBaseValues_id1[24] = 0.625835716724395751953125f * ((x4 - ((6.0f * y2) * x2)) + y4);
                }
            }
        }
    }
}

float4 LambertianPrefilteringSHNoComputePass1_2__Shading(inout PS_STREAMS _streams)
{
    float3 result = 0.0f.xxx;
    float2 uv = (_streams.TexCoord_id0 * 2.0f) - 1.0f.xx;
    float dist = 1.0f + dot(uv, uv);
    float weight = 4.0f / (sqrt(dist) * dist);
    for (int faceIndex = 0; faceIndex < 6; faceIndex++)
    {
        float param = uv.x;
        float param_1 = uv.y;
        int param_2 = faceIndex;
        float3 dirVS = normalize(LambertianPrefilteringSHNoComputePass1_2__uvToDirectionVS(param, param_1, param_2));
        float3 param_3 = dirVS;
        SphericalHarmonicsBase_2__EvaluateSHBases(_streams, param_3);
        float3 radiance = LambertianPrefilteringSHNoComputePass1_RadianceMap.Sample(Texturing_PointSampler, dirVS).xyz;
        float indexable[25] = { 1.0f, 0.666666686534881591796875f, 0.666666686534881591796875f, 0.666666686534881591796875f, 0.25f, 0.25f, 0.25f, 0.25f, 0.25f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, -0.0416666679084300994873046875f, -0.0416666679084300994873046875f, -0.0416666679084300994873046875f, -0.0416666679084300994873046875f, -0.0416666679084300994873046875f, -0.0416666679084300994873046875f, -0.0416666679084300994873046875f, -0.0416666679084300994873046875f, -0.0416666679084300994873046875f };
        result += ((radiance * (indexable[LambertianPrefilteringSHNoComputePass1_CoefficientIndex] * _streams.SHBaseValues_id1[LambertianPrefilteringSHNoComputePass1_CoefficientIndex])) * weight);
    }
    return float4(result, weight * 6.0f);
}

void frag_main()
{
    PS_STREAMS _streams = { 0.0f.xx, { 0.0f, 0.0f, 0.0f, 0.0f }, 0.0f.xxxx, 0.0f.xxxx };
    _streams.TexCoord_id0 = PS_IN_TexCoord;
    _streams.ShadingPosition_id2 = PS_IN_ShadingPosition;
    float4 _345 = LambertianPrefilteringSHNoComputePass1_2__Shading(_streams);
    _streams.ColorTarget_id3 = _345;
    PS_OUT_ColorTarget = _streams.ColorTarget_id3;
}

SPIRV_Cross_Output main(SPIRV_Cross_Input stage_input)
{
    PS_IN_TexCoord = stage_input.PS_IN_TexCoord;
    PS_IN_ShadingPosition = stage_input.PS_IN_ShadingPosition;
    frag_main();
    SPIRV_Cross_Output stage_output;
    stage_output.PS_OUT_ColorTarget = PS_OUT_ColorTarget;
    return stage_output;
}

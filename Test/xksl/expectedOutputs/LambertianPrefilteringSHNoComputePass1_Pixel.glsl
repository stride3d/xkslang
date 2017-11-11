#version 450

struct PS_STREAMS
{
    vec2 TexCoord_id0;
    float SHBaseValues_id1[4];
    vec4 ShadingPosition_id2;
    vec4 ColorTarget_id3;
};

layout(std140) uniform globalRGroup
{
    int LambertianPrefilteringSHNoComputePass1_CoefficientIndex;
} globalRGroup_var;

uniform samplerCube SPIRV_Cross_CombinedLambertianPrefilteringSHNoComputePass1_RadianceMapTexturing_PointSampler;

layout(location = 0) in vec2 PS_IN_TexCoord;
layout(location = 1) in vec4 PS_IN_ShadingPosition;
layout(location = 0) out vec4 PS_OUT_ColorTarget;

vec3 LambertianPrefilteringSHNoComputePass1_2__uvToDirectionVS(float u, float v, int viewIndex)
{
    if (viewIndex == 0)
    {
        return vec3(1.0, -v, -u);
    }
    if (viewIndex == 1)
    {
        return vec3(-1.0, -v, u);
    }
    if (viewIndex == 2)
    {
        return vec3(u, 1.0, v);
    }
    if (viewIndex == 3)
    {
        return vec3(u, -1.0, -v);
    }
    if (viewIndex == 4)
    {
        return vec3(u, -v, 1.0);
    }
    if (viewIndex == 5)
    {
        return vec3(-u, -v, -1.0);
    }
    return vec3(0.0);
}

void SphericalHarmonicsBase_2__EvaluateSHBases(inout PS_STREAMS _streams, vec3 direction)
{
    float x = direction.x;
    float y = direction.y;
    float z = direction.z;
    float x2 = x * x;
    float y2 = y * y;
    float z2 = z * z;
    _streams.SHBaseValues_id1[0] = 0.2820948064327239990234375;
    if (true)
    {
        _streams.SHBaseValues_id1[1] = (-0.48860251903533935546875) * y;
        _streams.SHBaseValues_id1[2] = 0.48860251903533935546875 * z;
        _streams.SHBaseValues_id1[3] = (-0.48860251903533935546875) * x;
        if (false)
        {
            _streams.SHBaseValues_id1[4] = (1.09254848957061767578125 * y) * x;
            _streams.SHBaseValues_id1[5] = ((-1.09254848957061767578125) * y) * z;
            _streams.SHBaseValues_id1[6] = 0.3153915703296661376953125 * ((3.0 * z2) - 1.0);
            _streams.SHBaseValues_id1[7] = ((-1.09254848957061767578125) * x) * z;
            _streams.SHBaseValues_id1[8] = 0.546274244785308837890625 * (x2 - y2);
            if (false)
            {
                float z3 = pow(z, 3.0);
                float x4 = pow(x, 4.0);
                float y4 = pow(y, 4.0);
                float z4 = pow(z, 4.0);
                _streams.SHBaseValues_id1[9] = ((-0.590043604373931884765625) * y) * ((3.0 * x2) - y2);
                _streams.SHBaseValues_id1[10] = ((2.8906114101409912109375 * y) * x) * z;
                _streams.SHBaseValues_id1[11] = ((-0.6463603973388671875) * y) * ((-1.0) + (5.0 * z2));
                _streams.SHBaseValues_id1[12] = 0.3731763362884521484375 * ((5.0 * z3) - (3.0 * z));
                _streams.SHBaseValues_id1[13] = ((-0.4570457935333251953125) * x) * ((-1.0) + (5.0 * z2));
                _streams.SHBaseValues_id1[14] = (1.44530570507049560546875 * (x2 - y2)) * z;
                _streams.SHBaseValues_id1[15] = ((-0.590043604373931884765625) * x) * (x2 - (3.0 * y2));
                if (false)
                {
                    _streams.SHBaseValues_id1[16] = ((2.5033428668975830078125 * x) * y) * (x2 - y2);
                    _streams.SHBaseValues_id1[17] = (((-1.77013075351715087890625) * y) * z) * ((3.0 * x2) - y2);
                    _streams.SHBaseValues_id1[18] = ((0.946174681186676025390625 * y) * x) * ((-1.0) + (7.0 * z2));
                    _streams.SHBaseValues_id1[19] = (((-0.66904652118682861328125) * y) * z) * ((-3.0) + (7.0 * z2));
                    _streams.SHBaseValues_id1[20] = (((105.0 * z4) - (90.0 * z2)) + 9.0) / 28.3592624664306640625;
                    _streams.SHBaseValues_id1[21] = (((-0.66904652118682861328125) * x) * z) * ((-3.0) + (7.0 * z2));
                    _streams.SHBaseValues_id1[22] = (0.4730873405933380126953125 * (x2 - y2)) * ((-1.0) + (7.0 * z2));
                    _streams.SHBaseValues_id1[23] = (((-1.77013075351715087890625) * x) * z) * (x2 - (3.0 * y2));
                    _streams.SHBaseValues_id1[24] = 0.625835716724395751953125 * ((x4 - ((6.0 * y2) * x2)) + y4);
                }
            }
        }
    }
}

vec4 LambertianPrefilteringSHNoComputePass1_2__Shading(inout PS_STREAMS _streams)
{
    vec3 result = vec3(0.0);
    vec2 uv = (_streams.TexCoord_id0 * 2.0) - vec2(1.0);
    float dist = 1.0 + dot(uv, uv);
    float weight = 4.0 / (sqrt(dist) * dist);
    for (int faceIndex = 0; faceIndex < 6; faceIndex++)
    {
        float param = uv.x;
        float param_1 = uv.y;
        int param_2 = faceIndex;
        vec3 dirVS = normalize(LambertianPrefilteringSHNoComputePass1_2__uvToDirectionVS(param, param_1, param_2));
        vec3 param_3 = dirVS;
        SphericalHarmonicsBase_2__EvaluateSHBases(_streams, param_3);
        vec3 radiance = texture(SPIRV_Cross_CombinedLambertianPrefilteringSHNoComputePass1_RadianceMapTexturing_PointSampler, dirVS).xyz;
        float indexable[25] = float[](1.0, 0.666666686534881591796875, 0.666666686534881591796875, 0.666666686534881591796875, 0.25, 0.25, 0.25, 0.25, 0.25, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, -0.0416666679084300994873046875, -0.0416666679084300994873046875, -0.0416666679084300994873046875, -0.0416666679084300994873046875, -0.0416666679084300994873046875, -0.0416666679084300994873046875, -0.0416666679084300994873046875, -0.0416666679084300994873046875, -0.0416666679084300994873046875);
        result += ((radiance * (indexable[globalRGroup_var.LambertianPrefilteringSHNoComputePass1_CoefficientIndex] * _streams.SHBaseValues_id1[globalRGroup_var.LambertianPrefilteringSHNoComputePass1_CoefficientIndex])) * weight);
    }
    return vec4(result, weight * 6.0);
}

void main()
{
    PS_STREAMS _streams = PS_STREAMS(vec2(0.0), float[](0.0, 0.0, 0.0, 0.0), vec4(0.0), vec4(0.0));
    _streams.TexCoord_id0 = PS_IN_TexCoord;
    _streams.ShadingPosition_id2 = PS_IN_ShadingPosition;
    vec4 _345 = LambertianPrefilteringSHNoComputePass1_2__Shading(_streams);
    _streams.ColorTarget_id3 = _345;
    PS_OUT_ColorTarget = _streams.ColorTarget_id3;
}


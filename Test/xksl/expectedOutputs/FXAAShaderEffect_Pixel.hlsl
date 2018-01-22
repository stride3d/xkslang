struct PS_STREAMS
{
    float2 TexCoord_id0;
    float4 ShadingPosition_id1;
    float4 ColorTarget_id2;
};

cbuffer globalRGroup
{
    float2 Texturing_Texture0TexelSize;
};
Texture2D<float4> Texturing_Texture0;
SamplerState Texturing_LinearSampler;

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

float FXAAShader_FxaaLuma(float4 rgba)
{
    return rgba.w;
}

float4 FXAAShader_FxaaPixelShader(float2 pos, float4 fxaaConsolePosPos, Texture2D<float4> tex, Texture2D<float4> fxaaConsole360TexExpBiasNegOne, Texture2D<float4> fxaaConsole360TexExpBiasNegTwo, float2 fxaaQualityRcpFrame, float4 fxaaConsoleRcpFrameOpt, float4 fxaaConsoleRcpFrameOpt2, float4 fxaaConsole360RcpFrameOpt2, float fxaaQualitySubpix, float fxaaQualityEdgeThreshold, float fxaaQualityEdgeThresholdMin, float fxaaConsoleEdgeSharpness, float fxaaConsoleEdgeThreshold, float fxaaConsoleEdgeThresholdMin, float4 fxaaConsole360ConstDir)
{
    float2 posM;
    posM.x = pos.x;
    posM.y = pos.y;
    float4 rgbyM = tex.SampleLevel(Texturing_LinearSampler, posM, 0.0f);
    float4 param = tex.SampleLevel(Texturing_LinearSampler, posM, 0.0f, int2(0, 1));
    float lumaS = FXAAShader_FxaaLuma(param);
    float4 param_1 = tex.SampleLevel(Texturing_LinearSampler, posM, 0.0f, int2(1, 0));
    float lumaE = FXAAShader_FxaaLuma(param_1);
    float4 param_2 = tex.SampleLevel(Texturing_LinearSampler, posM, 0.0f, int2(0, -1));
    float lumaN = FXAAShader_FxaaLuma(param_2);
    float4 param_3 = tex.SampleLevel(Texturing_LinearSampler, posM, 0.0f, int2(-1, 0));
    float lumaW = FXAAShader_FxaaLuma(param_3);
    float maxSM = max(lumaS, rgbyM.w);
    float minSM = min(lumaS, rgbyM.w);
    float maxESM = max(lumaE, maxSM);
    float minESM = min(lumaE, minSM);
    float maxWN = max(lumaN, lumaW);
    float minWN = min(lumaN, lumaW);
    float rangeMax = max(maxWN, maxESM);
    float rangeMin = min(minWN, minESM);
    float rangeMaxScaled = rangeMax * fxaaQualityEdgeThreshold;
    float range = rangeMax - rangeMin;
    float rangeMaxClamped = max(fxaaQualityEdgeThresholdMin, rangeMaxScaled);
    bool earlyExit = range < rangeMaxClamped;
    float subpixRcpRange;
    float lengthSign;
    bool horzSpan;
    float subpixA;
    float subpixB;
    float lumaNN;
    float lumaSS;
    bool pairN;
    float gradient;
    if (earlyExit)
    {
        return rgbyM;
    }
    float4 param_4 = tex.SampleLevel(Texturing_LinearSampler, posM, 0.0f, int2(-1, -1));
    float lumaNW = FXAAShader_FxaaLuma(param_4);
    float4 param_5 = tex.SampleLevel(Texturing_LinearSampler, posM, 0.0f, int2(1, 1));
    float lumaSE = FXAAShader_FxaaLuma(param_5);
    float4 param_6 = tex.SampleLevel(Texturing_LinearSampler, posM, 0.0f, int2(1, -1));
    float lumaNE = FXAAShader_FxaaLuma(param_6);
    float4 param_7 = tex.SampleLevel(Texturing_LinearSampler, posM, 0.0f, int2(-1, 1));
    float lumaSW = FXAAShader_FxaaLuma(param_7);
    float lumaNS = lumaN + lumaS;
    float lumaWE = lumaW + lumaE;
    subpixRcpRange = 1.0f / range;
    float subpixNSWE = lumaNS + lumaWE;
    float edgeHorz1 = ((-2.0f) * rgbyM.w) + lumaNS;
    float edgeVert1 = ((-2.0f) * rgbyM.w) + lumaWE;
    float lumaNESE = lumaNE + lumaSE;
    float lumaNWNE = lumaNW + lumaNE;
    float edgeHorz2 = ((-2.0f) * lumaE) + lumaNESE;
    float edgeVert2 = ((-2.0f) * lumaN) + lumaNWNE;
    float lumaNWSW = lumaNW + lumaSW;
    float lumaSWSE = lumaSW + lumaSE;
    float edgeHorz4 = (abs(edgeHorz1) * 2.0f) + abs(edgeHorz2);
    float edgeVert4 = (abs(edgeVert1) * 2.0f) + abs(edgeVert2);
    float edgeHorz3 = ((-2.0f) * lumaW) + lumaNWSW;
    float edgeVert3 = ((-2.0f) * lumaS) + lumaSWSE;
    float edgeHorz = abs(edgeHorz3) + edgeHorz4;
    float edgeVert = abs(edgeVert3) + edgeVert4;
    float subpixNWSWNESE = lumaNWSW + lumaNESE;
    lengthSign = fxaaQualityRcpFrame.x;
    horzSpan = edgeHorz >= edgeVert;
    subpixA = (subpixNSWE * 2.0f) + subpixNWSWNESE;
    if (!horzSpan)
    {
        lumaN = lumaW;
    }
    if (!horzSpan)
    {
        lumaS = lumaE;
    }
    if (horzSpan)
    {
        lengthSign = fxaaQualityRcpFrame.y;
    }
    subpixB = (subpixA * 0.083333335816860198974609375f) - rgbyM.w;
    float gradientN = lumaN - rgbyM.w;
    float gradientS = lumaS - rgbyM.w;
    lumaNN = lumaN + rgbyM.w;
    lumaSS = lumaS + rgbyM.w;
    pairN = abs(gradientN) >= abs(gradientS);
    gradient = max(abs(gradientN), abs(gradientS));
    if (pairN)
    {
        lengthSign = -lengthSign;
    }
    float subpixC = clamp(abs(subpixB) * subpixRcpRange, 0.0f, 1.0f);
    float2 posB;
    posB.x = posM.x;
    posB.y = posM.y;
    float _87;
    if (!horzSpan)
    {
        _87 = 0.0f;
    }
    else
    {
        _87 = fxaaQualityRcpFrame.x;
    }
    float2 offNP;
    offNP.x = _87;
    float _88;
    float2 posN;
    float2 posP;
    float subpixD;
    float lumaEndN;
    float subpixE;
    float lumaEndP;
    float gradientScaled;
    float subpixF;
    bool lumaMLTZero;
    bool doneN;
    bool doneP;
    bool doneNP;
    float dstN;
    float dstP;
    float pixelOffsetSubpix;
    if (horzSpan)
    {
        _88 = 0.0f;
    }
    else
    {
        _88 = fxaaQualityRcpFrame.y;
    }
    offNP.y = _88;
    if (!horzSpan)
    {
        posB.x += (lengthSign * 0.5f);
    }
    if (horzSpan)
    {
        posB.y += (lengthSign * 0.5f);
    }
    posN.x = posB.x - (offNP.x * 1.0f);
    posN.y = posB.y - (offNP.y * 1.0f);
    posP.x = posB.x + (offNP.x * 1.0f);
    posP.y = posB.y + (offNP.y * 1.0f);
    subpixD = ((-2.0f) * subpixC) + 3.0f;
    float4 param_8 = tex.SampleLevel(Texturing_LinearSampler, posN, 0.0f);
    lumaEndN = FXAAShader_FxaaLuma(param_8);
    subpixE = subpixC * subpixC;
    float4 param_9 = tex.SampleLevel(Texturing_LinearSampler, posP, 0.0f);
    lumaEndP = FXAAShader_FxaaLuma(param_9);
    if (!pairN)
    {
        lumaNN = lumaSS;
    }
    gradientScaled = (gradient * 1.0f) / 4.0f;
    float lumaMM = rgbyM.w - (lumaNN * 0.5f);
    subpixF = subpixD * subpixE;
    lumaMLTZero = lumaMM < 0.0f;
    lumaEndN -= (lumaNN * 0.5f);
    lumaEndP -= (lumaNN * 0.5f);
    doneN = abs(lumaEndN) >= gradientScaled;
    doneP = abs(lumaEndP) >= gradientScaled;
    if (!doneN)
    {
        posN.x -= (offNP.x * 1.5f);
    }
    if (!doneN)
    {
        posN.y -= (offNP.y * 1.5f);
    }
    doneNP = (!doneN) || (!doneP);
    if (!doneP)
    {
        posP.x += (offNP.x * 1.5f);
    }
    if (!doneP)
    {
        posP.y += (offNP.y * 1.5f);
    }
    if (doneNP)
    {
        if (!doneN)
        {
            float4 param_10 = tex.SampleLevel(Texturing_LinearSampler, posN, 0.0f);
            lumaEndN = FXAAShader_FxaaLuma(param_10);
        }
        if (!doneP)
        {
            float4 param_11 = tex.SampleLevel(Texturing_LinearSampler, posP, 0.0f);
            lumaEndP = FXAAShader_FxaaLuma(param_11);
        }
        if (!doneN)
        {
            lumaEndN -= (lumaNN * 0.5f);
        }
        if (!doneP)
        {
            lumaEndP -= (lumaNN * 0.5f);
        }
        doneN = abs(lumaEndN) >= gradientScaled;
        doneP = abs(lumaEndP) >= gradientScaled;
        if (!doneN)
        {
            posN.x -= (offNP.x * 2.0f);
        }
        if (!doneN)
        {
            posN.y -= (offNP.y * 2.0f);
        }
        doneNP = (!doneN) || (!doneP);
        if (!doneP)
        {
            posP.x += (offNP.x * 2.0f);
        }
        if (!doneP)
        {
            posP.y += (offNP.y * 2.0f);
        }
        if (doneNP)
        {
            if (!doneN)
            {
                float4 param_12 = tex.SampleLevel(Texturing_LinearSampler, posN, 0.0f);
                lumaEndN = FXAAShader_FxaaLuma(param_12);
            }
            if (!doneP)
            {
                float4 param_13 = tex.SampleLevel(Texturing_LinearSampler, posP, 0.0f);
                lumaEndP = FXAAShader_FxaaLuma(param_13);
            }
            if (!doneN)
            {
                lumaEndN -= (lumaNN * 0.5f);
            }
            if (!doneP)
            {
                lumaEndP -= (lumaNN * 0.5f);
            }
            doneN = abs(lumaEndN) >= gradientScaled;
            doneP = abs(lumaEndP) >= gradientScaled;
            if (!doneN)
            {
                posN.x -= (offNP.x * 2.0f);
            }
            if (!doneN)
            {
                posN.y -= (offNP.y * 2.0f);
            }
            doneNP = (!doneN) || (!doneP);
            if (!doneP)
            {
                posP.x += (offNP.x * 2.0f);
            }
            if (!doneP)
            {
                posP.y += (offNP.y * 2.0f);
            }
            if (doneNP)
            {
                if (!doneN)
                {
                    float4 param_14 = tex.SampleLevel(Texturing_LinearSampler, posN, 0.0f);
                    lumaEndN = FXAAShader_FxaaLuma(param_14);
                }
                if (!doneP)
                {
                    float4 param_15 = tex.SampleLevel(Texturing_LinearSampler, posP, 0.0f);
                    lumaEndP = FXAAShader_FxaaLuma(param_15);
                }
                if (!doneN)
                {
                    lumaEndN -= (lumaNN * 0.5f);
                }
                if (!doneP)
                {
                    lumaEndP -= (lumaNN * 0.5f);
                }
                doneN = abs(lumaEndN) >= gradientScaled;
                doneP = abs(lumaEndP) >= gradientScaled;
                if (!doneN)
                {
                    posN.x -= (offNP.x * 2.0f);
                }
                if (!doneN)
                {
                    posN.y -= (offNP.y * 2.0f);
                }
                doneNP = (!doneN) || (!doneP);
                if (!doneP)
                {
                    posP.x += (offNP.x * 2.0f);
                }
                if (!doneP)
                {
                    posP.y += (offNP.y * 2.0f);
                }
                if (doneNP)
                {
                    if (!doneN)
                    {
                        float4 param_16 = tex.SampleLevel(Texturing_LinearSampler, posN, 0.0f);
                        lumaEndN = FXAAShader_FxaaLuma(param_16);
                    }
                    if (!doneP)
                    {
                        float4 param_17 = tex.SampleLevel(Texturing_LinearSampler, posP, 0.0f);
                        lumaEndP = FXAAShader_FxaaLuma(param_17);
                    }
                    if (!doneN)
                    {
                        lumaEndN -= (lumaNN * 0.5f);
                    }
                    if (!doneP)
                    {
                        lumaEndP -= (lumaNN * 0.5f);
                    }
                    doneN = abs(lumaEndN) >= gradientScaled;
                    doneP = abs(lumaEndP) >= gradientScaled;
                    if (!doneN)
                    {
                        posN.x -= (offNP.x * 8.0f);
                    }
                    if (!doneN)
                    {
                        posN.y -= (offNP.y * 8.0f);
                    }
                    doneNP = (!doneN) || (!doneP);
                    if (!doneP)
                    {
                        posP.x += (offNP.x * 8.0f);
                    }
                    if (!doneP)
                    {
                        posP.y += (offNP.y * 8.0f);
                    }
                }
            }
        }
    }
    dstN = posM.x - posN.x;
    dstP = posP.x - posM.x;
    if (!horzSpan)
    {
        dstN = posM.y - posN.y;
    }
    if (!horzSpan)
    {
        dstP = posP.y - posM.y;
    }
    bool goodSpanN = (lumaEndN < 0.0f) != lumaMLTZero;
    float spanLength = dstP + dstN;
    bool goodSpanP = (lumaEndP < 0.0f) != lumaMLTZero;
    float spanLengthRcp = 1.0f / spanLength;
    bool directionN = dstN < dstP;
    float dst = min(dstN, dstP);
    bool goodSpan = directionN ? goodSpanN : goodSpanP;
    float subpixG = subpixF * subpixF;
    float pixelOffset = (dst * (-spanLengthRcp)) + 0.5f;
    float subpixH = subpixG * fxaaQualitySubpix;
    float pixelOffsetGood = goodSpan ? pixelOffset : 0.0f;
    pixelOffsetSubpix = max(pixelOffsetGood, subpixH);
    if (!horzSpan)
    {
        posM.x += (pixelOffsetSubpix * lengthSign);
    }
    if (horzSpan)
    {
        posM.y += (pixelOffsetSubpix * lengthSign);
    }
    return float4(tex.SampleLevel(Texturing_LinearSampler, posM, 0.0f).xyz, rgbyM.w);
}

float4 FXAAShader_Shading(PS_STREAMS _streams)
{
    float2 texCoord = _streams.TexCoord_id0;
    float2 screenPixelRatio = Texturing_Texture0TexelSize;
    float2 param = texCoord;
    float4 param_1 = 0.0f.xxxx;
    float2 param_2 = screenPixelRatio;
    float4 param_3 = 0.0f.xxxx;
    float4 param_4 = 0.0f.xxxx;
    float4 param_5 = 0.0f.xxxx;
    float param_6 = 0.75f;
    float param_7 = 0.063000001013278961181640625f;
    float param_8 = 0.031199999153614044189453125f;
    float param_9 = 8.0f;
    float param_10 = 0.125f;
    float param_11 = 0.0500000007450580596923828125f;
    float4 param_12 = 0.0f.xxxx;
    return FXAAShader_FxaaPixelShader(param, param_1, Texturing_Texture0, Texturing_Texture0, Texturing_Texture0, param_2, param_3, param_4, param_5, param_6, param_7, param_8, param_9, param_10, param_11, param_12);
}

void frag_main()
{
    PS_STREAMS _streams = { 0.0f.xx, 0.0f.xxxx, 0.0f.xxxx };
    _streams.TexCoord_id0 = PS_IN_TexCoord;
    _streams.ShadingPosition_id1 = PS_IN_ShadingPosition;
    _streams.ColorTarget_id2 = FXAAShader_Shading(_streams);
    PS_OUT_ColorTarget = _streams.ColorTarget_id2;
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

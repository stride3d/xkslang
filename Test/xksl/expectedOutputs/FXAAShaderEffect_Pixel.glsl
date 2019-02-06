#version 410
#ifdef GL_ARB_shading_language_420pack
#extension GL_ARB_shading_language_420pack : require
#endif

struct PS_STREAMS
{
    vec2 TexCoord_id0;
    vec4 ShadingPosition_id1;
    vec4 ColorTarget_id2;
};

layout(std140) uniform globalRGroup
{
    vec2 Texturing_Texture0TexelSize;
} globalRGroup_var;

uniform sampler2D SPIRV_Cross_CombinedparamTexturing_LinearSampler;

in vec2 PS_IN_TEXCOORD0;
in vec4 PS_IN_SV_Position;
out vec4 PS_OUT_ColorTarget;

float FXAAShader_FxaaLuma(vec4 rgba)
{
    return rgba.w;
}

vec4 FXAAShader_FxaaPixelShader(vec2 pos, vec4 fxaaConsolePosPos, vec2 fxaaQualityRcpFrame, vec4 fxaaConsoleRcpFrameOpt, vec4 fxaaConsoleRcpFrameOpt2, vec4 fxaaConsole360RcpFrameOpt2, float fxaaQualitySubpix, float fxaaQualityEdgeThreshold, float fxaaQualityEdgeThresholdMin, float fxaaConsoleEdgeSharpness, float fxaaConsoleEdgeThreshold, float fxaaConsoleEdgeThresholdMin, vec4 fxaaConsole360ConstDir, sampler2D SPIRV_Cross_CombinedtexTexturing_LinearSampler)
{
    vec2 posM;
    posM.x = pos.x;
    posM.y = pos.y;
    vec4 rgbyM = textureLod(SPIRV_Cross_CombinedtexTexturing_LinearSampler, posM, 0.0);
    vec4 param = textureLodOffset(SPIRV_Cross_CombinedtexTexturing_LinearSampler, posM, 0.0, ivec2(0, 1));
    float lumaS = FXAAShader_FxaaLuma(param);
    vec4 param_1 = textureLodOffset(SPIRV_Cross_CombinedtexTexturing_LinearSampler, posM, 0.0, ivec2(1, 0));
    float lumaE = FXAAShader_FxaaLuma(param_1);
    vec4 param_2 = textureLodOffset(SPIRV_Cross_CombinedtexTexturing_LinearSampler, posM, 0.0, ivec2(0, -1));
    float lumaN = FXAAShader_FxaaLuma(param_2);
    vec4 param_3 = textureLodOffset(SPIRV_Cross_CombinedtexTexturing_LinearSampler, posM, 0.0, ivec2(-1, 0));
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
    if (earlyExit)
    {
        return rgbyM;
    }
    vec4 param_4 = textureLodOffset(SPIRV_Cross_CombinedtexTexturing_LinearSampler, posM, 0.0, ivec2(-1));
    float lumaNW = FXAAShader_FxaaLuma(param_4);
    vec4 param_5 = textureLodOffset(SPIRV_Cross_CombinedtexTexturing_LinearSampler, posM, 0.0, ivec2(1));
    float lumaSE = FXAAShader_FxaaLuma(param_5);
    vec4 param_6 = textureLodOffset(SPIRV_Cross_CombinedtexTexturing_LinearSampler, posM, 0.0, ivec2(1, -1));
    float lumaNE = FXAAShader_FxaaLuma(param_6);
    vec4 param_7 = textureLodOffset(SPIRV_Cross_CombinedtexTexturing_LinearSampler, posM, 0.0, ivec2(-1, 1));
    float lumaSW = FXAAShader_FxaaLuma(param_7);
    float lumaNS = lumaN + lumaS;
    float lumaWE = lumaW + lumaE;
    float subpixRcpRange = 1.0 / range;
    float subpixNSWE = lumaNS + lumaWE;
    float edgeHorz1 = ((-2.0) * rgbyM.w) + lumaNS;
    float edgeVert1 = ((-2.0) * rgbyM.w) + lumaWE;
    float lumaNESE = lumaNE + lumaSE;
    float lumaNWNE = lumaNW + lumaNE;
    float edgeHorz2 = ((-2.0) * lumaE) + lumaNESE;
    float edgeVert2 = ((-2.0) * lumaN) + lumaNWNE;
    float lumaNWSW = lumaNW + lumaSW;
    float lumaSWSE = lumaSW + lumaSE;
    float edgeHorz4 = (abs(edgeHorz1) * 2.0) + abs(edgeHorz2);
    float edgeVert4 = (abs(edgeVert1) * 2.0) + abs(edgeVert2);
    float edgeHorz3 = ((-2.0) * lumaW) + lumaNWSW;
    float edgeVert3 = ((-2.0) * lumaS) + lumaSWSE;
    float edgeHorz = abs(edgeHorz3) + edgeHorz4;
    float edgeVert = abs(edgeVert3) + edgeVert4;
    float subpixNWSWNESE = lumaNWSW + lumaNESE;
    float lengthSign = fxaaQualityRcpFrame.x;
    bool horzSpan = edgeHorz >= edgeVert;
    float subpixA = (subpixNSWE * 2.0) + subpixNWSWNESE;
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
    float subpixB = (subpixA * 0.083333335816860198974609375) - rgbyM.w;
    float gradientN = lumaN - rgbyM.w;
    float gradientS = lumaS - rgbyM.w;
    float lumaNN = lumaN + rgbyM.w;
    float lumaSS = lumaS + rgbyM.w;
    bool pairN = abs(gradientN) >= abs(gradientS);
    float gradient = max(abs(gradientN), abs(gradientS));
    if (pairN)
    {
        lengthSign = -lengthSign;
    }
    float subpixC = clamp(abs(subpixB) * subpixRcpRange, 0.0, 1.0);
    vec2 posB;
    posB.x = posM.x;
    posB.y = posM.y;
    vec2 offNP;
    offNP.x = (!horzSpan) ? 0.0 : fxaaQualityRcpFrame.x;
    offNP.y = horzSpan ? 0.0 : fxaaQualityRcpFrame.y;
    if (!horzSpan)
    {
        posB.x += (lengthSign * 0.5);
    }
    if (horzSpan)
    {
        posB.y += (lengthSign * 0.5);
    }
    vec2 posN;
    posN.x = posB.x - (offNP.x * 1.0);
    posN.y = posB.y - (offNP.y * 1.0);
    vec2 posP;
    posP.x = posB.x + (offNP.x * 1.0);
    posP.y = posB.y + (offNP.y * 1.0);
    float subpixD = ((-2.0) * subpixC) + 3.0;
    vec4 param_8 = textureLod(SPIRV_Cross_CombinedtexTexturing_LinearSampler, posN, 0.0);
    float lumaEndN = FXAAShader_FxaaLuma(param_8);
    float subpixE = subpixC * subpixC;
    vec4 param_9 = textureLod(SPIRV_Cross_CombinedtexTexturing_LinearSampler, posP, 0.0);
    float lumaEndP = FXAAShader_FxaaLuma(param_9);
    if (!pairN)
    {
        lumaNN = lumaSS;
    }
    float gradientScaled = (gradient * 1.0) / 4.0;
    float lumaMM = rgbyM.w - (lumaNN * 0.5);
    float subpixF = subpixD * subpixE;
    bool lumaMLTZero = lumaMM < 0.0;
    lumaEndN -= (lumaNN * 0.5);
    lumaEndP -= (lumaNN * 0.5);
    bool doneN = abs(lumaEndN) >= gradientScaled;
    bool doneP = abs(lumaEndP) >= gradientScaled;
    if (!doneN)
    {
        posN.x -= (offNP.x * 1.5);
    }
    if (!doneN)
    {
        posN.y -= (offNP.y * 1.5);
    }
    bool doneNP = (!doneN) || (!doneP);
    if (!doneP)
    {
        posP.x += (offNP.x * 1.5);
    }
    if (!doneP)
    {
        posP.y += (offNP.y * 1.5);
    }
    if (doneNP)
    {
        if (!doneN)
        {
            vec4 param_10 = textureLod(SPIRV_Cross_CombinedtexTexturing_LinearSampler, posN, 0.0);
            lumaEndN = FXAAShader_FxaaLuma(param_10);
        }
        if (!doneP)
        {
            vec4 param_11 = textureLod(SPIRV_Cross_CombinedtexTexturing_LinearSampler, posP, 0.0);
            lumaEndP = FXAAShader_FxaaLuma(param_11);
        }
        if (!doneN)
        {
            lumaEndN -= (lumaNN * 0.5);
        }
        if (!doneP)
        {
            lumaEndP -= (lumaNN * 0.5);
        }
        doneN = abs(lumaEndN) >= gradientScaled;
        doneP = abs(lumaEndP) >= gradientScaled;
        if (!doneN)
        {
            posN.x -= (offNP.x * 2.0);
        }
        if (!doneN)
        {
            posN.y -= (offNP.y * 2.0);
        }
        doneNP = (!doneN) || (!doneP);
        if (!doneP)
        {
            posP.x += (offNP.x * 2.0);
        }
        if (!doneP)
        {
            posP.y += (offNP.y * 2.0);
        }
        if (doneNP)
        {
            if (!doneN)
            {
                vec4 param_12 = textureLod(SPIRV_Cross_CombinedtexTexturing_LinearSampler, posN, 0.0);
                lumaEndN = FXAAShader_FxaaLuma(param_12);
            }
            if (!doneP)
            {
                vec4 param_13 = textureLod(SPIRV_Cross_CombinedtexTexturing_LinearSampler, posP, 0.0);
                lumaEndP = FXAAShader_FxaaLuma(param_13);
            }
            if (!doneN)
            {
                lumaEndN -= (lumaNN * 0.5);
            }
            if (!doneP)
            {
                lumaEndP -= (lumaNN * 0.5);
            }
            doneN = abs(lumaEndN) >= gradientScaled;
            doneP = abs(lumaEndP) >= gradientScaled;
            if (!doneN)
            {
                posN.x -= (offNP.x * 2.0);
            }
            if (!doneN)
            {
                posN.y -= (offNP.y * 2.0);
            }
            doneNP = (!doneN) || (!doneP);
            if (!doneP)
            {
                posP.x += (offNP.x * 2.0);
            }
            if (!doneP)
            {
                posP.y += (offNP.y * 2.0);
            }
            if (doneNP)
            {
                if (!doneN)
                {
                    vec4 param_14 = textureLod(SPIRV_Cross_CombinedtexTexturing_LinearSampler, posN, 0.0);
                    lumaEndN = FXAAShader_FxaaLuma(param_14);
                }
                if (!doneP)
                {
                    vec4 param_15 = textureLod(SPIRV_Cross_CombinedtexTexturing_LinearSampler, posP, 0.0);
                    lumaEndP = FXAAShader_FxaaLuma(param_15);
                }
                if (!doneN)
                {
                    lumaEndN -= (lumaNN * 0.5);
                }
                if (!doneP)
                {
                    lumaEndP -= (lumaNN * 0.5);
                }
                doneN = abs(lumaEndN) >= gradientScaled;
                doneP = abs(lumaEndP) >= gradientScaled;
                if (!doneN)
                {
                    posN.x -= (offNP.x * 2.0);
                }
                if (!doneN)
                {
                    posN.y -= (offNP.y * 2.0);
                }
                doneNP = (!doneN) || (!doneP);
                if (!doneP)
                {
                    posP.x += (offNP.x * 2.0);
                }
                if (!doneP)
                {
                    posP.y += (offNP.y * 2.0);
                }
                if (doneNP)
                {
                    if (!doneN)
                    {
                        vec4 param_16 = textureLod(SPIRV_Cross_CombinedtexTexturing_LinearSampler, posN, 0.0);
                        lumaEndN = FXAAShader_FxaaLuma(param_16);
                    }
                    if (!doneP)
                    {
                        vec4 param_17 = textureLod(SPIRV_Cross_CombinedtexTexturing_LinearSampler, posP, 0.0);
                        lumaEndP = FXAAShader_FxaaLuma(param_17);
                    }
                    if (!doneN)
                    {
                        lumaEndN -= (lumaNN * 0.5);
                    }
                    if (!doneP)
                    {
                        lumaEndP -= (lumaNN * 0.5);
                    }
                    doneN = abs(lumaEndN) >= gradientScaled;
                    doneP = abs(lumaEndP) >= gradientScaled;
                    if (!doneN)
                    {
                        posN.x -= (offNP.x * 8.0);
                    }
                    if (!doneN)
                    {
                        posN.y -= (offNP.y * 8.0);
                    }
                    doneNP = (!doneN) || (!doneP);
                    if (!doneP)
                    {
                        posP.x += (offNP.x * 8.0);
                    }
                    if (!doneP)
                    {
                        posP.y += (offNP.y * 8.0);
                    }
                }
            }
        }
    }
    float dstN = posM.x - posN.x;
    float dstP = posP.x - posM.x;
    if (!horzSpan)
    {
        dstN = posM.y - posN.y;
    }
    if (!horzSpan)
    {
        dstP = posP.y - posM.y;
    }
    bool goodSpanN = (lumaEndN < 0.0) != lumaMLTZero;
    float spanLength = dstP + dstN;
    bool goodSpanP = (lumaEndP < 0.0) != lumaMLTZero;
    float spanLengthRcp = 1.0 / spanLength;
    bool directionN = dstN < dstP;
    float dst = min(dstN, dstP);
    bool goodSpan = directionN ? goodSpanN : goodSpanP;
    float subpixG = subpixF * subpixF;
    float pixelOffset = (dst * (-spanLengthRcp)) + 0.5;
    float subpixH = subpixG * fxaaQualitySubpix;
    float pixelOffsetGood = goodSpan ? pixelOffset : 0.0;
    float pixelOffsetSubpix = max(pixelOffsetGood, subpixH);
    if (!horzSpan)
    {
        posM.x += (pixelOffsetSubpix * lengthSign);
    }
    if (horzSpan)
    {
        posM.y += (pixelOffsetSubpix * lengthSign);
    }
    return vec4(textureLod(SPIRV_Cross_CombinedtexTexturing_LinearSampler, posM, 0.0).xyz, rgbyM.w);
}

vec4 FXAAShader_Shading(PS_STREAMS _streams)
{
    vec2 texCoord = _streams.TexCoord_id0;
    vec2 screenPixelRatio = globalRGroup_var.Texturing_Texture0TexelSize;
    vec2 param = texCoord;
    vec4 param_1 = vec4(0.0);
    vec2 param_2 = screenPixelRatio;
    vec4 param_3 = vec4(0.0);
    vec4 param_4 = vec4(0.0);
    vec4 param_5 = vec4(0.0);
    float param_6 = 0.75;
    float param_7 = 0.063000001013278961181640625;
    float param_8 = 0.031199999153614044189453125;
    float param_9 = 8.0;
    float param_10 = 0.125;
    float param_11 = 0.0500000007450580596923828125;
    vec4 param_12 = vec4(0.0);
    return FXAAShader_FxaaPixelShader(param, param_1, param_2, param_3, param_4, param_5, param_6, param_7, param_8, param_9, param_10, param_11, param_12, SPIRV_Cross_CombinedparamTexturing_LinearSampler);
}

void main()
{
    PS_STREAMS _streams = PS_STREAMS(vec2(0.0), vec4(0.0), vec4(0.0));
    _streams.TexCoord_id0 = PS_IN_TEXCOORD0;
    _streams.ShadingPosition_id1 = PS_IN_SV_Position;
    _streams.ColorTarget_id2 = FXAAShader_Shading(_streams);
    PS_OUT_ColorTarget = _streams.ColorTarget_id2;
}


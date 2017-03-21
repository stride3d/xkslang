//
// Copyright (C) 

#ifndef XKSLANG_XKSL_MIXER_H__
#define XKSLANG_XKSL_MIXER_H__

#include <string>
#include <vector>

#include "define.h"
#include "SpxBytecode.h"
#include "OutputStageBytecode.h"

namespace xkslang
{

//=============================================================================================================//
//==============================================  XkslMixer  =================================================//
//============================================================================================================//
class SpxStreamRemapper;
class XkslMixer
{
public:
    XkslMixer();
    virtual ~XkslMixer();

    //TMP?
    static void StartMixin();
    static void ReleaseMixin();

    static bool GetListAllShadersFromBytecode(SpxBytecode& spxBytecode, std::vector<std::string>& vecShaderName, std::vector<std::string>& messages);

    //Mix all shaders from spirXBytecode
    bool Mixin(const SpxBytecode& spirXBytecode, std::vector<std::string>& messages);

    //Mix shaders linked to shaderName
    bool Mixin(const SpxBytecode& spirXBytecode, const std::string& shaderName, std::vector<std::string>& messages);
    bool Mixin(const SpxBytecode& spirXBytecode, const std::vector<std::string>& shaders, std::vector<std::string>& messages);

    bool AddComposition(const std::string& shaderName, const std::string& variableName, XkslMixer* mixerSource, std::vector<std::string>& messages);

    bool GetCurrentMixinBytecode(SpxBytecode& output, std::vector<std::string>& messages);

    //bool GenerateStageBytecode(ShadingStage stage, std::string entryPoint, SpvBytecode& output, std::vector<std::string>& messages);
    //bool FinalizeMixin(std::vector<std::string>& messages);

    bool Compile(std::vector<OutputStageBytecode>& outputStages, std::vector<std::string>& messages,
        SpvBytecode* composedSpv, SpvBytecode* streamsMergeSpv, SpvBytecode* streamsReshuffledSpv, SpvBytecode* mergedCBuffersSpv, SpvBytecode* processedResourcesSpv, SpvBytecode* finalSpv, SpvBytecode* errorLatestSpv);

private:
    SpxStreamRemapper* spxStreamRemapper;
};

}  // namespace xkslang

#endif  // XKSLANG_XKSL_MIXER_H__

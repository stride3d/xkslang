//
// Copyright (C) 

#ifndef XKSLANG_XKSL_MIXER_H__
#define XKSLANG_XKSL_MIXER_H__

#include <string>
#include <vector>

#include "../Common/xkslangDefine.h"
#include "../Common/SpxBytecode.h"
#include "OutputStageBytecode.h"
#include "EffectReflection.h"

namespace xkslang
{

//=============================================================================================================//
//==============================================  SpxMixer  ==================================================//
//============================================================================================================//

class SpxCompiler;
class SpxMixer
{
public:
    SpxMixer();
    virtual ~SpxMixer();

    static void StartMixinEffect();
    static void StopMixinEffect();

    static bool GetListAllShadersFromBytecode(SpxBytecode& spxBytecode, std::vector<std::string>& vecShaderName, std::vector<std::string>& messages);
    bool GetListAllCompositions(std::vector<ShaderCompositionInfo>& vecCompositions, std::vector<std::string>& messages);

    //Mix shaders linked to shaderName
    bool Mixin(const SpxBytecode& spirXBytecode, const std::string& shaderName, std::vector<std::string>& messages);
    bool Mixin(const SpxBytecode& spirXBytecode, const std::vector<std::string>& shaders, std::vector<std::string>& messages);

    bool AddComposition(const std::string& shaderName, const std::string& variableName, SpxMixer* mixerSource, std::vector<std::string>& messages);

    const std::vector<uint32_t>* GetCurrentMixinBytecode();
    bool CopyCurrentMixinBytecode(std::vector<uint32_t>& bytecode, std::vector<std::string>& messages);

    //bool GenerateStageBytecode(ShadingStage stage, std::string entryPoint, SpvBytecode& output, std::vector<std::string>& messages);
    //bool FinalizeMixin(std::vector<std::string>& messages);

    bool Compile(std::vector<OutputStageBytecode>& outputStages, std::vector<std::string>& messages,
        SpvBytecode* composedSpv, SpvBytecode* streamsMergeSpv, SpvBytecode* streamsReshuffledSpv, SpvBytecode* mergedCBuffersSpv, SpvBytecode* compiledBytecode, SpvBytecode* errorLatestSpv);

    static bool GetCompiledBytecodeReflection(SpvBytecode& compiledBytecode, EffectReflection& effectReflection, std::vector<std::string>& errorMessages);

private:
    SpxCompiler* spxCompiler;
};

}  // namespace xkslang

#endif  // XKSLANG_XKSL_MIXER_H__

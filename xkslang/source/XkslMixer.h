//
// Copyright (C) 

#ifndef XKSLANG_XKSL_MIXER_H__
#define XKSLANG_XKSL_MIXER_H__

#include <string>
#include <vector>

#include "define.h"
#include "SpxBytecode.h"

namespace xkslang
{

//=============================================================================================================//
//==============================================  XkslMixer  =================================================//
//============================================================================================================//
class SpxStreamRemapper;
class XkslMixer
{
public:
    class XkslMixerOutputStage
    {
    public:
        ShadingStageEnum stage;
        std::string entryPoint;
        SpvBytecode resultingBytecode;

        XkslMixerOutputStage(ShadingStageEnum stage, std::string entryPoint) : stage(stage), entryPoint(entryPoint){}
    };

    XkslMixer();
    virtual ~XkslMixer();

    bool Mixin(const SpxBytecode& spirXBytecode, std::vector<std::string>& messages);
    bool GetCurrentMixinBytecode(SpxBytecode& output, std::vector<std::string>& messages);

    //bool GenerateStageBytecode(ShadingStage stage, std::string entryPoint, SpvBytecode& output, std::vector<std::string>& messages);
    //bool FinalizeMixin(std::vector<std::string>& messages);

    bool Compile(std::vector<XkslMixerOutputStage>& outputStages, SpvBytecode& compiledSpv, std::vector<std::string>& messages);

private:
    SpxStreamRemapper* spxStreamRemapper;
};

}  // namespace xkslang

#endif  // XKSLANG_XKSL_MIXER_H__

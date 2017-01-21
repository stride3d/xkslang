//
// Copyright (C) 

#ifndef XKSLANG_XKSL_MIXER_H__
#define XKSLANG_XKSL_MIXER_H__

#include <string>
#include <vector>

#include "define.h"
#include "SpxBytecode.h"
//#include "SpxStreamParser.h"

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

    bool Mixin(const SpxBytecode& spirXBytecode, std::vector<std::string>& messages);
    //bool MergeAllMixin(std::vector<std::string>& messages);
    bool GetMixinBytecode(SpxBytecode& output, std::vector<std::string>& messages);
    bool GenerateStageBytecode(ShadingStage stage, std::string entryPoint, SpvBytecode& output, std::vector<std::string>& messages);

private:
    //std::vector<SpxBytecode*> listMixins;
    SpxStreamRemapper* spxStreamRemapper;
};

}  // namespace xkslang

#endif  // XKSLANG_XKSL_MIXER_H__

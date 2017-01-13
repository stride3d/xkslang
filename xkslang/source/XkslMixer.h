//
// Copyright (C) 

#ifndef XKSLANG_XKSL_MIXER_H__
#define XKSLANG_XKSL_MIXER_H__

#include <string>
#include <vector>

#include "define.h"
#include "SPXBytecode.h"
#include "SPXStreamParser.h"

namespace xkslang
{

class XkslMixer
{
public:
    XkslMixer();
    virtual ~XkslMixer();

    void AddMixin(SPXBytecode* spirXBytecode);
    bool GenerateBytecode(SPVBytecode& bytecode, ShadingStage stage, std::string entryPoint, std::vector<std::string>& messages);

private:
    std::vector<SPXBytecode*> listMixins;
    std::vector<SPXStreamParser*> listParsedSprx;

private:
    SPXStreamParser* ParseSPXBytecode(SPXBytecode* spirXBytecode, std::vector<std::string>& msgs);
};

}  // namespace xkslang

#endif  // XKSLANG_XKSL_MIXER_H__

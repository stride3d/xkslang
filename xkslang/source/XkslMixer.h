//
// Copyright (C) 

#ifndef XKSLANG_XKSLMIXER_H
#define XKSLANG_XKSLMIXER_H

#include <string>
#include <vector>

#include "define.h"
#include "SPXBytecode.h"
#include "SpirxStreamParser.h"

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
    std::vector<SpirxStreamParser*> listParsedSprx;

private:
    SpirxStreamParser* ParseSPXBytecode(SPXBytecode* spirXBytecode, std::vector<std::string>& msgs);
};

}  // namespace xkslang

#endif  // XKSLANG_XKSLMIXER_H

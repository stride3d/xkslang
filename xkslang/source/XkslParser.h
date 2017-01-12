//
// Copyright (C) 

#ifndef XKSLANG_XKSLPARSER_H
#define XKSLANG_XKSLPARSER_H

#include <tuple>
#include <string>
#include <vector>

//#include "glslang/Public/ShaderLang.h"
//#include "StandAlone/ResourceLimits.h"

#include "define.h"
#include "SPXBytecode.h"

namespace xkslang
{

class XkslParser
{
private:

public:
    XkslParser();
    ~XkslParser();

    bool InitialiseXkslang();
    void Finalize();

    bool ConvertXkslToSpirX(const std::string& shaderFileName, const std::string& shaderString, SPXBytecode& spirXBytecode);
};

}  // namespace xkslang

#endif  // XKSLANG_XKSLPARSER_H

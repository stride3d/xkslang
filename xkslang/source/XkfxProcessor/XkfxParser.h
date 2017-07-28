//
// Copyright (C) 

#ifndef XKSLANG_XKFX_PARSER_H
#define XKSLANG_XKFX_PARSER_H

#include <unordered_map>
#include <string>
#include <vector>

#include "glslang/Public/ShaderLang.h"

#include "../Common/xkslangDefine.h"
#include "../Common/SpxBytecode.h"
#include "../XkslParser/XkslParser.h"

namespace xkfxProcessor
{

class XkfxParser
{
public:
    static bool ProcessXkfxCommandLines(xkslang::XkslParser* parser, const std::string& effectCmdLines, glslang::CallbackRequestDataForShader callbackRequestDataForShader, std::vector<std::string>& errorMsgs);

    static bool IsCommandLineInstructionComplete(const std::string& instruction);
    static bool GetNextLine(const std::string& txt, std::string& line, std::string& remainingText);
    static bool GetNextInstruction(const std::string& line, std::string& firstInstruction, std::string& remainingLine);
    static bool GetNextInstruction(const std::string& line, std::string& firstInstruction, std::string& remainingLine, const char stopDelimiters, bool keepTheStopDelimiterInTheRemainingString);
    static bool SeparateAdotB(const std::string str, std::string& A, std::string& B);
    static std::string GetUnmangledName(const std::string& fullName);
    static bool SplitPametersString(const std::string& parameterStr, std::vector<std::string>& parameters);

    static xkslang::SpxBytecode* GetSpxBytecodeForShader(const std::string& shaderName, std::string& shaderFullName,
        std::unordered_map<std::string, xkslang::SpxBytecode*>& mapShaderNameBytecode, bool canLookIfUnmangledNameMatch, std::vector<std::string>& errorMsgs);
};

}  // namespace xkfxProcessor

#endif  // XKSLANG_XKFX_PARSER_H

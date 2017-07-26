//
// Copyright (C) 

#ifndef XKSLANG_XKFX_PARSER_H
#define XKSLANG_XKFX_PARSER_H

#include <string>
#include <vector>

#include "../Common/xkslangDefine.h"
#include "../Common/SpxBytecode.h"

namespace xkslang
{

class XkfxParser
{
public:
    static SpxBytecode* GetSpxBytecodeForShader(const std::string& shaderName, std::string& shaderFullName,
        std::unordered_map<std::string, SpxBytecode*>& mapShaderNameBytecode, bool canLookIfUnmangledNameMatch, std::vector<std::string>& errorMsgs);

    static bool ProcessXkfxCommandLines(std::string effectCmdLines, glslang::CallbackRequestDataForShader callbackRequestDataForShader, std::vector<std::string>& errorMsgs);

    static bool IsCommandLineInstructionComplete(const std::string& instruction);
    static bool GetNextInstruction(const std::string& line, std::string& firstInstruction, std::string& remainingLine);
    static bool GetNextInstruction(const std::string& line, std::string& firstInstruction, std::string& remainingLine, const char stopDelimiters, bool keepTheStopDelimiterInTheRemainingString);
    static bool SeparateAdotB(const std::string str, std::string& A, std::string& B);
    static std::string GetUnmangledName(const std::string& fullName);
    static bool SplitPametersString(const std::string& parameterStr, std::vector<std::string>& parameters);
};

}  // namespace xkslang

#endif  // XKSLANG_XKFX_PARSER_H

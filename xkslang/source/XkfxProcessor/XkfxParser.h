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
#include "../SpxMixer/OutputStageBytecode.h"

namespace xkfxProcessor
{

class CompositionExpression
{
public:
    std::string Target;
    std::string Expression;
    //bool IsExpressionAnOpenMissingInstruction;

    CompositionExpression() {}
    CompositionExpression(const std::string& target, const std::string& expression)
        : Target(target), Expression(expression) {}
};

class XkfxParser
{
public:
    static bool ProcessXkfxCommandLines(xkslang::XkslParser* parser, const std::string& effectCmdLines, glslang::CallbackRequestDataForShader callbackRequestDataForShader,
        std::vector<uint32_t>* compiledBytecode, std::vector<xkslang::OutputStageBytecode>& outputStages, std::vector<std::string>& errorMsgs);

    static bool GetOutputStagesEntryPointFromMethodsList(const std::vector<xkslang::MethodInfo>& vecMethods, std::vector<xkslang::OutputStageBytecode>& vecOutputStages);

    static bool IsCommandLineInstructionComplete(const char* pInstruction);
    static bool SplitLine(char* txt, char** nextLine);
    static bool GetNextWord(const char* txt, const char** nextWordStart, int* nextWordLen, const char** followingWordStart, const char additionnalStopDelimiters = ' ');
    static bool GetAndCopyNextWord(const char* txt, char* nextWordBuffer, int bufferMaxSize, int* nextWordLen, const char** followingWordStart, char additionnalStopDelimiters = ' ');
    static bool GetNextStringExpression(const char* txt, char* const outputBuffer, int bufferMaxSize, int* expressionLen);
    static bool StartWith(const char* txt, const char* word);
    static bool StartWithThenImmediatelyFollowByAny(const char* txt, const char* word, const char* charList);
    static bool getFunctionParameterString(const char* txt, const char** stringStart, int* stringLen);
    static bool SplitParametersString(const char* parameterStr, std::vector<std::string>& parameters);
    static bool SplitCompositionParametersString(const char* parameterStr, std::vector<CompositionExpression>& listCompositions, bool targetRequired, std::vector<std::string>& errorMsgs);

    static bool GetNextInstruction(const std::string& line, std::string& firstInstruction, std::string& remainingLine);
    static bool GetNextInstruction(const std::string& line, std::string& firstInstruction, std::string& remainingLine, const char stopDelimiters, bool keepTheStopDelimiterInTheRemainingString);
    static bool SeparateAdotB(const std::string str, std::string& A, std::string& B);
    static std::string GetUnmangledName(const std::string& fullName);

    static bool NormalizeShaderName(const std::string& shaderName, std::string& normalizedShaderName, std::vector<std::string>& errorMsgs);
    static xkslang::SpxBytecode* GetSpxBytecodeForShader(const std::string& shaderName, std::string& shaderFullName,
        std::unordered_map<std::string, xkslang::SpxBytecode*>& mapShaderNameBytecode, bool canLookIfUnmangledNameMatch, std::vector<std::string>& errorMsgs);
};

}  // namespace xkfxProcessor

#endif  // XKSLANG_XKFX_PARSER_H

//
// Copyright (C) 

#ifndef XKSLANG_XKSLPARSER_H
#define XKSLANG_XKSLPARSER_H

#include <string>
#include <vector>
#include <sstream>

#include "glslang/Public/ShaderLang.h"

#include "../Common/xkslangDefine.h"
#include "../Common/SpxBytecode.h"

namespace xkslang
{

class GenericValue
{
public:
    std::string label;
    std::string value;

    GenericValue() {}
    GenericValue(std::string label, std::string value) : label(label), value(value) {}
};

class ShaderGenericValues
{
public:
    std::string shaderName;
    std::vector<GenericValue> genericsValue;

    ShaderGenericValues() {}
    ShaderGenericValues(const std::string& name, const std::vector<GenericValue>& generics)
        : shaderName(name), genericsValue(generics) {}

    std::string GetShaderNameWithGenerics() const;
};

class ShaderParsingDefinition
{
public:
    std::string shaderName;
    std::vector<GenericValue> genericsValue;
    std::string compositionString;

    std::string GetShaderNameWithGenerics() const;
};

class XkslUserDefinedMacro
{
public:
    std::string macroName;
    std::string macroValue;

    XkslUserDefinedMacro() {}
    XkslUserDefinedMacro(const std::string& name, const std::string& value): macroName(name), macroValue(value) {}
};

class XkslParser
{
private:

public:
    XkslParser();
    ~XkslParser();

    bool InitialiseXkslang();
    void Finalize();

    //Recursively convert a shader into SPX bytecode
    //If the shader misses some dependencies, xkslang will query their data through the callback function
    bool ConvertShaderToSpx(const std::string shaderName, glslang::CallbackRequestDataForShader callbackRequestDataForShader,
        const std::vector<ShaderGenericValues>& listGenericsValue, const std::vector<XkslUserDefinedMacro>& listUserDefinedMacros,
        SpxBytecode& spirXBytecode, std::ostringstream* errorAndDebugMessages);

    //Convert a xksl file into a SPX bytecode
    //The shader string has to contain the shader and all its dependencies
    bool ConvertXkslFileToSpx(const std::string& shaderFileName, const std::string& data,
        const std::vector<ShaderGenericValues>& listGenericsValue, const std::vector<XkslUserDefinedMacro>& listUserDefinedMacros,
        SpxBytecode& spirXBytecode, std::ostringstream* errorAndDebugMessages);

    ///Utilities functions
    //Format: {shaders}[compositions]
    static bool ParseStringWithMixinShadersAndCompositions(const char* strMixinInstruction, std::string& mixinShaders, std::string& mixinCompositions, std::vector<std::string>& errorMsgs);
    //Format: ShaderName<generics>[compositions]
    static bool ParseStringWithShaderDefinitions(const char* strShadersWithGenerics, std::vector<ShaderParsingDefinition>& listshaderDefinition);
    static int ParseStringMacroDefinition(const char* strMacrosDefinition, std::vector<XkslUserDefinedMacro>& listMacrosDefinition, bool removeValuesQuotationMark);

    static bool ProcessBytecodeSanityCheck(const std::vector<uint32_t>& bytecode, std::vector<std::string>& errorMsgs);
};

}  // namespace xkslang

#endif  // XKSLANG_XKSLPARSER_H

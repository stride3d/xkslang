//
// Copyright (C)

#include "StandAlone/ResourceLimits.h"

#include "XkslParser.h"

using namespace std;
using namespace xkslang;

//===================================================================================================================================
//===================================================================================================================================
string ShaderGenericValues::GetShaderNameWithGenerics() const
{
    string name = shaderName;
    unsigned int size = genericsValue.size();
    if (size > 0)
    {
        name += '<';
        for (unsigned int k = 0; k < size; k++)
        {
            name += genericsValue[k].value;
            if (k < size - 1) name += ',';
        }
        name += '>';
    }
    return name;
}

string ShaderParsingDefinition::GetShaderNameWithGenerics() const
{
    string name = shaderName;
    unsigned int size = genericsValue.size();
    if (size > 0)
    {
        name += '<';
        for (unsigned int k = 0; k < size; k++)
        {
            name += genericsValue[k].value;
            if (k < size - 1) name += ',';
        }
        name += '>';
    }
    return name;
}
//===================================================================================================================================
//===================================================================================================================================
XkslParser::XkslParser()
{}

XkslParser::~XkslParser()
{}

static bool isInitialized = false;
bool XkslParser::InitialiseXkslang()
{
    if (isInitialized) return true;

    bool res = glslang::InitializeProcess();
    isInitialized = res;
    return res;
}

void XkslParser::Finalize()
{
    isInitialized = false;
    glslang::FinalizeProcess();
}

//Convert a xksl file into a SPX bytecode
//The shader string has to contain the shader and all its dependencies
bool XkslParser::ConvertXkslFileToSpx(const string& shaderFileName, const string& data,
    const vector<ShaderGenericValues>& listGenericsValue, const std::vector<XkslUserDefinedMacro>& listUserDefinedMacros,
    SpxBytecode& spirXBytecode, std::ostringstream* errorAndDebugMessages)
{
    if (!isInitialized){
        if (errorAndDebugMessages != nullptr) (*errorAndDebugMessages) << "xkslParser has not been initialized, call XkslParser::InitialiseXkslang() function first" << endl;
        return false;
    }
    bool success = false;

    //const EShLanguage kind = EShLangFragment;   //Default kind (glslang expect one, even though we're parsing generic xksl files)
    const TBuiltInResource& resources = glslang::DefaultTBuiltInResource;
    EShMessages controls = static_cast<EShMessages>(EShMsgCascadingErrors | EShMsgReadHlsl | EShMsgAST);
    controls = static_cast<EShMessages>(controls | EShMsgVulkanRules);

    //Convert ShaderGenericValues to ClassGenericValues
    vector<ClassGenericValues> listGenericsPerShader;
    {
        unsigned int count = (unsigned int)listGenericsValue.size();
        listGenericsPerShader.resize(count);
        for (unsigned int k = 0; k < count; ++k)
        {
            ClassGenericValues& sg = listGenericsPerShader[k];
            sg.targetName = listGenericsValue[k].shaderName;
            for (unsigned int g = 0; g < listGenericsValue[k].genericsValue.size(); ++g)
            {
                GenericLabelAndValue aGeneric;
                aGeneric.label = listGenericsValue[k].genericsValue[g].label;
                aGeneric.value = listGenericsValue[k].genericsValue[g].value;
                sg.genericValues.push_back(aGeneric);
            }
        }
    }

    //Convert XkslUserDefinedMacro to glslang UserDefinedMacro
    vector<UserDefinedMacro> userDefinedMacros;
    {
        unsigned int count = (unsigned int)listUserDefinedMacros.size();
        userDefinedMacros.resize(count);
        for (unsigned int k = 0; k < count; ++k)
        {
            UserDefinedMacro& m = userDefinedMacros[k];
            m.macroName = listUserDefinedMacros[k].macroName;

            //remove the \" delimiter (if any)
            const string& macroValue = listUserDefinedMacros[k].macroValue;
            int macroValueLen = macroValue.length();
            if (macroValueLen > 1 && macroValue[0] == '"' && macroValue[macroValueLen - 1] == '"')
                m.macroValue = macroValue.substr(1, macroValueLen - 2);
            else
                m.macroValue = macroValue;
        }
    }

    vector<uint32_t>& spxBytecode = spirXBytecode.getWritableBytecodeStream();
    vector<string> infoMsgs;
    vector<string> debugMsgs;
    success = glslang::ConvertXkslFileToSpx(shaderFileName, data, listGenericsPerShader, userDefinedMacros, &resources, controls, spxBytecode, &infoMsgs, &debugMsgs);

    //output debug and error messages
    if (errorAndDebugMessages != nullptr)
    {
        ostringstream& stream = *errorAndDebugMessages;
        //stream << shaderFileName << "\n";
		for (unsigned int i = 0; i < infoMsgs.size(); ++i) { stream << infoMsgs[i] << endl; }
		for (unsigned int i = 0; i < debugMsgs.size(); ++i) { stream << debugMsgs[i] << endl; }
    }

    return success;
}

//Recursively convert a shader into SPX bytecode
//If the shader requires some dependencies, xkslang will query their data through the callback function
bool XkslParser::ConvertShaderToSpx(const std::string shaderName, glslang::CallbackRequestDataForShader callbackRequestDataForShader,
    const std::vector<ShaderGenericValues>& listGenericsValue, const std::vector<XkslUserDefinedMacro>& listUserDefinedMacros,
    SpxBytecode& spirXBytecode, std::ostringstream* errorAndDebugMessages)
{
    if (!isInitialized) {
        if (errorAndDebugMessages != nullptr) (*errorAndDebugMessages) << "xkslParser has not been initialized, call XkslParser::InitialiseXkslang() function first" << endl;
        return false;
    }
    bool success = true;

    //const EShLanguage kind = EShLangFragment;   //Default kind (glslang expect one, even though we're parsing generic xksl files)
    const TBuiltInResource& resources = glslang::DefaultTBuiltInResource;
    EShMessages controls = static_cast<EShMessages>(EShMsgCascadingErrors | EShMsgReadHlsl | EShMsgAST);
    controls = static_cast<EShMessages>(controls | EShMsgVulkanRules);

    //Convert ShaderGenericValues to ClassGenericValues
    vector<ClassGenericValues> listGenericsPerShader;
    {
        unsigned int count = (unsigned int)listGenericsValue.size();
        listGenericsPerShader.resize(count);
        for (unsigned int k = 0; k < count; ++k)
        {
            ClassGenericValues& sg = listGenericsPerShader[k];
            sg.targetName = listGenericsValue[k].shaderName;
            for (unsigned int g = 0; g < listGenericsValue[k].genericsValue.size(); ++g)
            {
                GenericLabelAndValue aGeneric;
                aGeneric.label = listGenericsValue[k].genericsValue[g].label;
                aGeneric.value = listGenericsValue[k].genericsValue[g].value;
                sg.genericValues.push_back(aGeneric);
            }
        }
    }

    //Convert XkslUserDefinedMacro to glslang UserDefinedMacro
    vector<UserDefinedMacro> userDefinedMacros;
    {
        unsigned int count = (unsigned int)listUserDefinedMacros.size();
        userDefinedMacros.resize(count);
        for (unsigned int k = 0; k < count; ++k)
        {
            UserDefinedMacro& m = userDefinedMacros[k];
            m.macroName = listUserDefinedMacros[k].macroName;

            //remove the \" delimiter (if any)
            const string& macroValue = listUserDefinedMacros[k].macroValue;
            int macroValueLen = macroValue.length();
            if (macroValueLen > 1 && macroValue[0] == '"' && macroValue[macroValueLen - 1] == '"')
                m.macroValue = macroValue.substr(1, macroValueLen - 2);
            else
                m.macroValue = macroValue;
        }
    }

    vector<uint32_t>& spxBytecode = spirXBytecode.getWritableBytecodeStream();
    vector<string> infoMsgs;
    vector<string> debugMsgs;
    success = glslang::ConvertXkslShaderToSpx(shaderName, callbackRequestDataForShader, listGenericsPerShader, userDefinedMacros, &resources, controls, spxBytecode, &infoMsgs, &debugMsgs);

    //output debug and error messages
    if (errorAndDebugMessages != nullptr)
    {
        ostringstream& stream = *errorAndDebugMessages;

        for (unsigned int i = 0; i < infoMsgs.size(); ++i) {
            stream << infoMsgs[i] << endl;
        }
        for (unsigned int i = 0; i < debugMsgs.size(); ++i) {
            stream << debugMsgs[i] << endl;
        }
    }

    return success;
}

//===================================================================================================================================
//===================================================================================================================================
// Utilitiers function to parse/process some string
static string TrimString(const string& str, char c)
{
    size_t first = str.find_first_not_of(c);
    if (first == string::npos) return str;
    size_t last = str.find_last_not_of(c);
    return str.substr(first, (last - first + 1));
}

static string TrimStringFromAny(const string& str, const char* chars)
{
    size_t first = str.find_first_not_of(chars);
    if (first == string::npos) return str;
    size_t last = str.find_last_not_of(chars);
    return str.substr(first, (last - first + 1));
}

int XkslParser::ParseStringMacroDefinition(const char* strMacrosDefinition, vector<XkslUserDefinedMacro>& listMacrosDefinition, bool removeValuesQuotationMark)
{
    int countMacrosParsed = 0;
    if (strMacrosDefinition == nullptr) return countMacrosParsed;

    const char* curPtr = strMacrosDefinition;
    const char* startPtr, *endPtr;

    while (true)
    {
        while (*curPtr == ' ') curPtr++;
        if (*curPtr == '"') curPtr++;  //skip the " for the macro name
        if (*curPtr == 0) return countMacrosParsed;

        startPtr = curPtr;

        while (*curPtr != 0 && *curPtr != ' ') curPtr++;
        endPtr = curPtr - 1;
        if (*(endPtr) == '"') endPtr--;

        if (endPtr < startPtr) return countMacrosParsed;

        string macroName(startPtr, (endPtr - startPtr) + 1);
        string macroValue;

        while (*curPtr == ' ') curPtr++;

        //A macro value must start and end with "" quotation marks
        if (*curPtr == '"')
        {
            //macro value
            startPtr = curPtr;
            curPtr++;
            while (*curPtr != 0 && *curPtr != '"') curPtr++;
            if (*curPtr == 0) return -1; //a macro value should end with '"'
            endPtr = curPtr;

            if (removeValuesQuotationMark) macroValue.assign(startPtr + 1, (endPtr - startPtr) - 1);
            else macroValue.assign(startPtr, (endPtr - startPtr) + 1);
            curPtr++;
        }

        listMacrosDefinition.push_back(XkslUserDefinedMacro(macroName, macroValue));
        countMacrosParsed++;

        if (*curPtr == 0) return countMacrosParsed;
    }

    return countMacrosParsed;
}

//Format: ShaderName<generics>[compositions]
bool XkslParser::ParseStringWithShaderDefinitions(const char* strShadersWithGenerics, vector<ShaderParsingDefinition>& listshaderDefinition)
{
    if (strShadersWithGenerics == nullptr) return true;

    string txt(strShadersWithGenerics);
    int len = txt.size();
    int pos = 0, end = 0;

    while (true)
    {
        while (pos < len && txt[pos] == ' ') pos++;
        if (pos == len) return true;

        //shader name: parse until we meet ' ', '<' or '['
        end = pos;
        bool hasGenerics = false;
        bool hasCompositions = false;
        bool loop = true;
        while (loop)
        {
            if (++end == len) break;

            char c = txt[end];
            switch (c)
            {
                case '<':
                {
                    loop = false;
                    hasGenerics = true;
                    break;
                }
                case '[':
                {
                    loop = false;
                    break;
                }
                case ' ':
                {
                    loop = false;
                    break;
                }
                case ',':
                {
                    loop = false;
                    break;
                }
            }
        }

        if (end == pos) return true;

        string shaderName = txt.substr(pos, end - pos);
        shaderName = TrimString(shaderName, ' ');

        ShaderParsingDefinition shaderDefintion;
        shaderDefintion.shaderName = shaderName;

        if (hasGenerics)
        {
            //generic values
            while (true)
            {
                pos = end + 1;
                while (pos < len && txt[pos] == ' ') pos++;
                if (pos == len) return false;

                end = pos + 1;
                while (end < len && txt[end] != '>' && txt[end] != ',') end++;
                if (end == len) return false;

                string aGenericValue = txt.substr(pos, end - pos);
                aGenericValue = TrimString(aGenericValue, ' ');

                GenericValue gv("", aGenericValue);
                shaderDefintion.genericsValue.push_back(gv);

                if (txt[end] == '>') break;
            }

            end++;
        }

        while (end < len && (txt[end] == ' ' || txt[end] == ',')) end++;
        if (end < len && txt[end] == '[') hasCompositions = true;

        if (hasCompositions)
        {
            int compositionsStart = end;
            int countBrackets = 0;
            loop = true;
            while (loop)
            {
                if (++end == len) break;

                char c = txt[end];
                switch (c)
                {
                    case ']':
                    {
                        if (countBrackets == 0)
                        {
                            loop = false;
                        }
                        else countBrackets--;
                        break;
                    }
                    case '[':
                    {
                        countBrackets++;
                        break;
                    }
                }
            }
            if (end >= len) return false;

            string compositionStr = txt.substr(compositionsStart + 1, (end - compositionsStart - 1));
            shaderDefintion.compositionString = compositionStr;
            end++;
        }

        listshaderDefinition.push_back(shaderDefintion);

        pos = end;
        if (pos >= len) return true;
    }

    return true;
}
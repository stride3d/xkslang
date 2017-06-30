//
// Copyright (C)

#include "StandAlone/ResourceLimits.h"

#include "XkslParser.h"

using namespace std;
using namespace xkslang;

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
            m.macroValue = listUserDefinedMacros[k].macroValue;
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
            m.macroValue = listUserDefinedMacros[k].macroValue;
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
            stream << infoMsgs[i];
            if (i < infoMsgs.size() - 1) stream << endl;
        }
        for (unsigned int i = 0; i < debugMsgs.size(); ++i) {
            stream << debugMsgs[i];
            if (i < infoMsgs.size() - 1) stream << endl;
        }
    }

    return success;
}

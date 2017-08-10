//
// Copyright (C)

#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include <iostream>
#include <memory>
#include <string>
#include <sstream>
#include <fstream>

#include "SPIRV/doc.h"
#include "SPIRV/disassemble.h"
#include "SPIRV/GlslangToSpv.h"

#include "Utils.h"
#include "../source/Common/SpxBytecode.h"
#include "../source/Common/Converter.h"
#include "../source/Common/xkslangUtils.h"
#include "../source/XkslParser/XkslParser.h"
#include "../source/XkfxProcessor/XkfxParser.h"
#include "../source/SpxMixer/OutputStageBytecode.h"
#include "../source/SpxMixer/SpxMixer.h"
#include "../source/SpxMixer/EffectReflection.h"

#include "../source/SPIRV-Cross/spirv_cross.hpp"

//#include "vld.h"

#ifdef _DEBUG
#define WRITE_BYTECODE_ON_DISK_AFTER_EVERY_MIXIN_STEPS
//#define OUTPUT_LIST_COMPOSITIONS_AFTER_EVERY_MIXIN_STEPS
#define PROCESS_BYTECODE_SANITY_CHECK_AFTER_EVERY_MIXIN_STEPS
#endif

//#define DISPLAY_OUTPUT_AND_EXPECTED_OUTPUT_DIFFERENCES

#define ALLOW_ACCESS_TO_XKSLANG_DLL

#ifdef ALLOW_ACCESS_TO_XKSLANG_DLL
#include "../source/XkslangDLL/XkslangDLL.h"
#endif

using namespace std;
using namespace xkslangtest;
using namespace xkslang;
using namespace xkfxProcessor;

//To test single file parsing and convertion
struct XkslFilesToParseAndConvert {
    const char* fileName;
};

struct XkslShaderToRecursivelyParse {
    const char* filesPrefix;
    const char* shaderName;
    const char* VSEntryPoint;
    const char* PSEntryPoint;
};

class EffectMixerObject
{
public:
    string name;
    SpxMixer* mixer;
    uint32_t mixerHandleId;
    unordered_map<int, string> stagesEntryPoints;

    EffectMixerObject(string name, SpxMixer* mixer) : name(name), mixer(mixer), mixerHandleId(0) {}
    EffectMixerObject(string name, uint32_t mixerHandleId) : name(name), mixer(nullptr), mixerHandleId(mixerHandleId) {}
    virtual ~EffectMixerObject() { if (mixer != nullptr) delete mixer; }
};

static string inputDir = "glslang\\source\\Test\\xksl\\";
static string outputDir;
static string finalResultOutputDir;
static string expectedOutputDir;
static string invalidOutputDir;

vector<XkslFilesToParseAndConvert> vecXkslFilesToConvert = {
    //{ "shaderOnly.xksl" },
    //{ "shaderWithVariable.xksl" },
    //{ "shaderWithManyVariables.xksl" },
    //{ "manySimpleShaders.xksl" },
    //{ "simpleShaderWithFunction.xksl" },
    //{ "declarationMixOfFunctionsAndVariables.xksl" },
    //{ "2ShaderWithSameFunctionNames.xksl" },
    //{ "shaderInheritance.xksl" },
    //{ "postDeclaration.xksl" },
    //{ "classAccessor.xksl" },
    //{ "typeDeclarationOnly.xksl" },
    //{ "streamsSimple.xksl" },
    //{ "streamsWithClassAccessor.xksl" },
    //{ "shaderWithDefinedConsts.xksl" },
    //{ "shaderWithUnresolvedConsts.xksl" },
    //{ "intrisicsHlslFunctions.xksl" },
    //{ "rulesWhenCallingShaderFunctions.xksl" },
    //{ "rulesWhenCallingShaderVariables.xksl" },
    //{ "methodReferingToShaderVariable.xksl" },
    //{ "methodsWithSimpleClassAccessor.xksl" },
    //{ "testParsingCbuffers.xksl" },
    //{ "shaderWithForLoop.xksl" },
    //{ "shaderWithLoops.xksl" },
    //{ "TestComposeSimple.xksl" },
    //{ "shaderDefiningAStruct.xksl" },
    //{ "TestForEach01.xksl" },
    //{ "TestStreamSemantics.xksl" },
    //{ "testParsingResources.xksl" },
    //{ "parseShaderWithStructs01.xksl" },
    //{ "parseShaderWithStructs02.xksl" },
    //{ "parseShaderWithStructs03.xksl" },
    //{ "parseShaderWithStructs04.xksl" },
    //{ "textureAndSampler.xksl" },

    ////{ "parseGeomShader01.xksl" },
    ////{ "parseGeomShader02.xksl" },
    ////{{"shaderTexturing.xksl"}, {"", nullptr}},
    ////{{"shaderBase.xksl"}, {"", nullptr}},
    ////{{"shaderSimple.xksl"}, {"", nullptr}},
    ////{ "functionsWithIOStruct01.xksl" },
    ////{{"shaderCustomEffect.xksl"}, {"", nullptr}},
    ////{{"methodsPrototypes.xksl"}, {"", nullptr}},
};

struct XkfxEffectsToProcess {
    string effectName;
    string inputFileName;
};

//Can be parameterized from the xkfx file
static bool xkfxOptions_automaticallyTryToLoadAndConvertUnknownMixinShader = false;  //if true, when we mix an unknown shader, we will try to find, load and parse/convert this shader from our library
static bool xkfxOptions_processSampleWithXkfxLibrary = false;  //if true, we also send the xkfx file into XkfxParser library

static bool buildEffectReflection = true;
static bool processEffectWithDirectCallToXkslang = true;
static bool processEffectWithDllApi = true;
static bool processEffectWithXkfxProcessorApi = true;

vector<XkfxEffectsToProcess> vecXkfxEffectToProcess = {
    //{ "TestMixin01", "TestMixin01.xkfx" },
    //{ "TestMixin02", "TestMixin02.xkfx" },
    //{ "TestMixin03", "TestMixin03.xkfx" },
    //{ "TestMixin04", "TestMixin04.xkfx" },
    //{ "TestMixin05", "TestMixin05.xkfx" },
    //{ "TestMixin06", "TestMixin06.xkfx" },
    //{ "TestMixin07", "TestMixin07.xkfx" },
    //{ "TestMixin08", "TestMixin08.xkfx" },
    
    //{ "TestMerge01", "TestMerge01.xkfx" },
    //{ "TestMerge02", "TestMerge02.xkfx" },
    //{ "TestMerge03", "TestMerge03.xkfx" },
    //{ "TestMerge04", "TestMerge04.xkfx" },
    //{ "TestMerge05", "TestMerge05.xkfx" },
    //{ "TestMerge06", "TestMerge06.xkfx" },
    //{ "TestMerge07", "TestMerge07.xkfx" },
    //{ "TestMerge08", "TestMerge08.xkfx" },
    //{ "TestMerge09", "TestMerge09.xkfx" },
    //{ "TestMerge10", "TestMerge10.xkfx" },
    //{ "TestMerge11", "TestMerge11.xkfx" },
    //{ "TestMerge12", "TestMerge12.xkfx" },
    //{ "TestMerge13", "TestMerge13.xkfx" },
    
    //{ "TestCompose02", "TestCompose02.xkfx" },
    //{ "TestCompose03", "TestCompose03.xkfx" },
    //{ "TestCompose04", "TestCompose04.xkfx" },
    //{ "TestCompose05", "TestCompose05.xkfx" },
    //{ "TestCompose06", "TestCompose06.xkfx" },
    //{ "TestCompose07", "TestCompose07.xkfx" },
    //{ "TestCompose08", "TestCompose08.xkfx" },
    //{ "TestCompose09", "TestCompose09.xkfx" },
    //{ "TestCompose09a", "TestCompose09a.xkfx" },
    //{ "TestCompose10", "TestCompose10.xkfx" },
    //{ "TestCompose11", "TestCompose11.xkfx" },
    //{ "TestCompose12", "TestCompose12.xkfx" },
    //{ "TestCompose13", "TestCompose13.xkfx" },
    //{ "TestCompose14", "TestCompose14.xkfx" },
    //{ "TestCompose15", "TestCompose15.xkfx" },
    //{ "TestCompose16", "TestCompose16.xkfx" },
    //{ "TestCompose17", "TestCompose17.xkfx" },
    //{ "TestCompose18", "TestCompose18.xkfx" },
    //{ "TestCompose19", "TestCompose19.xkfx" },
    //{ "TestCompose20", "TestCompose20.xkfx" },
    //{ "TestCompose21", "TestCompose21.xkfx" },
    //{ "TestCompose22", "TestCompose22.xkfx" },
    //{ "TestCompose23", "TestCompose23.xkfx" },
    //{ "TestCompose24", "TestCompose24.xkfx" },
    //{ "TestCompose25", "TestCompose25.xkfx" },
    //{ "TestCompose26", "TestCompose26.xkfx" },
    //{ "TestCompose27", "TestCompose27.xkfx" },
    //{ "TestCompose28", "TestCompose28.xkfx" },
    //{ "TestCompose29", "TestCompose29.xkfx" },
    //{ "TestCompose30", "TestCompose30.xkfx" },
    //{ "TestCompose31", "TestCompose31.xkfx" },
    //{ "TestCompose32", "TestCompose32.xkfx" },
    //{ "TestCompose33", "TestCompose33.xkfx" },
    //{ "TestCompose34", "TestCompose34.xkfx" },

    //{ "TestForLoop", "TestForLoop.xkfx" },
    //{ "TestForEach01", "TestForEach01.xkfx" },
    //{ "TestForEach02", "TestForEach02.xkfx" },
    //{ "TestForEach03", "TestForEach03.xkfx" },
    //{ "TestForEach04", "TestForEach04.xkfx" },
    //{ "TestForEachCompose01", "TestForEachCompose01.xkfx" },
    //{ "TestForEachCompose02", "TestForEachCompose02.xkfx" },
    //{ "TestForEachCompose03", "TestForEachCompose03.xkfx" },
    //{ "TestMergeStreams01", "TestMergeStreams01.xkfx" },
    //{ "TestMergeStreams02", "TestMergeStreams02.xkfx" },
    //{ "TestMergeStreams03", "TestMergeStreams03.xkfx" },
    
    //{ "TestReshuffleStreams01", "TestReshuffleStreams01.xkfx" },
    //{ "TestReshuffleStreams02", "TestReshuffleStreams02.xkfx" },
    //{ "TestReshuffleStreams03", "TestReshuffleStreams03.xkfx" },
    //{ "TestReshuffleStreams04", "TestReshuffleStreams04.xkfx" },
    //{ "TestReshuffleStreams05", "TestReshuffleStreams05.xkfx" },
    //{ "TestReshuffleStreams06", "TestReshuffleStreams06.xkfx" },
    //{ "TestReshuffleStreams07", "TestReshuffleStreams07.xkfx" },
    
    //{ "TestGenerics01", "TestGenerics01.xkfx" },
    //{ "TestGenerics02", "TestGenerics02.xkfx" },
    //{ "TestGenerics03", "TestGenerics03.xkfx" },
    //{ "TestGenerics04", "TestGenerics04.xkfx" },
    //{ "TestGenerics05", "TestGenerics05.xkfx" },
    //{ "TestGenerics06", "TestGenerics06.xkfx" },
    //{ "TestGenerics07", "TestGenerics07.xkfx" },
    //{ "TestGenerics08", "TestGenerics08.xkfx" },
    //{ "TestGenerics09", "TestGenerics09.xkfx" },
    //{ "TestGenerics10", "TestGenerics10.xkfx" },
    //{ "TestGenerics11", "TestGenerics11.xkfx" },
    
    //{ "CBuffer01", "CBuffer01.xkfx" },
    //{ "CBuffer02", "CBuffer02.xkfx" },
    //{ "CBuffer03", "CBuffer03.xkfx" },
    //{ "CBuffer04", "CBuffer04.xkfx" },
    //{ "CBuffer05", "CBuffer05.xkfx" },
    //{ "CBuffer06", "CBuffer06.xkfx" },
    //{ "CBuffer07", "CBuffer07.xkfx" },
    //{ "CBuffer08", "CBuffer08.xkfx" },
    //{ "CBuffer09", "CBuffer09.xkfx" },
    //{ "CBuffer10", "CBuffer10.xkfx" },
    //{ "CBuffer11", "CBuffer11.xkfx" },
    //{ "CBufferSubpart01", "CBufferSubpart01.xkfx" },
    
    //{ "ShaderWithResources01", "ShaderWithResources01.xkfx" },
    //{ "ShaderWithResources02", "ShaderWithResources02.xkfx" },
    //{ "ShaderWithResources03", "ShaderWithResources03.xkfx" },
    //{ "ShaderWithResources04", "ShaderWithResources04.xkfx" },
    //{ "ShaderWithResources05", "ShaderWithResources05.xkfx" },
    //{ "ShaderWithResources06", "ShaderWithResources06.xkfx" },
    /////////////////////////{ "ShaderWithResources07", "ShaderWithResources07.xkfx" },  //SPIRV-Cross crash
    //{ "ShaderWithResources08", "ShaderWithResources08.xkfx" },
    
    //{ "testDependency01", "testDependency01.xkfx" },
    //{ "testDependency02", "testDependency02.xkfx" },
    //{ "testDependency03", "testDependency03.xkfx" },
    //{ "testDependency04", "testDependency04.xkfx" },
    //{ "testDependency05", "testDependency05.xkfx" },
    //{ "testDependency06", "testDependency06.xkfx" },
    //{ "testDependency07", "testDependency07.xkfx" },
    
    //{ "EffectReflection01", "EffectReflection01.xkfx" },
    //{ "EffectReflection02", "EffectReflection02.xkfx" },
    //{ "EffectReflection03", "EffectReflection03.xkfx" },
    //{ "EffectReflection04", "EffectReflection04.xkfx" },
    //{ "EffectReflection05", "EffectReflection05.xkfx" },
    //{ "EffectReflection06", "EffectReflection06.xkfx" },
	//{ "EffectReflection07", "EffectReflection07.xkfx" },
    
    //{ "SemanticTest01", "SemanticTest01.xkfx" },
    //{ "testTypeSize", "testTypeSize.xkfx" },
    //{ "namespaces01", "namespaces01.xkfx" },
    //{ "testMacro01", "testMacro01.xkfx" },
    //{ "testVarKeyword01", "testVarKeyword01.xkfx" },
    //{ "userCustomType01", "userCustomType01.xkfx" },
    //{ "userCustomType02", "userCustomType02.xkfx" },
    //{ "userCustomType03", "userCustomType03.xkfx" },
    //{ "TestLink01", "TestLink01.xkfx" },
    //{ "TestLink02", "TestLink02.xkfx" },
    //{ "TestLink03", "TestLink03.xkfx" },
    //{ "TestMemberName01", "TestMemberName01.xkfx" },
    //{ "TestMemberName02", "TestMemberName02.xkfx" },
    //{ "TestMemberName03", "TestMemberName03.xkfx" },
    //{ "TestMemberName04", "TestMemberName04.xkfx" },
    //{ "TestMemberName05", "TestMemberName05.xkfx" },
    //{ "TestSemanticType01", "TestSemanticType01.xkfx" },
    //{ "TestSemanticType02", "TestSemanticType02.xkfx" },
    //{ "functionsFinding01", "functionsFinding01.xkfx" },
    //{ "mixCustomTypeAndCompose01", "mixCustomTypeAndCompose01.xkfx" },
    //{ "cbufferMembersNaming01", "cbufferMembersNaming01.xkfx" },

    //{ "ShadingBase", "ShadingBase.xkfx" },
    { "LuminanceLogShader", "LuminanceLogShader.xkfx" },
    //{ "CustomEffect", "CustomEffect.xkfx" },
    //{ "BackgroundShader", "BackgroundShader.xkfx" },
    //{ "ComputeColorWave", "ComputeColorWave.xkfx" },
    //{ "ComputeColorMultiply", "ComputeColorMultiply.xkfx" },
    //{ "TransformationBase", "TransformationBase.xkfx" },
    //{ "TransformationWAndVP", "TransformationWAndVP.xkfx" },
    //{ "DirectLightGroupArray", "DirectLightGroupArray.xkfx" },
    //{ "MaterialSurfaceStageCompositor", "MaterialSurfaceStageCompositor.xkfx" },
    //{ "NormalFromNormalMapping", "NormalFromNormalMapping.xkfx" },
    //{ "LightDirectionalGroup", "LightDirectionalGroup.xkfx" },
    //{ "DynamicTexture", "DynamicTexture.xkfx" },
    //{ "DynamicTexture01", "DynamicTexture01.xkfx" },
    //{ "DynamicTextureStream", "DynamicTextureStream.xkfx" },
    //{ "ComputeColorTextureScaledOffsetDynamicSampler", "ComputeColorTextureScaledOffsetDynamicSampler.xkfx" },
    //{ "MaterialSurfaceArray01", "MaterialSurfaceArray01.xkfx" },
    //{ "ComputeColorWaveNormal", "ComputeColorWaveNormal.xkfx" },
    //{ "MaterialSurfaceShadingSpecularMicrofacet", "MaterialSurfaceShadingSpecularMicrofacet.xkfx" },
    //{ "MaterialSurfaceLightingAndShading", "MaterialSurfaceLightingAndShading.xkfx" },
    //{ "MaterialSurfaceArray02", "MaterialSurfaceArray02.xkfx" },
    //{ "MaterialSurfaceArray03", "MaterialSurfaceArray03.xkfx" },
    //{ "MaterialSurfacePixelStageCompositor", "MaterialSurfacePixelStageCompositor.xkfx" },

    //{ "XenkoForwardShadingEffect", "XenkoForwardShadingEffect.xkfx" },
};

enum class ShaderLanguageEnum
{
    GlslLanguage,
    HlslLanguage
};

static bool error(const string& msg)
{
    cout << msg << endl;
    return false;
}

static bool ConvertAndWriteBytecodeToHlsl(const string& spvFile, const  string& outputFile)
{
    pair<bool, vector<uint32_t>> result = Utils::ReadSpvBinaryFile(spvFile);
    if (!result.first){
        error(" Failed to open the SPV file: \"" + spvFile + "\"");
        return false;
    }
    const vector<uint32_t>& bytecode = result.second;
    string hlslShader;
    int shaderModel = 40;
    bool res = Converter::ConvertBytecodeToHlsl(bytecode, shaderModel, hlslShader);

    if (res)
    {
        xkslangtest::Utils::WriteFile(outputFile, hlslShader);
        std::cout << " output: \"" << outputFile << "\"" << endl;
    }

    return res;
}

static bool ConvertAndWriteBytecodeToGlsl(const string& spvFile, const  string& outputFile)
{
    pair<bool, vector<uint32_t>> result = Utils::ReadSpvBinaryFile(spvFile);
    if (!result.first) {
        error(" Failed to open the SPV file: \"" + spvFile + "\"");
        return false;
    }
    const vector<uint32_t>& bytecode = result.second;
    string glslShader;

    bool res = Converter::ConvertBytecodeToGlsl(bytecode, glslShader);
    if (res)
    {
        xkslangtest::Utils::WriteFile(outputFile, glslShader);
        std::cout << " output: \"" << outputFile << "\"" << endl;
    }

    return res;
}

static BOOL DirectoryExists(LPCTSTR szPath)
{
    DWORD dwAttrib = GetFileAttributes(szPath);
    return (dwAttrib != INVALID_FILE_ATTRIBUTES && (dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}

static bool SetupTestDirectories()
{
    //Get app directories
    WCHAR fileName[MAX_PATH];
    int bytes = GetModuleFileName(NULL, fileName, MAX_PATH);
    wstring ws(fileName);
    string fullName(ws.begin(), ws.end());

    //WARNING: UNCLEAN AND HARDCODED FOR NOW!!
    const size_t pos = fullName.find("glslang");
    if (pos >= fullName.size()) return false;

    string dir = fullName.substr(0, pos);
    inputDir = dir + inputDir;

    outputDir = inputDir + string("outputs\\");
    finalResultOutputDir = outputDir + string("finals\\");
    expectedOutputDir = inputDir + string("expectedOutputs\\");
    invalidOutputDir = inputDir + string("invalidOutputs\\");

    {
        std::wstring wsDir(outputDir.length(), L' ');
        std::copy(outputDir.begin(), outputDir.end(), wsDir.begin());
        if (!DirectoryExists(wsDir.c_str())) CreateDirectory(wsDir.c_str(), NULL);
    }
    {
        std::wstring wsDir(finalResultOutputDir.length(), L' ');
        std::copy(finalResultOutputDir.begin(), finalResultOutputDir.end(), wsDir.begin());
        if (!DirectoryExists(wsDir.c_str())) CreateDirectory(wsDir.c_str(), NULL);
    }
    {
        std::wstring wsDir(expectedOutputDir.length(), L' ');
        std::copy(expectedOutputDir.begin(), expectedOutputDir.end(), wsDir.begin());
        if (!DirectoryExists(wsDir.c_str())) CreateDirectory(wsDir.c_str(), NULL);
    }
    {
        std::wstring wsDir(invalidOutputDir.length(), L' ');
        std::copy(invalidOutputDir.begin(), invalidOutputDir.end(), wsDir.begin());
        if (!DirectoryExists(wsDir.c_str())) CreateDirectory(wsDir.c_str(), NULL);
    }

    return true;
}

enum class BytecodeFileFormat
{
    Binary,
    Text,
};

static void WriteBytecode(const vector<uint32_t>& bytecode, const string& outputDir, const string& outputFileName, BytecodeFileFormat format)
{
    const string outputFullName = outputDir + outputFileName;

    if (bytecode.size() > 0)
    {
        if (format == BytecodeFileFormat::Binary)
        {
            //spv
            glslang::OutputSpvBin(bytecode, outputFullName.c_str());
        }
        else if (format == BytecodeFileFormat::Text)
        {
            //hr spv
            ostringstream disassembly_stream;
            spv::Parameterize();
            spv::Disassemble(disassembly_stream, bytecode);

            xkslangtest::Utils::WriteFile(outputFullName, disassembly_stream.str());
        }
        std::cout << " output: \"" << outputFileName << "\"" << endl;
    }
}

static bool displayListOfAllMethodsForTheMixer(SpxMixer* mixer)
{
    if (mixer == nullptr) return true;

    vector<string> errorMsgs;
    vector<MethodInfo> vecMethods;
    bool success = mixer->GetListAllMethodsInfo(vecMethods, errorMsgs);
    if (!success) return error("Failed to get the list of all methods from the mixer");
    if (vecMethods.size() > 0)
    {
        std::cout << endl;
        std::cout << "Count Methods: " << vecMethods.size() << endl;
        for (unsigned int c = 0; c < vecMethods.size(); c++)
        {
            MethodInfo& method = vecMethods[c];
            std::cout << " name:" << method.Name << " shader:" << method.ShaderClassName << " stage:" << (method.IsStage?"true":"false") << endl;
        }
        std::cout << endl;
    }

    return true;
}

static bool displayListOfAllCompositionsForTheMixer(SpxMixer* mixer)
{
    if (mixer == nullptr) return true;

    vector<string> errorMsgs;
    vector<ShaderCompositionInfo> vecCompositions;
    bool success = mixer->GetListAllCompositionsInfo(vecCompositions, errorMsgs);
    if (!success) return error("Failed to get the list of all compositions from the mixer");
    if (vecCompositions.size() > 0)
    {
        std::cout << endl;
        std::cout << "Count Compositions: " << vecCompositions.size() << endl;
        for (unsigned int c = 0; c < vecCompositions.size(); c++)
        {
            ShaderCompositionInfo& composition = vecCompositions[c];
            std::cout << " " << composition.CompositionShaderType << " " << composition.ShaderOwner << "."
                << composition.CompositionVariableName << (composition.IsArray ? "[]" : "") << " (instances=" << composition.CompositionCountInstances << ")"
                << " (overridenBy:" << composition.OverridenBy << ")" << endl;
        }
        std::cout << endl;
    }

    return true;
}

static bool GetMixerCurrentBytecode(EffectMixerObject* mixerTarget, vector<uint32_t>& mixinBytecode, bool useXkslangDll)
{
    if (useXkslangDll)
    {
#ifdef ALLOW_ACCESS_TO_XKSLANG_DLL
        int bytecodeLength = 0;
        uint32_t* pBytecodeBuffer = xkslangDll::GetMixerCurrentBytecode(mixerTarget->mixerHandleId, &bytecodeLength);

        if (pBytecodeBuffer == nullptr || bytecodeLength < 0)
        {
            char* pError = xkslangDll::GetErrorMessages();
            if (pError != nullptr) error(pError);
            GlobalFree(pError);

            error("failed to get the mixer current bytecode");
            return false;
        }

        mixinBytecode.assign(pBytecodeBuffer, pBytecodeBuffer + bytecodeLength);
        GlobalFree(pBytecodeBuffer);
#else
        return error("Unauthorized access to DLL functions");
#endif
    }
    else
    {
        vector<string> errorMsgs;
        bool canGetBytecode = mixerTarget->mixer->CopyCurrentMixinBytecode(mixinBytecode, errorMsgs);
        if (!canGetBytecode)
        {
            error("failed to get the mixer current bytecode");
            return false;
        }
    }

    return true;
}

static bool GetAndWriteMixerCurrentBytecode(EffectMixerObject* mixerTarget, const string& outputFileName, bool useXkslangDll)
{
    vector<uint32_t> mixinBytecode;
    if (!GetMixerCurrentBytecode(mixerTarget, mixinBytecode, useXkslangDll)) {
        error("failed to get the mixer current bytecode");
        return false;
    }
    WriteBytecode(mixinBytecode, outputDir, outputFileName, BytecodeFileFormat::Text);
    return true;
}

static bool OutputAndCheckOutputStagesCompiledBytecode(const string& effectName, vector<OutputStageBytecode>& outputStages, EffectReflection* effectReflection)
{
    DWORD time_before, time_after;
    bool success = true;

    string glslAllOutputs;
    string hlslAllOutputs;
    bool someExpectedOutputsDifferent = false;
    bool someExpectedOutputsAreMissing = false;
    for (unsigned int i = 0; i < outputStages.size(); ++i)
    {
        string labelStage = GetShadingStageLabel(outputStages[i].stage);
        std::cout << "Convert SPIRV bytecode for entry point=\"" << outputStages[i].entryPointName << "\" stage=\"" << labelStage << "\"" << endl;

        ///Save the SPIRV bytecode (and its HR form)
        string outputFileNameSpv = effectName + "_" + labelStage + ".spv";
        string outputFullnameSpv = outputDir + outputFileNameSpv;
        WriteBytecode(outputStages[i].resultingBytecode.getBytecodeStream(), outputDir, outputFileNameSpv, BytecodeFileFormat::Binary);
        WriteBytecode(outputStages[i].resultingBytecode.getBytecodeStream(), outputDir, effectName + "_" + labelStage + ".hr.spv", BytecodeFileFormat::Text);

        //======================================================================================================
        //convert back the SPIRV bytecode into GLSL / HLSL
        {
            {
                glslAllOutputs += "\n";
                glslAllOutputs += "//=============================\n";
                glslAllOutputs += "//" + labelStage + " Stage\n";
                glslAllOutputs += "//=============================\n";

                string fileNameGlsl = effectName + "_" + labelStage + ".glsl";
                string fullNameGlsl = outputDir + fileNameGlsl;
                std::cout << labelStage << " stage: Convert into GLSL." << endl;
                time_before = GetTickCount();
                //int result = ConvertSpvToShaderLanguage(outputFullnameSpv, fullNameGlsl, ShaderLanguageEnum::GlslLanguage);
                success = ConvertAndWriteBytecodeToGlsl(outputFullnameSpv, fullNameGlsl);
                time_after = GetTickCount();

                if (success) std::cout << " OK. time: " << (time_after - time_before) << "ms" << endl;
                else error(" Failed to convert the SPIRV file to GLSL");

                //======================================================================================================
                //compare the glsl output with the expected output
                if (success)
                {
                    string glslExpectedOutput;
                    string glslConvertedOutput;
                    if (Utils::ReadFile(fullNameGlsl, glslConvertedOutput))
                    {
                        glslAllOutputs += glslConvertedOutput;

                        string expectedOutputFullNameGlsl = expectedOutputDir + fileNameGlsl;
                        if (Utils::ReadFile(expectedOutputFullNameGlsl, glslExpectedOutput))
                        {
                            if (glslExpectedOutput.compare(glslConvertedOutput) != 0) {
#ifdef DISPLAY_OUTPUT_AND_EXPECTED_OUTPUT_DIFFERENCES
                                std::cout << "expected output:" << endl << glslExpectedOutput;
                                std::cout << "output:" << endl << glslConvertedOutput;
#endif
                                std::cout << " Glsl output and expected output are different !!!" << endl;

                                someExpectedOutputsDifferent = true;

                                //copy the invalid output in the folder
                                xkslangtest::Utils::WriteFile(invalidOutputDir + fileNameGlsl, glslConvertedOutput);
                            }
                            else {
                                std::cout << " GLSL output VS expected output: OK" << endl;
                            }
                        }
                        else {
                            std::cout << " !!! Warning: No expected output file for: " << fileNameGlsl << endl;
                            someExpectedOutputsAreMissing = true;
                        }
                    }
                    else {
                        error(" Failed to read the file: " + fileNameGlsl);
                        success = false;
                    }
                }
            }

            {
                hlslAllOutputs += "\n";
                hlslAllOutputs += "//=============================\n";
                hlslAllOutputs += "//" + labelStage + " Stage\n";
                hlslAllOutputs += "//=============================\n";

                string fileNameHlsl = effectName + "_" + labelStage + ".hlsl";
                string fullNameHlsl = outputDir + fileNameHlsl;
                std::cout << labelStage << " stage: Convert into HLSL." << endl;
                time_before = GetTickCount();
                //int result = ConvertSpvToShaderLanguage(outputFullnameSpv, fullNameHlsl, ShaderLanguageEnum::HlslLanguage);
                success = ConvertAndWriteBytecodeToHlsl(outputFullnameSpv, fullNameHlsl);
                time_after = GetTickCount();

                if (success) std::cout << " OK. time: " << (time_after - time_before) << "ms" << endl;
                else error(" Failed to convert the SPIRV file to HLSL");

                //======================================================================================================
                //compare the hlsl output with the expected output
                if (success)
                {
                    string hlslExpectedOutput;
                    string hlslConvertedOutput;
                    if (Utils::ReadFile(fullNameHlsl, hlslConvertedOutput))
                    {
                        hlslAllOutputs += hlslConvertedOutput;

                        string expectedOutputFullNameHlsl = expectedOutputDir + string(fileNameHlsl);
                        if (Utils::ReadFile(expectedOutputFullNameHlsl, hlslExpectedOutput))
                        {
                            if (hlslExpectedOutput.compare(hlslConvertedOutput) != 0) {
#ifdef DISPLAY_OUTPUT_AND_EXPECTED_OUTPUT_DIFFERENCES
                                std::cout << "expected output:" << endl << hlslExpectedOutput;
                                std::cout << "output:" << endl << hlslConvertedOutput;
#endif
                                std::cout << " Hlsl output and expected output are different !!!" << endl;
                                someExpectedOutputsDifferent = true;

                                //copy the invalid output in the folder
                                xkslangtest::Utils::WriteFile(invalidOutputDir + fileNameHlsl, hlslConvertedOutput);
                            }
                            else {
                                std::cout << " HLSL output VS expected output: OK" << endl;
                            }
                        }
                        else {
                            std::cout << " !!! Warning: No expected output file for: " << fileNameHlsl << endl;
                            someExpectedOutputsAreMissing = true;
                        }
                    }
                    else {
                        error(" Failed to read the file: " + fileNameHlsl);
                        success = false;
                    }
                }
            }
            //======================================================================================================
        }
    }

    string reflectionTxt = "";
    if (effectReflection != nullptr)
    {
        reflectionTxt = effectReflection->Print();

        //save the reflection data
        string fileNameReflect = effectName + "_reflection" + ".txt";
        string fullnameReflect = outputDir + fileNameReflect;
        xkslangtest::Utils::WriteFile(fullnameReflect, reflectionTxt);
        std::cout << " output: \"" << fullnameReflect << "\"" << endl;

        //if the reflection file exists in the expected folder, compare them
        string reflectionExpectedOutput;
        string reflectionLatestOutput;
        if (Utils::ReadFile(fullnameReflect, reflectionLatestOutput))
        {
            string expectedOutputFullNameReflect = expectedOutputDir + string(fileNameReflect);
            if (Utils::ReadFile(expectedOutputFullNameReflect, reflectionExpectedOutput))
            {
                if (reflectionExpectedOutput.compare(reflectionLatestOutput) != 0) {
#ifdef DISPLAY_OUTPUT_AND_EXPECTED_OUTPUT_DIFFERENCES
                    std::cout << "expected output:" << endl << reflectionExpectedOutput;
                    std::cout << "output:" << endl << reflectionLatestOutput;
#endif
                    std::cout << " Reflection: output and expected output are different !!!" << endl;
                    someExpectedOutputsDifferent = true;

                    //copy the invalid output in the folder
                    xkslangtest::Utils::WriteFile(invalidOutputDir + fileNameReflect, reflectionLatestOutput);
                }
                else {
                    std::cout << " Reflection: output VS expected output: OK" << endl;
                }
            }
            else {
                //No output, nothing to compare
            }
        }
        else {
            error(" Failed to read the file: " + fileNameReflect);
            success = false;
        }
    }

    //Save full data
    if (glslAllOutputs.size() > 0)
    {
        if (reflectionTxt.size() > 0) glslAllOutputs = "/*\n" + effectReflection->Print() + "*/\n\n" + glslAllOutputs;
        string fileNameAllGlsl = effectName + ".glsl";
        string fullNameAllGlsl = finalResultOutputDir + fileNameAllGlsl;
        xkslangtest::Utils::WriteFile(fullNameAllGlsl, glslAllOutputs);
        std::cout << " output: \"" << fullNameAllGlsl << "\"" << endl;
    }
    if (hlslAllOutputs.size() > 0)
    {
        if (reflectionTxt.size() > 0) hlslAllOutputs = "/*\n" + effectReflection->Print() + "*/\n\n" + hlslAllOutputs;
        string fileNameAllHlsl = effectName + ".hlsl";
        string fullNameAllHlsl = finalResultOutputDir + fileNameAllHlsl;
        xkslangtest::Utils::WriteFile(fullNameAllHlsl, hlslAllOutputs);
        std::cout << " output: \"" << fullNameAllHlsl << "\"" << endl;
    }

    if (someExpectedOutputsDifferent) success = false;
    if (someExpectedOutputsAreMissing) success = false;

    return success;
}

static string ConvertCharPtrToString(const char* ptr)
{
    if (ptr == nullptr) return string("");
    return string(ptr);
}

static bool ConvertAndReleaseDllStructMemberDataToReflectionStructMemberType(
    const xkslangDll::ConstantBufferMemberReflectionDescriptionData& structMemberSrc, TypeMemberReflectionDescription& structMemberDst)
{
    if (structMemberSrc.KeyName == nullptr) return error("struct member src keyname is null");

    structMemberDst.Name = ConvertCharPtrToString(structMemberSrc.KeyName);
    structMemberDst.Offset = structMemberSrc.Offset;

    structMemberDst.Type.Set(
        0,
        structMemberSrc.Class,
        structMemberSrc.Type,
        structMemberSrc.RowCount,
        structMemberSrc.ColumnCount,
        structMemberSrc.Size,
        structMemberSrc.Alignment,
        structMemberSrc.ArrayStride,
        structMemberSrc.MatrixStride,
        structMemberSrc.ArrayElements
    );

    if (structMemberSrc.KeyName != nullptr) { GlobalFree((HGLOBAL)structMemberSrc.KeyName); }
    if (structMemberSrc.RawName != nullptr) { GlobalFree((HGLOBAL)structMemberSrc.RawName); }

    if (structMemberSrc.CountMembers > 0)
    {
        if (structMemberSrc.StructMembers != nullptr)
        {
            TypeMemberReflectionDescription* structMemberSubStruct = new TypeMemberReflectionDescription[structMemberSrc.CountMembers];

            //Set the cbuffer member's struct members
            for (int im = 0; im < structMemberSrc.CountMembers; im++)
            {
                const xkslangDll::ConstantBufferMemberReflectionDescriptionData& structMemberSubStructSrc = structMemberSrc.StructMembers[im];
                TypeMemberReflectionDescription& structMemberSubStructDst = structMemberSubStruct[im];
                if (!ConvertAndReleaseDllStructMemberDataToReflectionStructMemberType(structMemberSubStructSrc, structMemberSubStructDst))
                    return error("Failed to convert the struct member");
            }

            structMemberDst.Type.SetStructMembers(structMemberSubStruct, structMemberSrc.CountMembers);

            GlobalFree((HGLOBAL)structMemberSrc.StructMembers);
        }
        else
        {
            return error("A cbuffer struct member is missing Reflection data about its members");
        }
    }

    return true;
}

static bool CompileMixerUsingXkslangDll(string effectName, EffectMixerObject* mixer, vector<OutputStageBytecode>& outputStages, vector<string>& errorMsgs)
{
#ifdef ALLOW_ACCESS_TO_XKSLANG_DLL
    DWORD time_before, time_after;
    bool success = true;

    unsigned int countOutputStages = (unsigned int)(outputStages.size());
    xkslangDll::OutputStageEntryPoint* stageEntryPointArray = new xkslangDll::OutputStageEntryPoint[countOutputStages];
    for (unsigned int i = 0; i < countOutputStages; i++)
    {
        stageEntryPointArray[i].stage = outputStages[i].stage;
        stageEntryPointArray[i].entryPointName = outputStages[i].entryPointName.c_str();
    }

    time_before = GetTickCount();
    success = xkslangDll::CompileMixer(mixer->mixerHandleId, stageEntryPointArray, countOutputStages);
    time_after = GetTickCount();
    delete stageEntryPointArray;

    if (success) std::cout << "OK. time: " << (time_after - time_before) << "ms" << endl;
    else
    {
        char* pError = xkslangDll::GetErrorMessages();
        if (pError != nullptr) error(pError);
        GlobalFree(pError);

        error("Compilation Failed");
        return false;
    }

    //get and save the final mixin compiled bytecode
    vector<uint32_t> compiledBytecode;
    {
        int bytecodeLength = 0;
        uint32_t* pBytecodeBuffer = xkslangDll::GetMixerCompiledBytecode(mixer->mixerHandleId, &bytecodeLength);
        if (pBytecodeBuffer == nullptr || bytecodeLength <= 0) {
            error("Failed to get the mixin compiled bytecode");
            success = false;
        }

        compiledBytecode.assign(pBytecodeBuffer, pBytecodeBuffer + bytecodeLength);
        if (pBytecodeBuffer != nullptr) GlobalFree(pBytecodeBuffer);

        string outputFileNameFinalSpv = effectName + "_compile4_final.spv";
        string outputFileNameFinalSpvHr = effectName + "_compile4_final.hr.spv";
        WriteBytecode(compiledBytecode, outputDir, outputFileNameFinalSpv, BytecodeFileFormat::Binary);
        WriteBytecode(compiledBytecode, outputDir, outputFileNameFinalSpvHr, BytecodeFileFormat::Text);
    }

    if (!success) return false;

    //get the reflection data from the compiled bytecode
    EffectReflection effectReflection;
    if (buildEffectReflection)
    {
        int32_t constantBufferStructSize, resourceBindingsStructSize, inputAttributesStructSize;
        xkslangDll::ConstantBufferReflectionDescriptionData* pAllocsConstantBuffers;
        xkslangDll::EffectResourceBindingDescriptionData* pAllocsResourceBindings;
        xkslangDll::ShaderInputAttributeDescriptionData* pAllocsInputAttributes;
        int32_t countConstantBuffers, countResourceBindings, countInputAttributes;

        success = xkslangDll::GetMixerEffectReflectionData(mixer->mixerHandleId,
            &pAllocsConstantBuffers, &countConstantBuffers, &constantBufferStructSize,
            &pAllocsResourceBindings, &countResourceBindings, &resourceBindingsStructSize,
            &pAllocsInputAttributes, &countInputAttributes, &inputAttributesStructSize);

        if (!success)
        {
            char* pError = xkslangDll::GetErrorMessages();
            if (pError != nullptr) error(pError);
            GlobalFree(pError);

            return error("Failed to get the Effect Reflection data");
        }

        //Convert the data into the effectReflection object
        //Process the ResourceBindings
        vector<EffectResourceBindingDescription> vecAllResourceBindings;
        if (countResourceBindings > 0 && pAllocsResourceBindings != nullptr)
        {
            if (sizeof(xkslangDll::EffectResourceBindingDescriptionData) != resourceBindingsStructSize) return error("Invalid data struct size");

            xkslangDll::EffectResourceBindingDescriptionData* effectResourceBinding;
            for (int i = 0; i < countResourceBindings; i++)
            {
                effectResourceBinding = pAllocsResourceBindings + i;

                string keyName = ConvertCharPtrToString(effectResourceBinding->KeyName);
                string rawName = ConvertCharPtrToString(effectResourceBinding->RawName);
                string resourceGroupName = ConvertCharPtrToString(effectResourceBinding->ResourceGroupName);
                EffectResourceBindingDescription binding
                (
                    effectResourceBinding->Stage,
                    keyName,
                    rawName,
                    resourceGroupName,
                    effectResourceBinding->Class,
                    effectResourceBinding->Type
                );
                vecAllResourceBindings.push_back(binding);

                if (effectResourceBinding->KeyName != nullptr) GlobalFree((HGLOBAL)effectResourceBinding->KeyName);
                if (effectResourceBinding->RawName != nullptr) GlobalFree((HGLOBAL)effectResourceBinding->RawName);
                if (effectResourceBinding->ResourceGroupName != nullptr) GlobalFree((HGLOBAL)effectResourceBinding->ResourceGroupName);
            }

            //delete the data allocated on the native code
            GlobalFree((HGLOBAL)pAllocsResourceBindings);
        }
        effectReflection.SetResourcesBindings(vecAllResourceBindings);

        //Process the InputAttributes
        vector<ShaderInputAttributeDescription> vecAllInputAttributes;
        if (countInputAttributes > 0 && pAllocsInputAttributes != nullptr)
        {
            if (sizeof(xkslangDll::ShaderInputAttributeDescriptionData) != inputAttributesStructSize) return error("Invalid data struct size");

            xkslangDll::ShaderInputAttributeDescriptionData* shaderInputAttribute;
            for (int i = 0; i < countInputAttributes; i++)
            {
                shaderInputAttribute = pAllocsInputAttributes + i;

                ShaderInputAttributeDescription inputAttribute
                (
                    ConvertCharPtrToString(shaderInputAttribute->SemanticName),
                    shaderInputAttribute->SemanticIndex
                );
                vecAllInputAttributes.push_back(inputAttribute);

                if (shaderInputAttribute->SemanticName != nullptr) GlobalFree((HGLOBAL)shaderInputAttribute->SemanticName);
            }

            //delete the data allocated on the native code
            GlobalFree((HGLOBAL)pAllocsInputAttributes);
        }
        effectReflection.SetInputAttributes(vecAllInputAttributes);

        //Process the ConstantBuffers
        if (countConstantBuffers > 0 && pAllocsConstantBuffers != nullptr)
        {
            if (sizeof(xkslangDll::ConstantBufferReflectionDescriptionData) != constantBufferStructSize) return error("Invalid data struct size");

            effectReflection.CountConstantBuffers = countConstantBuffers;
            effectReflection.ConstantBuffers = new ConstantBufferReflectionDescription[countConstantBuffers];

            for (int i = 0; i < countConstantBuffers; i++)
            {
                //read the cbuffer data
                xkslangDll::ConstantBufferReflectionDescriptionData* constantBufferData;
                constantBufferData = pAllocsConstantBuffers + i;

                //create the cbuffer
                string cbufferName = ConvertCharPtrToString(constantBufferData->CbufferName);
                if (constantBufferData->CbufferName != nullptr) GlobalFree((HGLOBAL)constantBufferData->CbufferName);

                ConstantBufferReflectionDescription& constantBuffer = effectReflection.ConstantBuffers[i];
                constantBuffer.CbufferName = cbufferName;
                constantBuffer.Size = constantBufferData->Size;

                //process the cbuffer members
                if (constantBufferData->CountMembers > 0)
                {
                    constantBuffer.Members.clear();
                    constantBuffer.Members.resize(constantBufferData->CountMembers);

                    for (int m = 0; m < constantBufferData->CountMembers; ++m)
                    {
                        ConstantBufferMemberReflectionDescription& cbufferMember = constantBuffer.Members[m];

                        xkslangDll::ConstantBufferMemberReflectionDescriptionData* memberData;
                        memberData = constantBufferData->Members + m;

                        cbufferMember.KeyName = ConvertCharPtrToString(memberData->KeyName);
                        cbufferMember.RawName = ConvertCharPtrToString(memberData->RawName);
                        cbufferMember.LogicalGroup = ConvertCharPtrToString(memberData->LogicalGroup);
                        cbufferMember.Offset = memberData->Offset;
                        cbufferMember.ReflectionType.Set(
                            0,
                            memberData->Class,
                            memberData->Type,
                            memberData->RowCount,
                            memberData->ColumnCount,
                            memberData->Size,
                            memberData->Alignment,
                            memberData->ArrayStride,
                            memberData->MatrixStride,
                            memberData->ArrayElements
                        );

                        if (memberData->CountMembers > 0)
                        {
                            if (memberData->StructMembers != nullptr)
                            {
                                TypeMemberReflectionDescription* structMembers = new TypeMemberReflectionDescription[memberData->CountMembers];

                                //Set the cbuffer member's struct members
                                for (int im = 0; im < memberData->CountMembers; im++)
                                {
                                    const xkslangDll::ConstantBufferMemberReflectionDescriptionData& structMemberSrc = memberData->StructMembers[im];
                                    TypeMemberReflectionDescription& structMemberDst = structMembers[im];
                                    if (!ConvertAndReleaseDllStructMemberDataToReflectionStructMemberType(structMemberSrc, structMemberDst))
                                        return error("Failed to convert the struct member");
                                }

                                cbufferMember.ReflectionType.SetStructMembers(structMembers, memberData->CountMembers);

                                GlobalFree((HGLOBAL)memberData->StructMembers);
                            }
                            else
                            {
                                return error("A cbuffer struct member is missing Reflection data about its members");
                            }
                        }

                        if (memberData->KeyName != nullptr) GlobalFree((HGLOBAL)memberData->KeyName);
                        if (memberData->RawName != nullptr) GlobalFree((HGLOBAL)memberData->RawName);
                        if (memberData->LogicalGroup != nullptr) GlobalFree((HGLOBAL)memberData->LogicalGroup);
                    }

                    GlobalFree((HGLOBAL)constantBufferData->Members);
                }
            }

            GlobalFree((HGLOBAL)pAllocsConstantBuffers);
        }

        //TMP: directly call xkslang classes
        /*success = SpxMixer::GetCompiledBytecodeReflection(compiledBytecode, effectReflection, errorMsgs);
        if (!success)
        {
            error("Failed to get the reflection data from the compiled bytecode");
            return false;
        }*/
    }

    //get the output stages' compiled bytecode
    for (unsigned int i = 0; i < outputStages.size(); ++i)
    {
        int bytecodeLength = 0;
        uint32_t* pBytecodeBuffer = xkslangDll::GetMixerCompiledBytecodeForStage(mixer->mixerHandleId, outputStages[i].stage, &bytecodeLength);
        if (pBytecodeBuffer == nullptr || bytecodeLength <= 0) {
            error("Failed to get the mixin compiled bytecode");
            success = false;
        }

        vector<uint32_t>& vecBytecode = outputStages[i].resultingBytecode.getWritableBytecodeStream();
        vecBytecode.assign(pBytecodeBuffer, pBytecodeBuffer + bytecodeLength);
        if (pBytecodeBuffer != nullptr) GlobalFree(pBytecodeBuffer);
    }

    //write and check the stages's compiled bytecode
    success = OutputAndCheckOutputStagesCompiledBytecode(effectName, outputStages, buildEffectReflection ? &effectReflection : nullptr);
    return success;
#else
    return error("Unauthorized access to DLL functions");
#endif
}

static bool CompileMixer(string effectName, SpxMixer* mixer, vector<OutputStageBytecode>& outputStages, vector<string>& errorMsgs)
{
    DWORD time_before, time_after;
    bool success = true;

    std::vector<uint32_t> composedSpv;
    std::vector<uint32_t> streamsMergedSpv;
    std::vector<uint32_t> streamsReshuffledSpv;
    std::vector<uint32_t> mergedCBuffersSpv;
    std::vector<uint32_t> compiledBytecode;
    //std::vector<uint32_t> errorSpv;
    std::cout << "Compile Mixin: ";
    time_before = GetTickCount();
    success = mixer->Compile(outputStages, errorMsgs, &composedSpv, &streamsMergedSpv, &streamsReshuffledSpv, &mergedCBuffersSpv, &compiledBytecode, nullptr);
    time_after = GetTickCount();

    if (success) std::cout << "OK. time: " << (time_after - time_before) << "ms" << endl;
    else {
        error("Compilation Failed");
        //WriteBytecode(errorSpv, outputDir, effectName + "_compile_error.hr.spv", BytecodeFileFormat::Text);
    }

#ifdef WRITE_BYTECODE_ON_DISK_AFTER_EVERY_MIXIN_STEPS
    {
        //output the compiled intermediary bytecodes
        WriteBytecode(composedSpv, outputDir, effectName + "_compile0_composed.hr.spv", BytecodeFileFormat::Text);
        WriteBytecode(streamsMergedSpv, outputDir, effectName + "_compile1_streamsMerged.hr.spv", BytecodeFileFormat::Text);
        WriteBytecode(streamsReshuffledSpv, outputDir, effectName + "_compile2_streamsReshuffled.hr.spv", BytecodeFileFormat::Text);
        WriteBytecode(mergedCBuffersSpv, outputDir, effectName + "_compile3_mergedCBuffers.hr.spv", BytecodeFileFormat::Text);
    }
#endif

#ifdef OUTPUT_LIST_COMPOSITIONS_AFTER_EVERY_MIXIN_STEPS
    if (!displayListOfAllCompositionsForTheMixer(mixer))
        return error("Failed to display the mixer list of compositions");
#endif

    {
        //write the final SPIRV bytecode then convert it into GLSL
        string outputFileNameFinalSpv = effectName + "_compile4_final.spv";
        string outputFileNameFinalSpvHr = effectName + "_compile4_final.hr.spv";
        WriteBytecode(compiledBytecode, outputDir, outputFileNameFinalSpv, BytecodeFileFormat::Binary);
        WriteBytecode(compiledBytecode, outputDir, outputFileNameFinalSpvHr, BytecodeFileFormat::Text);
    }

    if (!success) return false;

    //get the reflection data from the compiled bytecode
    EffectReflection effectReflection;
    if (buildEffectReflection)
    {
        success = SpxMixer::GetCompiledBytecodeReflection(compiledBytecode, effectReflection, errorMsgs);
        if (!success)
        {
            error("Failed to get the reflection data from the compiled bytecode");
            return false;
        }

        //std::cout << endl << "EffectReflection:" << endl;
        //std::cout << effectReflection.Print() << endl;
    }

    //write and check the stages's compiled bytecode
    success = OutputAndCheckOutputStagesCompiledBytecode(effectName, outputStages, buildEffectReflection? &effectReflection: nullptr);
    return success;
}

static bool ParseAndConvertXkslFile(XkslParser* parser, string& xkslInputFile,
    const vector<ShaderGenericValues>& listGenericsValue, const vector<XkslUserDefinedMacro> listUserDefinedMacros,
     SpxBytecode& spirXBytecode)
{
    const string inputFname = inputDir + xkslInputFile;
    string xkslInput;
    if (!Utils::ReadFile(inputFname, xkslInput))
    {
        error(" Failed to read the file: " + inputFname);
        return false;
    }

    //======================================================================================================
    //======================================================================================================
    // Parse and convert XKSL shaders to SPIRX bytecode
    ostringstream errorAndDebugMessages;
    bool success = parser->ConvertXkslFileToSpx(xkslInputFile, xkslInput, listGenericsValue, listUserDefinedMacros, spirXBytecode, &errorAndDebugMessages);

    string infoMsg = errorAndDebugMessages.str();
    if (infoMsg.size() > 0) std::cout << infoMsg;

    if (!success) {
        error(" Failed to parse the XKSL file");
    }
    
    return success;
}

static map<string, string> mapShaderNameData;
static vector<string> libraryResourcesFolders;  //folders where we can look to find the shader file
static string shaderFilesPrefix; //we can concatenate a prefix string to the shader name before searching them
static bool callbackRequestDataForShader(const string& shaderName, string& shaderData)
{
    std::cout << " Parsing shader file: " << shaderName << endl;

    int countLoop = 1;
    if (shaderFilesPrefix.size() > 0) countLoop = 2;
    for (int i = 0; i < countLoop; i++)
    {
        string filePrefix = (i==0? "": (shaderFilesPrefix + "_"));

        for (unsigned int k = 0; k < libraryResourcesFolders.size(); ++k)
        {
            string& folder = libraryResourcesFolders[k];

            const string inputFname = folder + filePrefix + shaderName + ".xksl";
            if (Utils::ReadFile(inputFname, shaderData))
            {
                mapShaderNameData[shaderName] = shaderData;
                return true;
            }
        }
    }
    
    error("Callback function: Cannot find any data for the shader: " + shaderName);
    return false;
}

static bool callbackRequestDataForShader_XkfxParser(const string& shaderName, string& shaderData)
{
    std::cout << " Parsing shader file: " << shaderName << endl;

    map<string, string>::const_iterator it = mapShaderNameData.find(shaderName);
    if (it != mapShaderNameData.end())
    {
        shaderData = it->second;
        return true;
    }

    error("Callback function: Cannot find recorded data for the shader: " + shaderName);
    return false;
}

static map<string, char*> mapShaderData;
char* __stdcall callbackRequestDataForShaderDll_Recursif(const char* shaderName, int32_t* dataLen)
{
    if (shaderName == nullptr || dataLen == nullptr) return nullptr;
    const string shaderNameStr(shaderName);

    map<string, char*>::const_iterator it = mapShaderData.find(shaderNameStr);
    if (it != mapShaderData.end())
    {
        char* pShaderData = it->second;
        *dataLen = (int32_t)(strlen(pShaderData));
        return pShaderData;
    }
    else
    {
        string shaderData;
        bool success = callbackRequestDataForShader(shaderNameStr, shaderData);
        if (!success) return nullptr;

        int len = (int)(shaderData.size());
        if (len <= 0) return nullptr;

        char* pShaderData = new char[len + 1];
        strncpy_s(pShaderData, len + 1, shaderData.c_str(), len);
        pShaderData[len] = 0;

        mapShaderData[shaderNameStr] = pShaderData;
        *dataLen = len;
        return pShaderData;
    }
}

static char* singleXkslShaderToReturn = nullptr;
char* __stdcall callbackRequestDataForShaderDll_Single(const char* shaderName, int32_t* dataLen)
{
    if (singleXkslShaderToReturn == nullptr) return nullptr;
    *dataLen = (int32_t)(strlen(singleXkslShaderToReturn));
    return singleXkslShaderToReturn;
}

static bool RecursivelyParseAndConvertXkslShader(XkslParser* parser, const string& effectName, const string& shaderName, string filesPrefix,
    const vector<ShaderGenericValues>& listGenericsValue, const vector<XkslUserDefinedMacro>& listUserDefinedMacros, SpxBytecode& spirXBytecode, string& infoMsg)
{
    shaderFilesPrefix = filesPrefix; //set for the callback function

    ostringstream errorAndDebugMessages;
    bool success = parser->ConvertShaderToSpx(shaderName, callbackRequestDataForShader, listGenericsValue, listUserDefinedMacros, spirXBytecode, &errorAndDebugMessages);
    infoMsg = errorAndDebugMessages.str();
    
    return success;
}

static bool GetShadingStageForString(const string& str, ShadingStageEnum& stage)
{
    if (str.compare("Vertex") == 0) {stage = ShadingStageEnum::Vertex; return true;}
    if (str.compare("Pixel") == 0) {stage = ShadingStageEnum::Pixel; return true;}
    if (str.compare("TessControl") == 0) {stage = ShadingStageEnum::TessControl; return true;}
    if (str.compare("TessEvaluation") == 0) {stage = ShadingStageEnum::TessEvaluation; return true;}
    if (str.compare("Geometry") == 0) {stage = ShadingStageEnum::Geometry; return true;}
    if (str.compare("Compute") == 0) {stage = ShadingStageEnum::Compute; return true;}
    return false;
}

static string GetStringForShadingStage(ShadingStageEnum stage)
{
    switch (stage)
    {
        case xkslang::ShadingStageEnum::Vertex: return "Vertex";
        case xkslang::ShadingStageEnum::Pixel: return "Pixel";
        case xkslang::ShadingStageEnum::TessControl: return "TessControl";
        case xkslang::ShadingStageEnum::TessEvaluation: return "TessEvaluation";
        case xkslang::ShadingStageEnum::Geometry: return "Geometry";
        case xkslang::ShadingStageEnum::Compute: return "Compute";
    }
    return "";
}

static SpxBytecode* GetSpxBytecodeForShader(const string& shaderName, string& shaderFullName, unordered_map<string, SpxBytecode*>& mapShaderNameBytecode, bool canLookIfUnmangledNameMatch)
{
    auto it = mapShaderNameBytecode.find(shaderName);
    if (it != mapShaderNameBytecode.end())
    {
        SpxBytecode* spxBytecode = it->second;
        shaderFullName = it->first;
        return spxBytecode;
    }

    if (canLookIfUnmangledNameMatch)
    {
        SpxBytecode* spxBytecode = nullptr;
        for (auto it = mapShaderNameBytecode.begin(); it != mapShaderNameBytecode.end(); it++)
        {
            string anUnmangledShaderName = XkfxParser::GetUnmangledName(it->first);
            if (anUnmangledShaderName == shaderName)
            {
                if (spxBytecode == nullptr)
                {
                    spxBytecode = it->second;
                    shaderFullName = it->first;
                }
                else
                {
                    error("2 or more shaders match the unmangled shader name: " + anUnmangledShaderName);
                    return nullptr;
                }
            }
        }
        return spxBytecode;
    }
    
    return nullptr;
}
    
static bool RecordSPXShaderBytecode(string shaderFullName, SpxBytecode* spxBytecode, unordered_map<string, SpxBytecode*>& mapShaderNameBytecode)
{
    //auto it = mapShaderNameBytecode.find(fullShaderName);
    //if (it != mapShaderNameBytecode.end()) {
    //    error("Shader is already recorded in the map: " + fullShaderName);
    //    return false;
    //}

    mapShaderNameBytecode[shaderFullName] = spxBytecode;
    return true;
}

static bool ConvertAndLoadRecursif(const string& effectName, unordered_map<string, SpxBytecode*>& mapShaderNameBytecode, vector<SpxBytecode*>& listAllocatedBytecodes,
    const string& stringShaderAndgenericsValue, const string& xkslInputFilePrefix, const vector<XkslUserDefinedMacro>& listUserDefinedMacros,
    XkslParser* parser, bool useXkslangDll)
{
    vector<string> errorMsgs;
    DWORD time_before, time_after;

    //================================================
    //Parse and get the list of shader defintion
    vector<ShaderParsingDefinition> listshaderDefinition;
    if (!XkslParser::ParseStringWithShaderDefinitions(stringShaderAndgenericsValue.c_str(), listshaderDefinition))
        return error("convertAndLoadRecursif: failed to parse the shaders definition from: " + stringShaderAndgenericsValue);
    vector<ShaderGenericValues> listShaderAndGenerics;
    for (unsigned int is = 0; is < listshaderDefinition.size(); is++) {
        listShaderAndGenerics.push_back(ShaderGenericValues(listshaderDefinition[is].shaderName, listshaderDefinition[is].genericsValue));
    }
    if (listShaderAndGenerics.size() == 0) return error("convertAndLoadRecursif: no shader name found");
    string shaderName = listShaderAndGenerics[0].shaderName;

    //================================================
    //Parse and convert the shader and its dependencies
    std::cout << endl << "Recursively parsing XKSL shader \"" << shaderName + "\"" << endl;

    SpxBytecode* spxBytecode = nullptr;
    vector<string> vecShadersParsed;
    if (useXkslangDll)
    {
#ifdef ALLOW_ACCESS_TO_XKSLANG_DLL
        shaderFilesPrefix = xkslInputFilePrefix; //set for the callback function

        string stringMacroDef = "";
        for (unsigned int k = 0; k < listUserDefinedMacros.size(); ++k)
            stringMacroDef += listUserDefinedMacros[k].macroName + " " + listUserDefinedMacros[k].macroValue + " ";

        int bytecodeLength = 0;
        time_before = GetTickCount();
        uint32_t* pBytecodeBuffer = xkslangDll::ConvertXkslShaderToSPX(shaderName.c_str(), stringShaderAndgenericsValue.c_str(), stringMacroDef.c_str(),
            callbackRequestDataForShaderDll_Recursif, &bytecodeLength);
        time_after = GetTickCount();

        if (pBytecodeBuffer == nullptr || bytecodeLength < 0)
        {
            char* pError = xkslangDll::GetErrorMessages();
            if (pError != nullptr) error(pError);
            GlobalFree(pError);

            return error("convertAndLoadRecursif: failed to convert the XKSL file name: " + xkslInputFilePrefix);
        }
        else std::cout << " OK. time: " << (time_after - time_before) << "ms" << endl;

        spxBytecode = new SpxBytecode;
        listAllocatedBytecodes.push_back(spxBytecode);
        vector<uint32_t>& vecBytecode = spxBytecode->getWritableBytecodeStream();
        vecBytecode.assign(pBytecodeBuffer, pBytecodeBuffer + bytecodeLength);
        if (pBytecodeBuffer != nullptr) GlobalFree(pBytecodeBuffer);
#else
        return error("Unauthorized access to DLL functions");
#endif
    }
    else
    {
        string infoMsg;
        spxBytecode = new SpxBytecode;
        listAllocatedBytecodes.push_back(spxBytecode);
        string spxOutputFileName;

        time_before = GetTickCount();
        bool success = RecursivelyParseAndConvertXkslShader(parser, effectName, shaderName, xkslInputFilePrefix, listShaderAndGenerics, listUserDefinedMacros, *spxBytecode, infoMsg);
        time_after = GetTickCount();

        if (infoMsg.size() > 0) std::cout << infoMsg;

        if (success) std::cout << " OK. time: " << (time_after - time_before) << "ms" << endl;
        else
        {
            error("convertAndLoadRecursif: failed to parse and convert the XKSL file name: " + shaderName);
            return false;
        }
    }

#ifdef WRITE_BYTECODE_ON_DISK_AFTER_EVERY_MIXIN_STEPS
    {
        //Save the bytecode on the disk
        string prefix = xkslInputFilePrefix;
        if (prefix.size() == 0) prefix = effectName;
        WriteBytecode(spxBytecode->getBytecodeStream(), outputDir, prefix + "_" + shaderName + ".spv", BytecodeFileFormat::Binary);
        WriteBytecode(spxBytecode->getBytecodeStream(), outputDir, prefix + "_" + shaderName + ".hr.spv", BytecodeFileFormat::Text);
    }
#endif

    //Query the list of shaders from the bytecode
    if (useXkslangDll)
    {
#ifdef ALLOW_ACCESS_TO_XKSLANG_DLL
        int countShaders = 0;
        xkslangDll::BytecodeShaderInformation *shadersInfo;
        uint32_t* pBytecodeBuffer = &(spxBytecode->getWritableBytecodeStream().front());
        int32_t bytecodeLength = spxBytecode->GetBytecodeSize();
        bool success = xkslangDll::GetBytecodeShadersInformation(pBytecodeBuffer, bytecodeLength, &shadersInfo, &countShaders);
        if (!success)
        {
            char* pError = xkslangDll::GetErrorMessages();
            if (pError != nullptr) error(pError);
            GlobalFree(pError);

            return error("convertAndLoadRecursif: failed to get the shader info from the bytecode");
        }

        for (int k = 0; k < countShaders; ++k)
        {
            vecShadersParsed.push_back(shadersInfo[k].ShaderName);
            GlobalFree(shadersInfo[k].ShaderName);
        }
        GlobalFree(shadersInfo);
#else
        return error("Unauthorized access to DLL functions");
#endif
    }
    else
    {
        if (!SpxMixer::GetListAllShadersFromBytecode(*spxBytecode, vecShadersParsed, errorMsgs))
        {
            error("convertAndLoadRecursif: failed to get the list of shader names from: " + xkslInputFilePrefix);
            return false;
        }
    }

    //Store the new shaders bytecode in our map
    for (unsigned int is = 0; is < vecShadersParsed.size(); ++is)
    {
        string shaderName = vecShadersParsed[is];
        if (!RecordSPXShaderBytecode(shaderName, spxBytecode, mapShaderNameBytecode))
        {
            error("convertAndLoadRecursif: Can't add the shader into the bytecode: " + shaderName);
            return false;
        }
    }

    return true;
}

static EffectMixerObject* CreateAndAddNewMixer(unordered_map<string, EffectMixerObject*>& mixerMap, string newMixerName, bool useXkslangDll)
{
    if (mixerMap.find(newMixerName) != mixerMap.end()) {
        error("CreateAndAddNewMixer: a mixer already exists with the name:" + newMixerName);
        return nullptr;
    }

    EffectMixerObject* mixerObject = nullptr;
    if (useXkslangDll)
    {
#ifdef ALLOW_ACCESS_TO_XKSLANG_DLL
        uint32_t mixerHandleId = xkslangDll::CreateSpxShaderMixer();
        if (mixerHandleId == 0) {
            error("Failed to create a new spx mixer");
            return nullptr;
        }
        mixerObject = new EffectMixerObject(newMixerName, mixerHandleId);
#else
        error("Unauthorized access to DLL functions");
        return nullptr;
#endif
    }
    else
    {
        SpxMixer* mixer = new SpxMixer();
        mixerObject = new EffectMixerObject(newMixerName, mixer);
    }

    if (mixerObject == nullptr) return nullptr;
    mixerMap[newMixerName] = mixerObject;

    return mixerObject;
}

static bool MixinShaders(const string& effectName, unordered_map<string, SpxBytecode*>& mapShaderNameBytecode, unordered_map<string, EffectMixerObject*>& mixerMap,
    vector<SpxBytecode*>& listAllocatedBytecodes, const vector<XkslUserDefinedMacro>& listUserDefinedMacros, XkslParser* parser, bool useXkslangDll,
    EffectMixerObject* mixerTarget, const string& mixinShadersInstructionString,
    const string mixinOperationInstructionLineLog, int& operationNum);

static bool AddCompositionToMixer(const string& effectName, unordered_map<string, SpxBytecode*>& mapShaderNameBytecode, unordered_map<string, EffectMixerObject*>& mixerMap,
    vector<SpxBytecode*>& listAllocatedBytecodes, const vector<XkslUserDefinedMacro>& listUserDefinedMacros, XkslParser* parser, bool useXkslangDll, int& operationNum,
    EffectMixerObject* mixerTarget, const CompositionExpression& compositionExpression, const string& targetedShaderName)
{
    bool success = false;
    DWORD time_before, time_after;

    //===================================================
    //composition variable target
    const string& compTarget = compositionExpression.Target;

    //We can either have shader.variableName, or only a variableName
    string shaderName, variableName;
    if (!XkfxParser::SeparateAdotB(compTarget, shaderName, variableName))
    {
        variableName = compTarget;
        shaderName = targetedShaderName;
    }

    //===================================================
    //composition expression: we can either have a single expression, or multiple ones
    string compExpression = compositionExpression.Expression;

    vector<string> listCompositionInstructions;
    if (XkslangUtils::startWith(compExpression.c_str(), "["))
    {
        if (!XkslangUtils::endWith(compExpression, "]"))
            return error("Invalid compositions instruction string: " + compExpression);

        compExpression = compExpression.substr(1, compExpression.size() - 2);

        //If the instruction start with '[', there are several compositions assigned to the target
        vector<string> errorMsgs;
        vector<CompositionExpression> compositionExpressions;
        if (!XkfxParser::SplitCompositionParametersString(compExpression.c_str(), compositionExpressions, false, errorMsgs))
        {
            for (unsigned int k = 0; k < errorMsgs.size(); k++) error(errorMsgs[k]);
            return error("Failed to split the compositions instruction string: " + compExpression);
        }
        for (unsigned int k = 0; k < compositionExpressions.size(); ++k) listCompositionInstructions.push_back(compositionExpressions[k].Expression);
    }
    else listCompositionInstructions.push_back(compExpression);

    //===================================================
    //Process all compositions
    for (unsigned int iComp = 0; iComp < listCompositionInstructions.size(); iComp++)
    {
        const string compositionInstruction = listCompositionInstructions[iComp];

        //expression can have the following cases:
        // mixin( "mixinInstruction" )
        // mixin "mixinInstruction"
        // "mixinInstruction"
        // mixerName

        EffectMixerObject* compositionSourceMixer = nullptr;

        //first check if a mixer exists with the mixinInstruction name
        {
            const string& mixerName = compositionInstruction;

            //find the mixer in our mixer map
            auto itm = mixerMap.find(mixerName);
            if (itm != mixerMap.end())
                compositionSourceMixer = itm->second;
        }

        if (compositionSourceMixer == nullptr)
        {
            string mixinInstructionStr = compositionInstruction;

            if (XkslangUtils::startWith(mixinInstructionStr, "mixin(") || XkslangUtils::startWith(mixinInstructionStr, "mixin "))
            {
                mixinInstructionStr = mixinInstructionStr.substr(strlen("mixin"));
                mixinInstructionStr = XkslangUtils::trim(mixinInstructionStr);
                if (XkslangUtils::startWith(mixinInstructionStr, "(") && XkslangUtils::endWith(mixinInstructionStr, ")"))
                    mixinInstructionStr = mixinInstructionStr.substr(1, mixinInstructionStr.length() - 2);
                else
                    return error("mixin instruction: parenthesis are missing");
            }

            //Create the anonymous mixer
            string anonymousMixerName = "_anonMixer_" + to_string(mixerMap.size());
            EffectMixerObject* anonymousMixer = CreateAndAddNewMixer(mixerMap, anonymousMixerName, useXkslangDll);
            if (anonymousMixer == nullptr) {
                return error("addComposition: Failed to create a new mixer object");
            }

            //Mix the new mixer with the shaders specified in the function parameter
            success = MixinShaders(effectName, mapShaderNameBytecode, mixerMap, listAllocatedBytecodes, listUserDefinedMacros, parser, useXkslangDll,
                anonymousMixer, mixinInstructionStr, mixinInstructionStr, operationNum);
            if (!success) return error("Mixin failed: " + mixinInstructionStr);

            compositionSourceMixer = anonymousMixer;
        }

        if (compositionSourceMixer == nullptr) {
            return error("addComposition: no mixer source to make the composition");
        }

        //=====================================
        // Add the composition to the mixer
        std::cout << endl;
        std::cout << "===================" << endl;
        std::cout << "Instancing composition: \"" << (shaderName.size() > 0? (shaderName + "."): "") << variableName << " = "
            << compositionInstruction << "\"" << " from mixer: \"" << compositionSourceMixer->name << "\""  << ", to mixer: \"" << mixerTarget->name << "\"" << endl;

        if (useXkslangDll)
        {
#ifdef ALLOW_ACCESS_TO_XKSLANG_DLL
            time_before = GetTickCount();
            success = xkslangDll::AddComposition(mixerTarget->mixerHandleId, shaderName.c_str(), variableName.c_str(), compositionSourceMixer->mixerHandleId);
            time_after = GetTickCount();
#else
            return error("Unauthorized access to DLL functions");
#endif
        }
        else
        {
            vector<string> errorMsgs;
            time_before = GetTickCount();
            success = mixerTarget->mixer->AddCompositionInstance(shaderName, variableName, compositionSourceMixer->mixer, errorMsgs);
            time_after = GetTickCount();

            if (!success)
            {
                for (unsigned int k = 0; k < errorMsgs.size(); k++) error(errorMsgs[k]);
            }
        }

        if (success) std::cout << " OK. Time:  " << (time_after - time_before) << "ms" << endl;
        else return error("Failed to add the composition to the mixer");

#ifdef WRITE_BYTECODE_ON_DISK_AFTER_EVERY_MIXIN_STEPS
        {
            //write the mixer current bytecode
            string outputFileName = effectName + "_op" + to_string(operationNum++) + "_compose" + ".hr.spv";
            success = GetAndWriteMixerCurrentBytecode(mixerTarget, outputFileName, useXkslangDll);
            if (!success) return error("Failed to get and write the mixer current bytecode");
        }
#endif

#ifdef OUTPUT_LIST_COMPOSITIONS_AFTER_EVERY_MIXIN_STEPS
        if (!displayListOfAllCompositionsForTheMixer(mixerTarget->mixer))
            return error("Failed to display the mixer list of compositions");
#endif

#ifdef PROCESS_BYTECODE_SANITY_CHECK_AFTER_EVERY_MIXIN_STEPS
        {
            //Do a bytecode sanity check
            vector<string> errorMsgs;
            vector<uint32_t> mixinBytecode;
            if (!GetMixerCurrentBytecode(mixerTarget, mixinBytecode, useXkslangDll)) return error("failed to get the mixer current bytecode");
            if (!XkslParser::ProcessBytecodeSanityCheck(mixinBytecode, errorMsgs)) {
                for (unsigned int k = 0; k < errorMsgs.size(); k++) error(errorMsgs[k]);
                error("Failed to process a bytecode sanity check");
                return false;
            }
        }
#endif
    }

    return true;
}

static bool AddCompositionsToMixer(const string& effectName, unordered_map<string, SpxBytecode*>& mapShaderNameBytecode, unordered_map<string, EffectMixerObject*>& mixerMap,
    vector<SpxBytecode*>& listAllocatedBytecodes, const vector<XkslUserDefinedMacro>& listUserDefinedMacros, XkslParser* parser, bool useXkslangDll, int& operationNum,
    EffectMixerObject* mixerTarget, const string& compositionsString, const string& targetedShaderName)
{
    std::cout << endl;
    std::cout << "====================================" << endl;
    std::cout << "Process AddCompositions instructions: \"" << compositionsString << "\"" << endl;

    //split the string
    vector<string> errorMsgs;
    vector<CompositionExpression> compositions;
    if (!XkfxParser::SplitCompositionParametersString(compositionsString.c_str(), compositions, true, errorMsgs))
    {
        for (unsigned int k = 0; k < errorMsgs.size(); k++) error(errorMsgs[k]);
        return error("failed to split the parameters");
    }

    if (compositions.size() == 0)
        return error("No composition found in the string: " + compositionsString);

    for (unsigned int k = 0; k < compositions.size(); ++k)
    {
        const CompositionExpression& compositionExpression = compositions[k];
        bool success = AddCompositionToMixer(effectName, mapShaderNameBytecode, mixerMap,
            listAllocatedBytecodes, listUserDefinedMacros, parser, useXkslangDll, operationNum,
            mixerTarget, compositionExpression, targetedShaderName);

        if (!success)
            return error("Failed to add the composition into the mixer");
    }

    return true;
}

static bool MixinShaders(const string& effectName, unordered_map<string, SpxBytecode*>& mapShaderNameBytecode, unordered_map<string, EffectMixerObject*>& mixerMap,
    vector<SpxBytecode*>& listAllocatedBytecodes, const vector<XkslUserDefinedMacro>& listUserDefinedMacros, XkslParser* parser, bool useXkslangDll,
    EffectMixerObject* mixerTarget, const string& mixinShadersInstructionString,
    const string mixinOperationInstructionLineLog, int& operationNum)
{
    std::cout << endl;
    std::cout << "===========================" << endl;
    std::cout << "Process mixin instructions: \"" << mixinOperationInstructionLineLog << "\"" << " into mixer: \"" << mixerTarget->name << "\"" << endl;

    bool success = true;

    //we can either have a list of shaders (with some generics and compositions) directly in the function defintion,
    //or having those shaders encapsulated in {} brackets (to let us apply some compositions to the whole mixin)
    string listShadersToMix;
    string compositionInstructionsToApplyToTheMixin;
    {
        vector<string> errorMsgs;
        if (!XkslParser::ParseStringWithMixinShadersAndCompositions(mixinShadersInstructionString.c_str(), listShadersToMix, compositionInstructionsToApplyToTheMixin, errorMsgs))
        {
            for (unsigned int k = 0; k < errorMsgs.size(); k++) error(errorMsgs[k]);
            return error("Failed to parse the mixin instructions: " + mixinShadersInstructionString);
        }
    }

    if (listShadersToMix.size() == 0) return error("No shader to mix");

    //================================================
    //Parse and get the list of shader defintion
    vector<ShaderParsingDefinition> listShaderDefinition;
    if (!XkslParser::ParseStringWithShaderDefinitions(listShadersToMix.c_str(), listShaderDefinition))
        return error("mixin: failed to parse the shaders definition from: " + listShadersToMix);
    if (listShaderDefinition.size() == 0) return error("mixin: list of shader is empty");
    string shaderName = listShaderDefinition[0].shaderName;
    
    //================================================
    //Get the bytecode file and the fullName of all shader to mix into the mixer
    vector<pair<string, SpxBytecode*>> listShaderBytecodeToMix; //list of shaders to mix, and their corresponding bytecode
    {
        for (auto its = listShaderDefinition.begin(); its != listShaderDefinition.end(); its++)
        {
            const ShaderParsingDefinition& shaderDef = *its;
            string shaderName = shaderDef.GetShaderNameWithGenerics();
            string shaderFullName;   //we can omit to specify the generics when mixin a shader, we will search the best match

            SpxBytecode* shaderBytecode = GetSpxBytecodeForShader(shaderName, shaderFullName, mapShaderNameBytecode, true);
            if (shaderBytecode == nullptr)
            {
                if (xkfxOptions_automaticallyTryToLoadAndConvertUnknownMixinShader)
                {
                    //the shader bytecode does not exist, but we can try to find and generate it
                    success = ConvertAndLoadRecursif(effectName, mapShaderNameBytecode, listAllocatedBytecodes,
                        shaderName, shaderFilesPrefix, listUserDefinedMacros,
                        parser, useXkslangDll);

                    if (!success) return error("Failed to recursively convert and load the shaders: " + shaderName);

                    shaderBytecode = GetSpxBytecodeForShader(shaderName, shaderFullName, mapShaderNameBytecode, true);
                }

                if (shaderBytecode == nullptr) {
                    return error("cannot find or generate a bytecode for the shader: " + shaderName);
                }
            }

            listShaderBytecodeToMix.push_back(pair<string, SpxBytecode*>(shaderFullName, shaderBytecode));
        }
    }

    unsigned int countShadersToMix = (unsigned int)(listShaderBytecodeToMix.size());
    if (countShadersToMix == 0) return error("No bytecode to mix");
    if (countShadersToMix != listShaderDefinition.size()) return error("Invalid size");

    //Previous version could mix several shaders at once into the mixer (provided that all shaders are converted into the same bytecode)
    //We now mix one shader after the previous one (to fix the same bytecode issue)
    //(also mixin shaders all-together won't give the same result at mixin one after another one)
    for (unsigned int cs = 0; cs < countShadersToMix; cs++)
    {
        const ShaderParsingDefinition& shaderDefinition = listShaderDefinition[cs];
        string shaderFullName = listShaderBytecodeToMix[cs].first;
        SpxBytecode* shaderBytecode = listShaderBytecodeToMix[cs].second;
        vector<string> listShaderToMix = { shaderFullName }; //mixer can accept a list of shaders to mix (we disable it for now)

        //=====================================================
        //Mixin the bytecode into the mixer
        {
            DWORD time_before, time_after;
            std::cout << endl;
            std::cout << "mixin: " << shaderFullName << endl;
            if (useXkslangDll)
            {
#ifdef ALLOW_ACCESS_TO_XKSLANG_DLL
                if (listShaderToMix.size() == 0) return error("not shader to mix");

                string stringListShadersFullName;
                for (unsigned int k = 0; k < listShaderToMix.size(); k++) stringListShadersFullName += listShaderToMix[k] + " ";

                uint32_t* pBytecodeBuffer = &(shaderBytecode->getWritableBytecodeStream().front());
                int32_t bytecodeLength = shaderBytecode->GetBytecodeSize();
                time_before = GetTickCount();
                success = xkslangDll::MixinShaders(mixerTarget->mixerHandleId, stringListShadersFullName.c_str(), pBytecodeBuffer, bytecodeLength);
                time_after = GetTickCount();

                if (!success)
                {
                    char* pError = xkslangDll::GetErrorMessages();
                    if (pError != nullptr) error(pError);
                    GlobalFree(pError);
                }
#else
                return error("Unauthorized access to DLL functions");
#endif
            }
            else
            {
                vector<string> errorMsgs;
                time_before = GetTickCount();
                success = mixerTarget->mixer->Mixin(*shaderBytecode, listShaderToMix, errorMsgs);
                time_after = GetTickCount();

                if (!success)
                {
                    for (unsigned int k = 0; k < errorMsgs.size(); k++) error(errorMsgs[k]);
                }
            }

            if (success) std::cout << " OK. Time:  " << (time_after - time_before) << "ms" << endl;
            else return false;

#ifdef WRITE_BYTECODE_ON_DISK_AFTER_EVERY_MIXIN_STEPS
            {
                //write the mixer current bytecode
                string outputFileName = effectName + "_op" + to_string(operationNum++) + "_mixin" + ".hr.spv";
                success = GetAndWriteMixerCurrentBytecode(mixerTarget, outputFileName, useXkslangDll);
                if (!success) return error("Failed to get and write the mixer current bytecode");
            }
#endif

#ifdef OUTPUT_LIST_COMPOSITIONS_AFTER_EVERY_MIXIN_STEPS
            if (!displayListOfAllCompositionsForTheMixer(mixerTarget->mixer))
                return error("Failed to display the mixer list of compositions");
#endif

#ifdef PROCESS_BYTECODE_SANITY_CHECK_AFTER_EVERY_MIXIN_STEPS
            {
                //Do a bytecode sanity check
                vector<string> errorMsgs;
                vector<uint32_t> mixinBytecode;
                if (!GetMixerCurrentBytecode(mixerTarget, mixinBytecode, useXkslangDll)) return error("failed to get the mixer current bytecode");
                if (!XkslParser::ProcessBytecodeSanityCheck(mixinBytecode, errorMsgs)) {
                    for (unsigned int k = 0; k < errorMsgs.size(); k++) error(errorMsgs[k]);
                    error("Failed to process a bytecode sanity check");
                    return false;
                }
            }
#endif
        }

        string compositionString = shaderDefinition.compositionString;
        if (compositionString.size() > 0)
        {
            //Directly add compositions into the mixed shader
            success = AddCompositionsToMixer(effectName, mapShaderNameBytecode, mixerMap,
                listAllocatedBytecodes, listUserDefinedMacros, parser, useXkslangDll, operationNum,
                mixerTarget, compositionString, shaderFullName);

            if (!success) 
                return error("Failed to add the compositions instruction to the mixer: " + compositionString);
        }
    }

    //Add the compositions into the mixin
    if (compositionInstructionsToApplyToTheMixin.size() > 0)
    {
        success = AddCompositionsToMixer(effectName, mapShaderNameBytecode, mixerMap,
            listAllocatedBytecodes, listUserDefinedMacros, parser, useXkslangDll, operationNum,
            mixerTarget, compositionInstructionsToApplyToTheMixin, "");

        if (!success)
            return error("Failed to add the compositions instruction to the mixin: " + compositionInstructionsToApplyToTheMixin);
    }

    return true;
}

static bool ProcessEffectCommandLineThroughXkfxParserApi(XkslParser* parser, string effectName, string effectCmdLines, glslang::CallbackRequestDataForShader callbackRequestDataForShader)
{
    DWORD effect_timeStart, effect_timeEnd;

    vector<string> errorMsgs;
    vector<uint32_t> compiledBytecode;
    vector<OutputStageBytecode> outputStages;

    effect_timeStart = GetTickCount();
    bool success = XkfxParser::ProcessXkfxCommandLines(parser, effectCmdLines, callbackRequestDataForShader, &compiledBytecode, outputStages, errorMsgs);
    effect_timeEnd = GetTickCount();

    if (!success) {
        std::cout << endl;
        for (auto it = errorMsgs.begin(); it != errorMsgs.end(); it++) error(*it);
    }

    {
        //write the final SPIRV bytecode then convert it into GLSL
        string outputFileNameFinalSpv = effectName + "_compile4_final.spv";
        string outputFileNameFinalSpvHr = effectName + "_compile4_final.hr.spv";
        WriteBytecode(compiledBytecode, outputDir, outputFileNameFinalSpv, BytecodeFileFormat::Binary);
        WriteBytecode(compiledBytecode, outputDir, outputFileNameFinalSpvHr, BytecodeFileFormat::Text);
    }

    if (!success) return false;

    //get the reflection data from the compiled bytecode
    if (compiledBytecode.size() > 0)
    {
        EffectReflection effectReflection;
        if (buildEffectReflection)
        {
            success = SpxMixer::GetCompiledBytecodeReflection(compiledBytecode, effectReflection, errorMsgs);
            if (!success)
            {
                for (auto it = errorMsgs.begin(); it != errorMsgs.end(); it++) error(*it);
                error("Failed to get the reflection data from the compiled bytecode");
                return false;
            }

            //std::cout << endl << "EffectReflection:" << endl;
            //std::cout << effectReflection.Print() << endl;
        }

        //write and check the stages's compiled bytecode
        success = OutputAndCheckOutputStagesCompiledBytecode(effectName, outputStages, buildEffectReflection ? &effectReflection : nullptr);
    }

    std::cout << endl;
    std::cout << "Effect Total Time: " << (effect_timeEnd - effect_timeStart) << "ms" << endl;
    std::cout << "=================================" << endl;

    return success;
}

static bool ProcessEffectCommandLine(XkslParser* parser, string effectName, string effectCmdLines, bool useXkslangDll, string& updatedEffectCommandLines)
{
    bool success = true;
    updatedEffectCommandLines = "";

    DWORD effect_timeStart, effect_timeEnd;
    effect_timeStart = GetTickCount();

    if (useXkslangDll)
    {
#ifdef ALLOW_ACCESS_TO_XKSLANG_DLL
        xkslangDll::InitializeMixer();
#else
        return error("Unauthorized access to DLL functions");
#endif
    }

    vector<string> errorMsgs;
    DWORD time_before, time_after;
    vector<SpxBytecode*> listAllocatedBytecodes;
    unordered_map<string, SpxBytecode*> mapShaderNameBytecode;
    unordered_map<string, EffectMixerObject*> mixerMap;
    int operationNum = 0;

    vector<string> listParsedInstructions;
    vector<XkslUserDefinedMacro> listUserDefinedMacros;

    string previousPartialLine = "";
    string parsedLine = "";
    stringstream ss(effectCmdLines);
    while (getline(ss, parsedLine, '\n'))
    {
        listParsedInstructions.push_back(parsedLine);

        parsedLine = XkslangUtils::trim(parsedLine, " \t");
        if (parsedLine.size() == 0) continue;
        if (XkslangUtils::startWith(parsedLine, "//"))
        {
            //a comment: ignore the line
            continue;
        }

        //if some instructions are not complete (some unclosed parentheses or brackets, we concatenate them with the next instructions)
        parsedLine = previousPartialLine + parsedLine;
        if (!XkfxParser::IsCommandLineInstructionComplete(parsedLine.c_str()))
        {
            previousPartialLine = parsedLine;
            continue;
        }
        else previousPartialLine = "";
        
        string instructionFullLine = XkslangUtils::trim(parsedLine);
        string firstInstruction;
        string remainingLine;
        if (!XkfxParser::GetNextInstruction(instructionFullLine, firstInstruction, remainingLine, '(', true))
            return error("Failed to get the next instruction from: " + instructionFullLine);

        if (firstInstruction.compare("break") == 0)
        {
            //quit parsing the effect
            break;
        }
        else if (firstInstruction.compare("setSampleTestOptions") == 0)
        {
            string parameterName;
            if (!XkfxParser::GetNextInstruction(remainingLine, parameterName, remainingLine)) {
                error("set: failed to get the parameter value");
                success = false; break;
            }

            if (parameterName.compare("automaticallyTryToLoadAndConvertUnknownMixinShader") == 0)
            {
                string parameterValue;
                if (!XkfxParser::GetNextInstruction(remainingLine, parameterValue, remainingLine)) { error("set: failed to get the parameter value"); success = false; break; }
                if (parameterValue.compare("true") == 0) xkfxOptions_automaticallyTryToLoadAndConvertUnknownMixinShader = true;
                else xkfxOptions_automaticallyTryToLoadAndConvertUnknownMixinShader = false;
            }
            else if (parameterName.compare("processSampleWithXkfxLibrary") == 0)
            {
                string parameterValue;
                if (!XkfxParser::GetNextInstruction(remainingLine, parameterValue, remainingLine)) { error("set: failed to get the parameter value"); success = false; break; }
                if (parameterValue.compare("true") == 0) xkfxOptions_processSampleWithXkfxLibrary = true;
                else xkfxOptions_processSampleWithXkfxLibrary = false;
            }
            else
            {
                error("set: unknown parameter name: " + parameterName);
                success = false; break;
            }
        }
        else if (firstInstruction.compare("addResourcesLibrary") == 0)
        {
            string folder;
            if (!XkfxParser::GetNextInstruction(remainingLine, folder, remainingLine)) {
                error("addResourcesLibrary: failed to get the library resource folder");
                success = false; break;
            }
            folder = XkslangUtils::trim(folder, '\"');

            string path = inputDir + folder + "\\";
            libraryResourcesFolders.push_back(path);
        }
        else if (firstInstruction.compare("setDefine") == 0)
        {
            string strMacrosDefinition = XkslangUtils::trim(remainingLine);
            if (strMacrosDefinition.size() == 0) {
                error("missing macro definition");
                success = false; break;
            }

            if (XkslParser::ParseStringMacroDefinition(strMacrosDefinition.c_str(), listUserDefinedMacros, false) != 1)
            {
                error("Fails to parse the macros definition: " + strMacrosDefinition);
                success = false; break;
            }
        }
        else if (firstInstruction.compare("convertAndLoadRecursif") == 0)
        {
            /// recursively convert and load xksl shaders

            //================================================
            //Parse the command line parameters
            string stringShaderAndgenericsValue = XkslangUtils::trim(remainingLine);
            if (stringShaderAndgenericsValue.size() == 0) {
                error("convertAndLoadRecursif: failed to get the XKSL file parameters");
                success = false; break;
            }

            //any search prefix?
            string xkslInputFilePrefix = "";
            if (stringShaderAndgenericsValue[0] == '"')
            {
                unsigned int indexEnd = 1;
                while (indexEnd < stringShaderAndgenericsValue.size() && stringShaderAndgenericsValue[indexEnd] != '"') indexEnd++;
                if (indexEnd == stringShaderAndgenericsValue.size()) {
                    error("convertAndLoadRecursif: failed to get the prefix parameter");
                    success = false; break;
                }

                xkslInputFilePrefix = stringShaderAndgenericsValue.substr(0, indexEnd + 1);
                xkslInputFilePrefix = XkslangUtils::trim(xkslInputFilePrefix, '\"');
                stringShaderAndgenericsValue = stringShaderAndgenericsValue.substr(indexEnd + 1);
            }

            //Can convert and load the shader
            success = ConvertAndLoadRecursif(effectName, mapShaderNameBytecode, listAllocatedBytecodes,
                stringShaderAndgenericsValue, xkslInputFilePrefix, listUserDefinedMacros,
                parser, useXkslangDll);

            if (!success)
            {
                error("Failed to recursively convert and load the shaders: " + stringShaderAndgenericsValue);
                success = false; break;
            }
        }
        else if (firstInstruction.compare("convertAndLoad") == 0)
        {
            /// convert and load xksl shaders

            //================================================
            //Parse the command line parameters
            string xkslInputFile;
            if (!XkfxParser::GetNextInstruction(remainingLine, xkslInputFile, remainingLine)) {
                error("convertAndLoad: failed to get the XKSL file name");
                success = false; break;
            }
            xkslInputFile = XkslangUtils::trim(xkslInputFile, '\"');

            //================================================
            //any shader with generic values defined?
            vector<ShaderGenericValues> listShaderAndGenerics;
            string stringShaderAndgenericsValue = XkslangUtils::trim(remainingLine);
            if (stringShaderAndgenericsValue.size() > 0)
            {
                vector<ShaderParsingDefinition> listshaderDefinition;
                if (!XkslParser::ParseStringWithShaderDefinitions(stringShaderAndgenericsValue.c_str(), listshaderDefinition))
                    return error("convertAndLoad: failed to read the shaders and their generics value from: " + stringShaderAndgenericsValue);
                
                for (unsigned int is = 0; is < listshaderDefinition.size(); is++) {
                    listShaderAndGenerics.push_back(ShaderGenericValues(listshaderDefinition[is].shaderName, listshaderDefinition[is].genericsValue));
                }
            }
            else stringShaderAndgenericsValue = "";

            //================================================
            //Parse and convert the shader
            std::cout << "Parsing XKSL file \"" << xkslInputFile << "\"" << endl;

            SpxBytecode* spxBytecode = nullptr;
            vector<string> vecShadersParsed;
            if (useXkslangDll)
            {
#ifdef ALLOW_ACCESS_TO_XKSLANG_DLL
                //load and init the xksl file
                const string inputFname = inputDir + xkslInputFile;
                string xkslInput;
                if (!Utils::ReadFile(inputFname, xkslInput))
                {
                    error(" Failed to read the file: " + inputFname);
                    success = false; break;
                }
                if (singleXkslShaderToReturn != nullptr) delete[] singleXkslShaderToReturn;
                int len = (int)(xkslInput.size() + 1);
                singleXkslShaderToReturn = new char[len + 1];
                strncpy_s(singleXkslShaderToReturn, len + 1, xkslInput.c_str(), len);
                singleXkslShaderToReturn[xkslInput.size()] = 0;

                string stringMacroDef = "";
                for (unsigned int k = 0; k < listUserDefinedMacros.size(); ++k)
                    stringMacroDef += listUserDefinedMacros[k].macroName + " " + listUserDefinedMacros[k].macroValue + " ";

                string shaderName = xkslInput; //ShaderName is not really important here (callback function will return singleXkslShaderToReturn anyway)

                int bytecodeLength = 0;
                time_before = GetTickCount();
                uint32_t* pBytecodeBuffer = xkslangDll::ConvertXkslShaderToSPX(shaderName.c_str(), stringShaderAndgenericsValue.c_str(), stringMacroDef.c_str(),
                    callbackRequestDataForShaderDll_Single, &bytecodeLength);
                time_after = GetTickCount();

                if (pBytecodeBuffer == nullptr || bytecodeLength < 0)
                {
                    char* pError = xkslangDll::GetErrorMessages();
                    if (pError != nullptr) error(pError);
                    GlobalFree(pError);

                    error("convertAndLoadRecursif: failed to convert the xksl file name: " + xkslInputFile);
                    success = false; break;
                }
                else std::cout << " OK. time: " << (time_after - time_before) << "ms" << endl;

                spxBytecode = new SpxBytecode;
                listAllocatedBytecodes.push_back(spxBytecode);
                vector<uint32_t>& vecBytecode = spxBytecode->getWritableBytecodeStream();
                vecBytecode.assign(pBytecodeBuffer, pBytecodeBuffer + bytecodeLength);
                GlobalFree(pBytecodeBuffer);
#else
                return error("Unauthorized access to DLL functions");
#endif
            }
            else
            {
                spxBytecode = new SpxBytecode;
                listAllocatedBytecodes.push_back(spxBytecode);

                time_before = GetTickCount();
                success = ParseAndConvertXkslFile(parser, xkslInputFile, listShaderAndGenerics, listUserDefinedMacros, *spxBytecode);
                time_after = GetTickCount();

                if (success) std::cout << " OK. time: " << (time_after - time_before) << "ms" << endl;
                else {
                    error("convertAndLoad: failed to convert the xksl file name: " + xkslInputFile);
                    success = false; break;
                }
            }
            
#ifdef WRITE_BYTECODE_ON_DISK_AFTER_EVERY_MIXIN_STEPS
            {
                //Save the bytecode on the disk
                WriteBytecode(spxBytecode->getBytecodeStream(), outputDir, xkslInputFile + ".spv", BytecodeFileFormat::Binary);
                WriteBytecode(spxBytecode->getBytecodeStream(), outputDir, xkslInputFile + ".hr.spv", BytecodeFileFormat::Text);
            }
#endif

            //Query the list of shaders from the bytecode
            if (useXkslangDll)
            {
#ifdef ALLOW_ACCESS_TO_XKSLANG_DLL
                int countShaders = 0;
                xkslangDll::BytecodeShaderInformation *shadersInfo;
                uint32_t* pBytecodeBuffer = &(spxBytecode->getWritableBytecodeStream().front());
                int32_t bytecodeLength = spxBytecode->GetBytecodeSize();
                success = xkslangDll::GetBytecodeShadersInformation(pBytecodeBuffer, bytecodeLength, &shadersInfo, &countShaders);
                if (!success)
                {
                    char* pError = xkslangDll::GetErrorMessages();
                    if (pError != nullptr) error(pError);
                    GlobalFree(pError);

                    error("convertAndLoadRecursif: failed to get the shader info from the bytecode");
                    success = false; break;
                }

                for (int k = 0; k < countShaders; ++k)
                {
                    vecShadersParsed.push_back(shadersInfo[k].ShaderName);
                    GlobalFree(shadersInfo[k].ShaderName);
                }
                GlobalFree(shadersInfo);
#else
                return error("Unauthorized access to DLL functions");
#endif
            }
            else
            {
                if (!SpxMixer::GetListAllShadersFromBytecode(*spxBytecode, vecShadersParsed, errorMsgs))
                {
                    error("convertAndLoad: failed to get the list of shader names from: " + xkslInputFile);
                    success = false; break;
                }
            }

            for (unsigned int is = 0; is < vecShadersParsed.size(); ++is)
            {
                string shaderName = vecShadersParsed[is];
                if (!RecordSPXShaderBytecode(shaderName, spxBytecode, mapShaderNameBytecode))
                {
                    error("Can't add the shader into the bytecode: " + shaderName);
                    success = false; break;
                }
            }
        }
        else if (firstInstruction.compare("mixer") == 0)
        {
            string mixerName;
            if (!XkfxParser::GetNextInstruction(remainingLine, mixerName, remainingLine)) {
                error("mixer: failed to get the xksl file name");
                success = false; break;
            }
            mixerName = XkslangUtils::trim(mixerName, '\"');

            if (mixerName.find_first_of("<>()[].,+-/*\\?:;\"{}=&%^") != string::npos)
            {
                error("Invalid mixer name: " + mixerName);
                success = false; break;
            }

            EffectMixerObject* mixer = CreateAndAddNewMixer(mixerMap, mixerName, useXkslangDll);
            if (mixer == nullptr) {
                error("Failed to create a new mixer object");
                success = false; break;
            }
        }
        else
        {
            //mixer operation (mixer.instructions)
            string mixerName, instruction;
            if (!XkfxParser::SeparateAdotB(firstInstruction, mixerName, instruction)) {
                error("Unknown instruction: " + firstInstruction);
                success = false; break;
            }

            if (mixerMap.find(mixerName) == mixerMap.end()) {
                error(firstInstruction + ": no mixer found with the name:" + mixerName);
                success = false; break;
            }
            EffectMixerObject* mixerTarget = mixerMap[mixerName];

            //get the function parameters
            string instructionParametersStr;
            remainingLine = XkslangUtils::trim(remainingLine);
            if (remainingLine.size() > 0)
            {
                const char* paramStart;
                int paramLen;
                if (!XkfxParser::getFunctionParameterString(remainingLine.c_str(), &paramStart, &paramLen))
                    instructionParametersStr = "";
                else
                    instructionParametersStr = string(paramStart, paramLen);
            }

            if (instruction.compare("mixin") == 0)
            {
                if (instructionParametersStr.size() == 0) { error("Mixin: parameters expected"); success = false; break; }

                success = MixinShaders(effectName, mapShaderNameBytecode, mixerMap, listAllocatedBytecodes, listUserDefinedMacros, parser, useXkslangDll,
                    mixerTarget, instructionParametersStr, instructionFullLine, operationNum);

                if (!success) { error("Mixin failed"); success = false; break; }
            }
            else if (instruction.compare("addComposition") == 0)
            {
                if (instructionParametersStr.size() == 0) { error("addComposition: parameters expected"); success = false; break; }

                success = AddCompositionsToMixer(effectName, mapShaderNameBytecode, mixerMap,
                    listAllocatedBytecodes, listUserDefinedMacros, parser, useXkslangDll, operationNum,
                    mixerTarget, instructionParametersStr, "");

                if (!success) { error("Failed to add the compositions instruction to the mixer: " + instructionParametersStr); success = false; break; }
            }
            else if (instruction.compare("setStageEntryPoint") == 0)
            {
                //string& currentInstruction = listParsedInstructions[listParsedInstructions.size() - 1];
                //Utils::replaceAll(currentInstruction, "mixin ", "mixin( ");
                //currentInstruction += " )";
                //Utils::replaceAll(currentInstruction, compositionTargetStr, compositionTargetStr + " =");

                if (instructionParametersStr.size() == 0) { error("setStageEntryPoint: parameters expected"); success = false; break; }

                vector<string> entryPoints;
                if (!XkfxParser::SplitParametersString(instructionParametersStr.c_str(), entryPoints))
                    return error("failed to split the entryPoints parameters");

                for (unsigned int e = 0; e < entryPoints.size(); e++)
                {
                    const string& entryPointInstruction = entryPoints[e];

                    string stageStr;
                    string entryPointStr;
                    if (!XkfxParser::GetNextInstruction(entryPointInstruction, stageStr, entryPointStr, '=', false))
                        return error("\"=\" expected");
                    stageStr = XkslangUtils::trim(stageStr);
                    entryPointStr = XkslangUtils::trim(entryPointStr);
                    entryPointStr = XkslangUtils::trim(entryPointStr, '"');

                    ShadingStageEnum stage;
                    if (!GetShadingStageForString(stageStr, stage)) {
                        error("Unknown stage: " + stageStr);
                        success = false; break;
                    }

                    mixerTarget->stagesEntryPoints[(int)stage] = entryPointStr;
                }

                if (!success) break;
            }
            else if (instruction.compare("compile") == 0)
            {
                std::cout << endl;
                std::cout << "Compiling the mixer \"" << mixerName << "\"" << endl;

                vector<OutputStageBytecode> outputStages;
                for (auto its = mixerTarget->stagesEntryPoints.begin(); its != mixerTarget->stagesEntryPoints.end(); its++){
                    if (its->second.size() > 0)
                        outputStages.push_back(OutputStageBytecode(ShadingStageEnum(its->first), its->second));
                }

                if (useXkslangDll)
                {
                    success = CompileMixerUsingXkslangDll(effectName, mixerTarget, outputStages, errorMsgs);
                    if (!success) error("Failed to compile the effect: " + effectName);
                }
                else
                {
                    //Optionnal: get and display the list of all methods
                    if (!displayListOfAllMethodsForTheMixer(mixerTarget->mixer)) {
                        error("Failed to display the mixer list of compositions"); success = false;
                    }

                    //Optionnal: get and display all compositions before compiling
                    if (!displayListOfAllCompositionsForTheMixer(mixerTarget->mixer)) {
                        error("Failed to display the mixer list of compositions"); success = false;
                    }

                    success = CompileMixer(effectName, mixerTarget->mixer, outputStages, errorMsgs);
                    if (!success) error("Failed to compile the effect: " + effectName);
                }
            }
            else {
                error("Unknown instruction: " + instruction);
                success = false; break;
            }
        }

        if (!success) break;
    }

    if (previousPartialLine.size() > 0)
    {
        error("Incomplete instruction: " + previousPartialLine);
        success = false;
    }

    //Release allocated data
    {
        for (auto itm = mixerMap.begin(); itm != mixerMap.end(); itm++)
            delete (*itm).second;

        for (auto itv = listAllocatedBytecodes.begin(); itv != listAllocatedBytecodes.end(); itv++)
            delete (*itv);

        for (auto it = mapShaderData.begin(); it != mapShaderData.end(); ++it)
        {
            delete it->second;
        }
        mapShaderData.clear();

        if (singleXkslShaderToReturn != nullptr) {
            delete[] singleXkslShaderToReturn;
            singleXkslShaderToReturn = nullptr;
        }
    }

    if (errorMsgs.size() > 0)
    {
        std::cout << "   Messages:" << endl;
        for (unsigned int m = 0; m<errorMsgs.size(); m++) std::cout << "   " << errorMsgs[m] << "" << endl;
    }

    if (useXkslangDll)
    {
#ifdef ALLOW_ACCESS_TO_XKSLANG_DLL
        xkslangDll::ReleaseMixer();
#else
        return error("Unauthorized access to DLL functions");
#endif
    }

    effect_timeEnd = GetTickCount();
    std::cout << endl;
    std::cout << "Effect Total Time: " << (effect_timeEnd - effect_timeStart) << "ms" << endl;
    std::cout << "=================================" << endl;

    for (unsigned int l = 0; l < listParsedInstructions.size(); l++)
        updatedEffectCommandLines += listParsedInstructions[l] + "\n";

    return success;
}

static bool ProcessEffect(XkslParser* parser, XkfxEffectsToProcess& effect)
{
    string effectName = effect.effectName;

    std::cout << "=================================" << endl;
    std::cout << "=================================" << endl;
    std::cout << "Effect: " << effectName << endl;

    bool success1 = true;
    bool success2 = true;
    bool success3 = true;
    const string inputFname = inputDir + effect.inputFileName;
    string effectCmdLines;
    if (!Utils::ReadFile(inputFname, effectCmdLines))
    {
        error(" Failed to read the file: " + inputFname);
        return false;
    }

    //reset processing options
    xkfxOptions_automaticallyTryToLoadAndConvertUnknownMixinShader = false;
    xkfxOptions_processSampleWithXkfxLibrary = false;

    //reset some fields set by the xkfx commands
    libraryResourcesFolders.clear();
    libraryResourcesFolders.push_back(inputDir);
    shaderFilesPrefix = "";
    mapShaderNameData.clear();

    //Utils::replaceAll(effectCmdLines, "addComposition ", "addComposition = ");
    //Utils::WriteFile(inputFname, effectCmdLines);

    string updatedEffectCommandLines;
    if (processEffectWithDirectCallToXkslang)
    {
        std::cout << "=====================================================================" << endl;
        std::cout << "=====================================================================" << endl;
        std::cout << "Process XKSL File (direct call to Xkslang classes)" << endl;

        SpxMixer::StartMixinEffect();
        success1 = ProcessEffectCommandLine(parser, effectName, effectCmdLines, false, updatedEffectCommandLines);
        if (success1) std::cout << "Effect successfully processed." << endl;
        else error("Failed to process the effect");
        SpxMixer::StopMixinEffect();
        //if (success)
        //{
        //    Utils::WriteFile(inputFname, updatedEffectCommandLines);
        //}
    }

    if (processEffectWithDllApi)
    {
        std::cout << endl;
        std::cout << "=====================================================================" << endl;
        std::cout << "=====================================================================" << endl;
        std::cout << "Process XKSL File through Xkslang Dll API" << endl;

        success2 = ProcessEffectCommandLine(parser, effectName, effectCmdLines, true, updatedEffectCommandLines);
        if (success2) std::cout << "Effect successfully processed." << endl;
        else error("Failed to process the effect");
    }

    if (processEffectWithXkfxProcessorApi && xkfxOptions_processSampleWithXkfxLibrary)
    {
        std::cout << endl;
        std::cout << "=====================================================================" << endl;
        std::cout << "=====================================================================" << endl;
        std::cout << "Process XKSL File through Xkfx Parser classes" << endl;

        success3 = ProcessEffectCommandLineThroughXkfxParserApi(parser, effectName, effectCmdLines, callbackRequestDataForShader_XkfxParser);
        if (success3) std::cout << "Effect successfully processed." << endl;
        else error("Failed to process the effect");
    }

    bool success = success1 && success2 && success3;

    return success;
}

void main(int argc, char** argv)
{
#ifdef _DEBUG
    std::cout << "DEBUG mode" << endl << endl;
#else
    std::cout << "RELEASE mode" << endl << endl;
#endif

    if (!SetupTestDirectories())
    {
        error("Failed to setup the directories");
        return;
    }

    //Init parser
    XkslParser parser;
    if (!parser.InitialiseXkslang())
    {
        error("Failed to initialize the XkslParser");
        return;
    }

    /*{
        //To test XkfxParser before anything else
        XkfxEffectsToProcess effect = vecXkfxEffectToProcess[0];
        const string inputFname = inputDir + effect.inputFileName;
        string effectCmdLines;
        if (!Utils::ReadFile(inputFname, effectCmdLines))
        {
            error(" Failed to read the file: " + inputFname);
            return;
        }

        {
            std::cout << endl;
            std::cout << "=====================================================================" << endl;
            std::cout << "=====================================================================" << endl;
            std::cout << "Process XKSL File through Xkfx Parser classes" << endl;

            libraryResourcesFolders.push_back(inputDir);
            libraryResourcesFolders.push_back(inputDir + "library\\");
            shaderFilesPrefix = effect.effectName;
            for (int i = 0; i < 100; ++i)
            {
                vector<string> errorMsgs;
                bool success3 = ProcessEffectCommandLineThroughXkfxParserApi(&parser, effect.effectName, effectCmdLines, callbackRequestDataForShader);
                if (success3) std::cout << "Effect successfully processed." << endl;
                else {
                    std::cout << endl;
                    for (auto it = errorMsgs.begin(); it != errorMsgs.end(); it++) error(*it);
                    error("Failed to process the effect");
                }
            }
        }
    }*/

    //====================================================================================================================
    //====================================================================================================================
    std::cout << "___________________________________________________________________________________" << endl;
    std::cout << "Parse and convert XKSL Files:" << endl << endl;
    int countParsingProcessed = 0;
    int countParsingSuccessful = 0;
    vector<string> listXkslParsingFailed;
    //Parse the xksl files using XkslParser library
    {
        for (unsigned int n = 0; n < vecXkslFilesToConvert.size(); ++n)
        {
            countParsingProcessed++;
            bool success = true;

            XkslFilesToParseAndConvert& xkslFilesToParseAndConvert = vecXkslFilesToConvert[n];
            string xkslShaderInputFile = xkslFilesToParseAndConvert.fileName;
            vector<ShaderGenericValues> listGenericsValue;
            vector<XkslUserDefinedMacro> listUserDefinedMacros;

            // parse and convert all xksl files
            SpxBytecode spirXBytecode;
            success = ParseAndConvertXkslFile(&parser, xkslShaderInputFile, listGenericsValue, listUserDefinedMacros, spirXBytecode);

            if (success) countParsingSuccessful++;
            else listXkslParsingFailed.push_back(xkslShaderInputFile);

            std::cout << endl;
        }
    }

    //====================================================================================================================
    //====================================================================================================================
    std::cout << endl;
    std::cout << "___________________________________________________________________________________" << endl;
    std::cout << "Process XKFX Effect  Files:" << endl << endl;

    if (processEffectWithDllApi)
    {
#ifdef ALLOW_ACCESS_TO_XKSLANG_DLL
        xkslangDll::InitializeParser();
#else
        error("Unauthorized access to DLL functions");
#endif
    }

    int countEffectsProcessed = 0;
    int countEffectsSuccessful = 0;
    vector<string> listEffectsFailed;
    //Parse the effects
    {
        for (unsigned int n = 0; n < vecXkfxEffectToProcess.size(); ++n)
        {
            XkfxEffectsToProcess effect = vecXkfxEffectToProcess[n];
            countEffectsProcessed++;

            bool success = ProcessEffect(&parser, effect);

            if (success) countEffectsSuccessful++;
            else listEffectsFailed.push_back(effect.effectName);

            std::cout << endl;
        }
    }

    if (processEffectWithDllApi)
    {
#ifdef ALLOW_ACCESS_TO_XKSLANG_DLL
        xkslangDll::ReleaseParser();
#else
        error("Unauthorized access to DLL functions");
#endif
    }

    std::cout << endl;
    std::cout << "___________________________________________________________________________________" << endl;
    std::cout << "Results:" << endl << endl;

    //==========================================================
    std::cout << "Count XKSL files parsed: " << countParsingProcessed << endl;
    std::cout << "Count XKSL files successful: " << countParsingSuccessful << endl;
    if (listXkslParsingFailed.size() > 0)
    {
        std::cout << "  Failed XKSL Files:" << endl;
        for (unsigned int i = 0; i<listXkslParsingFailed.size(); ++i) std::cout << listXkslParsingFailed[i] << endl;
    }
    std::cout << endl;

    //==========================================================
    std::cout << "Count Effects processed: " << countEffectsProcessed << endl;
    std::cout << "Count Effects successful: " << countEffectsSuccessful << endl;
    if (listEffectsFailed.size() > 0)
    {
        std::cout << "  Failed Effects:" << endl;
        for (unsigned int i = 0; i<listEffectsFailed.size(); ++i) std::cout << listEffectsFailed[i] << endl;
    }
    std::cout << endl;

    parser.Finalize();

}

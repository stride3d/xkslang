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
#include "../source/XkslParser/XkslParser.h"
#include "../source/SpxMixer/OutputStageBytecode.h"
#include "../source/SpxMixer/SpxMixer.h"
#include "../source/SpxMixer/EffectReflection.h"

#include "../source/SPIRV-Cross/spirv_cross.hpp"
#include "../source/XkslangDLL/XkslangDLL.h"

using namespace std;
using namespace xkslangtest;
using namespace xkslang;

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
    SpxMixer* mixer;
    uint32_t mixerHandleId;
    unordered_map<int, string> stagesEntryPoints;

    EffectMixerObject(SpxMixer* mixer) : mixer(mixer), mixerHandleId(0) {}
    EffectMixerObject(uint32_t mixerHandleId) : mixer(nullptr), mixerHandleId(mixerHandleId) {}
    ~EffectMixerObject() { if (mixer != nullptr) delete mixer; }
};

static string inputDir = "glslang\\source\\Test\\xksl\\";
static string outputDir;
static string finalResultOutputDir;
static string expectedOutputDir;

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
static bool automaticallyTryToLoadAndConvertUnknownMixinShader = false;  //if true, when we mix an unknown shader, we will try to find, load and parse/convert this shader from our library

static bool buildEffectReflection = true;
static bool processEffectWithDirectCallToXkslang = true;
static bool processEffectWithDllApi = false;

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
    //{ "TestCompose10", "TestCompose10.xkfx" },
    //{ "TestCompose11", "TestCompose11.xkfx" },
    //{ "TestCompose12", "TestCompose12.xkfx" },
    //{ "TestCompose13", "TestCompose13.xkfx" },
    //{ "TestCompose14", "TestCompose14.xkfx" },
    //{ "TestCompose15", "TestCompose15.xkfx" },
    //{ "TestCompose16", "TestCompose16.xkfx" },
    //{ "TestCompose17", "TestCompose17.xkfx" },
    { "TestCompose18", "TestCompose18.xkfx" },
    
    //{ "TestForLoop", "TestForLoop.xkfx" },
    //{ "TestForEach01", "TestForEach01.xkfx" },
    //{ "TestForEach02", "TestForEach02.xkfx" },
    //{ "TestForEach03", "TestForEach03.xkfx" },
    //{ "TestForEach04", "TestForEach04.xkfx" },
    //{ "TestForEachCompose01", "TestForEachCompose01.xkfx" },
    //{ "TestForEachCompose02", "TestForEachCompose02.xkfx" },
    //{ "TestMergeStreams01", "TestMergeStreams01.xkfx" },
    
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
    
    //{ "SemanticTest01", "SemanticTest01.xkfx" },
    //{ "testTypeSize", "testTypeSize.xkfx" },
    
    //{ "EffectReflection01", "EffectReflection01.xkfx" },
    //{ "EffectReflection02", "EffectReflection02.xkfx" },
    //{ "EffectReflection03", "EffectReflection03.xkfx" },
    //{ "EffectReflection04", "EffectReflection04.xkfx" },
    //{ "EffectReflection05", "EffectReflection05.xkfx" },
    //{ "EffectReflection06", "EffectReflection06.xkfx" },
	//{ "EffectReflection07", "EffectReflection07.xkfx" },
    
    //{ "namespaces01", "namespaces01.xkfx" },
    //{ "testMacro01", "testMacro01.xkfx" },
    //{ "testVarKeyword01", "testVarKeyword01.xkfx" },
    //{ "userCustomType01", "userCustomType01.xkfx" },
    //{ "userCustomType02", "userCustomType02.xkfx" },
    
    //{ "ShadingBase", "ShadingBase.xkfx" },
    //{ "CustomEffect", "CustomEffect.xkfx" },
    //{ "BackgroundShader", "BackgroundShader.xkfx" },
    //{ "ComputeColorWave", "ComputeColorWave.xkfx" },
    //{ "TransformationBase", "TransformationBase.xkfx" },
    //{ "TransformationWAndVP", "TransformationWAndVP.xkfx" },
    //{ "DirectLightGroupArray", "DirectLightGroupArray.xkfx" },
    //{ "MaterialSurfaceStageCompositor", "MaterialSurfaceStageCompositor.xkfx" },
    //{ "NormalFromNormalMapping", "NormalFromNormalMapping.xkfx" },
    //{ "LightDirectionalGroup", "LightDirectionalGroup.xkfx" },
    //{ "MaterialSurfaceArray", "MaterialSurfaceArray.xkfx" },

    //{ "MaterialSurfacePixelStageCompositor", "MaterialSurfacePixelStageCompositor.xkfx" },
    ///{ "XenkoForwardShadingEffect", "XenkoForwardShadingEffect.xkfx" },
};

vector<XkfxEffectsToProcess> vecSpvFileToConvertToGlslAndHlsl = {
    //{ "TestConvert01", "TestForEach01_Pixel.spv" },
    //{ "TestCbuffer", "cbuffer.spv" },
    //{ "parseResources", "parseResources.spv" },
    //{ "test", "xcross.spv" },
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

static bool ConvertBytecodeToHlsl(const string& spvFile, const  string& outputFile)
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

static bool ConvertBytecodeToGlsl(const string& spvFile, const  string& outputFile)
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

/*static int ConvertSpvToShaderLanguage(string spvFile, string outputFile, ShaderLanguageEnum language)
{
    int argc;
    char **argv;
    
    if (language == ShaderLanguageEnum::GlslLanguage)
    {
        argc = 4;
        const char *args[] = {
            "spirv_cross.lib",
            "--output",
            outputFile.c_str(),
            spvFile.c_str()
        };
        argv = (char**)args;
    }
    else
    {
        argc = 7;
        const char *args[] = {
            "spirv_cross.lib",
            "--hlsl",
            "--shader-model",
            "40",
            "--output",
            outputFile.c_str(),
            spvFile.c_str()
        };
        argv = (char**)args;
    }

    return spirv_cross::SPIRV_CROSS::executeCmd(argc, argv);

    /// #ifdef _DEBUG
    ///     static string spirvCrossExeGlsl = "D:/Prgms/glslang/source/bin/spirv-cross/SPIRV-Cross_d.exe";
    ///     static string spirvCrossExeHlsl = "D:/Prgms/glslang/source/bin/spirv-cross/SPIRV-Cross_d.exe --hlsl";
    /// #else
    ///     static string spirvCrossExe = "D:/Prgms/glslang/source/bin/spirv-cross/SPIRV-Cross.exe";
    /// #endif

    /// STARTUPINFO si;
    /// PROCESS_INFORMATION pi;
    /// 
    /// ZeroMemory(&si, sizeof(si));
    /// si.cb = sizeof(si);
    /// ZeroMemory(&pi, sizeof(pi));
    /// 
    /// string exeInstr = (language == ShaderLanguageEnum::GlslLanguage)? spirvCrossExeGlsl : spirvCrossExeHlsl;
    /// string commandLine = exeInstr + string(" --output ") + outputFile + string(" ") + spvFile;
    /// 
    /// const char* cl = commandLine.c_str();
    /// wchar_t wtext[256];
    /// size_t size;
    /// errno_t error = mbstowcs_s(&size, wtext, cl, strlen(cl) + 1);
    /// LPWSTR ptr = wtext;
    /// 
    /// // Start the child process. 
    /// if (!CreateProcess(NULL,     // No module name (use command line)
    ///     ptr,                     // Command line
    ///     NULL,                    // Process handle not inheritable
    ///     NULL,                    // Thread handle not inheritable
    ///     FALSE,                   // Set handle inheritance to FALSE
    ///     0,                       // No creation flags
    ///     NULL,                    // Use parent's environment block
    ///     NULL,                    // Use parent's starting directory 
    ///     &si,                     // Pointer to STARTUPINFO structure
    ///     &pi)                     // Pointer to PROCESS_INFORMATION structure
    ///     )
    /// {
    ///     int error = GetLastError();
    ///     printf("CreateProcess failed (%d).\n", error);
    ///     return error;
    /// }
    /// 
    /// // Wait until child process exits.
    /// WaitForSingleObject(pi.hProcess, INFINITE);
    /// 
    /// // Close process and thread handles. 
    /// CloseHandle(pi.hProcess);
    /// CloseHandle(pi.hThread);
    /// 
    /// return 0;
}*/

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

static bool GetAndWriteMixerCurrentBytecode(EffectMixerObject* mixerTarget, const string& outputFileName, bool useXkslangDll)
{
    vector<uint32_t> mixinBytecode;
    if (useXkslangDll)
    {
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
                success = ConvertBytecodeToGlsl(outputFullnameSpv, fullNameGlsl);
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

                        string expectedOutputFullNameGlsl = expectedOutputDir + string(fileNameGlsl);
                        if (Utils::ReadFile(expectedOutputFullNameGlsl, glslExpectedOutput))
                        {
                            if (glslExpectedOutput.compare(glslConvertedOutput) != 0) {
                                std::cout << "expected output:" << endl << glslExpectedOutput;
                                std::cout << "output:" << endl << glslConvertedOutput;
                                std::cout << " Glsl output and expected output are different !!!" << endl;
                                someExpectedOutputsDifferent = true;
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
                success = ConvertBytecodeToHlsl(outputFullnameSpv, fullNameHlsl);
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
                                std::cout << "expected output:" << endl << hlslExpectedOutput;
                                std::cout << "output:" << endl << hlslConvertedOutput;
                                std::cout << " Hlsl output and expected output are different !!!" << endl;
                                someExpectedOutputsDifferent = true;
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

    if (someExpectedOutputsDifferent) success = false;
    if (someExpectedOutputsAreMissing) success = false;

    if (glslAllOutputs.size() > 0)
    {
        if (effectReflection != nullptr) glslAllOutputs = "/*\n" + effectReflection->Print() + "*/\n\n" + glslAllOutputs;
        string fileNameAllGlsl = effectName + ".glsl";
        string fullNameAllGlsl = finalResultOutputDir + fileNameAllGlsl;
        xkslangtest::Utils::WriteFile(fullNameAllGlsl, glslAllOutputs);
        std::cout << " output: \"" << fileNameAllGlsl << "\"" << endl;
    }
    if (hlslAllOutputs.size() > 0)
    {
        if (effectReflection != nullptr) hlslAllOutputs = "/*\n" + effectReflection->Print() + "*/\n\n" + hlslAllOutputs;
        string fileNameAllHlsl = effectName + ".hlsl";
        string fullNameAllHlsl = finalResultOutputDir + fileNameAllHlsl;
        xkslangtest::Utils::WriteFile(fullNameAllHlsl, hlslAllOutputs);
        std::cout << " output: \"" << fileNameAllHlsl << "\"" << endl;
    }

    return success;
}

static bool CompileMixerUsingXkslangDll(string effectName, EffectMixerObject* mixer, vector<OutputStageBytecode>& outputStages, vector<string>& errorMsgs)
{
    DWORD time_before, time_after;
    bool success = true;

    unsigned int countOutputStages = outputStages.size();
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
    SpvBytecode compiledBytecode;
    {
        int bytecodeLength = 0;
        uint32_t* pBytecodeBuffer = xkslangDll::GetMixerCompiledBytecode(mixer->mixerHandleId, &bytecodeLength);
        if (pBytecodeBuffer == nullptr || bytecodeLength <= 0) {
            error("Failed to get the mixin compiled bytecode");
            success = false;
        }

        vector<uint32_t>& vecBytecode = compiledBytecode.getWritableBytecodeStream();
        vecBytecode.assign(pBytecodeBuffer, pBytecodeBuffer + bytecodeLength);
        if (pBytecodeBuffer != nullptr) GlobalFree(pBytecodeBuffer);

        string outputFileNameFinalSpv = effectName + "_compile4_final.spv";
        string outputFileNameFinalSpvHr = effectName + "_compile4_final.hr.spv";
        WriteBytecode(compiledBytecode.getBytecodeStream(), outputDir, outputFileNameFinalSpv, BytecodeFileFormat::Binary);
        WriteBytecode(compiledBytecode.getBytecodeStream(), outputDir, outputFileNameFinalSpvHr, BytecodeFileFormat::Text);
    }

    if (!success) return false;

    //get the reflection data from the compiled bytecode
    EffectReflection effectReflection;
    if (buildEffectReflection)
    {
        /*xkslangDll::ConstantBufferReflectionDescriptionData* constantBuffers;
        xkslangDll::EffectResourceBindingDescriptionData* resourceBindings;
        xkslangDll::ShaderInputAttributeDescriptionData* inputAttributes;
        int32_t countConstantBuffers, countResourceBindings, countInputAttributes;

        success = xkslangDll::GetMixerEffectReflectionData(mixer->mixerHandleId,
            &constantBuffers, &countConstantBuffers,
            &resourceBindings, &countResourceBindings,
            &inputAttributes, &countInputAttributes);

        if (!success)
        {
            char* pError = xkslangDll::GetErrorMessages();
            if (pError != nullptr) error(pError);
            GlobalFree(pError);

            error("Failed to get the Effect Reflection data");
            return false;
        }*/

        //TMP: directly call xkslang classes
        success = SpxMixer::GetCompiledBytecodeReflection(compiledBytecode, effectReflection, errorMsgs);
        if (!success)
        {
            error("Failed to get the reflection data from the compiled bytecode");
            return false;
        }
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
}

static bool CompileMixer(string effectName, SpxMixer* mixer, vector<OutputStageBytecode>& outputStages, vector<string>& errorMsgs)
{
    DWORD time_before, time_after;
    bool success = true;

    SpvBytecode composedSpv;
    SpvBytecode streamsMergedSpv;
    SpvBytecode streamsReshuffledSpv;
    SpvBytecode mergedCBuffersSpv;
    SpvBytecode compiledBytecode;
    //SpvBytecode errorSpv;
    std::cout << "Compile Mixin: ";
    time_before = GetTickCount();
    success = mixer->Compile(outputStages, errorMsgs, &composedSpv, &streamsMergedSpv, &streamsReshuffledSpv, &mergedCBuffersSpv, &compiledBytecode, nullptr);
    time_after = GetTickCount();

    if (success) std::cout << "OK. time: " << (time_after - time_before) << "ms" << endl;
    else {
        error("Compilation Failed");
        //WriteBytecode(errorSpv, outputDir, effectName + "_compile_error.hr.spv", BytecodeFileFormat::Text);
    }

    //output the compiled intermediary bytecodes
    WriteBytecode(composedSpv.getBytecodeStream(), outputDir, effectName + "_compile0_composed.hr.spv", BytecodeFileFormat::Text);
    WriteBytecode(streamsMergedSpv.getBytecodeStream(), outputDir, effectName + "_compile1_streamsMerged.hr.spv", BytecodeFileFormat::Text);
    WriteBytecode(streamsReshuffledSpv.getBytecodeStream(), outputDir, effectName + "_compile2_streamsReshuffled.hr.spv", BytecodeFileFormat::Text);
    WriteBytecode(mergedCBuffersSpv.getBytecodeStream(), outputDir, effectName + "_compile3_mergedCBuffers.hr.spv", BytecodeFileFormat::Text);

    {
        //write the final SPIRV bytecode then convert it into GLSL
        string outputFileNameFinalSpv = effectName + "_compile4_final.spv";
        string outputFileNameFinalSpvHr = effectName + "_compile4_final.hr.spv";
        WriteBytecode(compiledBytecode.getBytecodeStream(), outputDir, outputFileNameFinalSpv, BytecodeFileFormat::Binary);
        WriteBytecode(compiledBytecode.getBytecodeStream(), outputDir, outputFileNameFinalSpvHr, BytecodeFileFormat::Text);
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
     SpxBytecode& spirXBytecode, bool writeOutputsOnDisk)
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
                return true;
            }
        }
    }
    
    error("Callback function: Cannot find any data for the shader: " + shaderName);
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
        *dataLen = strlen(pShaderData);
        return pShaderData;
    }
    else
    {
        string shaderData;
        bool success = callbackRequestDataForShader(shaderNameStr, shaderData);
        if (!success) return nullptr;

        int len = shaderData.size();
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
    *dataLen = strlen(singleXkslShaderToReturn);
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

static bool SeparateAdotB(const string str, string& A, string& B)
{
    size_t pdot = str.find_first_of('.');
    if (pdot == string::npos) return false;
    A = str.substr(0, pdot);
    B = str.substr(pdot + 1);
    return true;
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

static bool splitPametersString(const string& parameterStr, vector<string>& parameters)
{
    const char* ptrStr = parameterStr.c_str();
    int len = parameterStr.size();

    char c;
    int start = 0;
    int countBrackets = 0;
    while (true)
    {
        while (ptrStr[start] == ' ' || ptrStr[start] == ',') start++;
        if (start >= len) return true;

        int end = start;
        bool loop = true;
        while (loop)
        {
            if (end == len) {
                end--;
                break;
            }

            c = ptrStr[end++];
            switch (c)
            {
                case '(': countBrackets++; break;
                case ')': countBrackets--; break;
                case ',':
                {
                    if (countBrackets == 0) {
                        loop = false;
                        end--;
                    }
                    break;
                }
            }
        }

        int lastChar = end;
        while (ptrStr[lastChar] == ' ' || ptrStr[lastChar] == ',') lastChar--;
        
        parameters.push_back(parameterStr.substr(start, (lastChar - start) + 1));

        start = end + 1;
        if (start >= len) return true;
    }
}

//return the string between the "()" brackets
static bool getFunctionParameterString(const string& instruction, string& parameterStr, bool leftBracketExpected)
{
    int len = instruction.size();

    int firstCharPos = 0;
    if (leftBracketExpected)
    {
        size_t first = (int)instruction.find_first_of('(');
        if (first == string::npos) return false;
        firstCharPos = first + 1;
    }
    if (firstCharPos >= len - 1) return false;

    int rightBracketPos = firstCharPos;
    int countBracketToPass = 0;
    while (rightBracketPos < len)
    {
        char c = instruction[rightBracketPos];
        if (c == ')')
        {
            if (countBracketToPass == 0) break;
            else countBracketToPass--;
        }
        else if (c == '(') countBracketToPass++;
        rightBracketPos++;
    }

    if (rightBracketPos == firstCharPos || rightBracketPos == len) return false;
    int lastCharPos = rightBracketPos - 1;

    parameterStr = Utils::trim(instruction.substr(firstCharPos, (lastCharPos - firstCharPos) + 1));
    return true;
}

static bool getNextWord(stringstream& stream, string& word)
{
    while (stream.peek() == ' ') stream.get(); // skip front spaces

    if (!getline(stream, word, ' ')) {
        return false;
    }

    return true;
}

static bool getNextWord(stringstream& stream, const string& delimiters, string& word)
{
    word = "";
    while (stream.peek() == ' ') stream.get(); // skip front spaces

    unsigned int countDelimiters = delimiters.size();
    char c;
    while (stream.get(c))
    {
        for (unsigned int d = 0; d < countDelimiters; d++)
            if (c == delimiters[d]) return true;
        word += c;
    }

    return true;
}

static bool getNextWord(string& str, string& word)
{
    int len = str.size();
    if (len == 0) return false;
    const char* strPtr = str.c_str();
    int start = 0;

    while (strPtr[start] == ' ') start++; // skip front spaces
    if (start >= len) return false;
    int end = start;

    while (end < len)
    {
        if (strPtr[end] == ' ') {
            word = str.substr(start, (end - start));
            str = str.substr(end);
            return true;
        }
        end++;
    }

    word = str.substr(start);
    word = "";
    return true;
}

static bool getNextWord(const string& str, const string& delimiters, string& word, string& remainingStr)
{
    int len = str.size();
    if (len == 0) return false;
    const char* strPtr = str.c_str();
    int start = 0;
    
    while (strPtr[start] == ' ') start++; // skip front spaces
    if (start >= len) return false;
    int end = start;

    unsigned int countDelimiters = delimiters.size();
    char c;
    while (end < len)
    {
        c = strPtr[end];
        for (unsigned int d = 0; d < countDelimiters; d++)
        {
            if (c == delimiters[d]){
                word = str.substr(start, (end - start));
                remainingStr = str.substr(end + 1);
                return true;
            }
        }
        end++;
    }

    word = str.substr(start);
    remainingStr = "";
    return true;
}

static string getShaderUnmangledName(const string& shaderFullName)
{
    size_t pos = shaderFullName.find_first_of('<');
    if (pos == string::npos) return shaderFullName;
    return shaderFullName.substr(0, pos);
}

static SpxBytecode* GetSpxBytecodeForShader(string shaderName, string& shaderFullName, unordered_map<string, SpxBytecode*>& mapShaderNameWithBytecode, bool canLookIfUnmangledNameMatch)
{
    auto it = mapShaderNameWithBytecode.find(shaderName);
    if (it != mapShaderNameWithBytecode.end())
    {
        SpxBytecode* spxBytecode = it->second;
        shaderFullName = it->first;
        return spxBytecode;
    }

    if (canLookIfUnmangledNameMatch)
    {
        SpxBytecode* spxBytecode = nullptr;
        for (auto it = mapShaderNameWithBytecode.begin(); it != mapShaderNameWithBytecode.end(); it++)
        {
            string anUnmangledShaderName = getShaderUnmangledName(it->first);
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
    
static bool RecordSPXShaderBytecode(string shaderFullName, SpxBytecode* spxBytecode, unordered_map<string, SpxBytecode*>& mapShaderNameWithBytecode)
{
    //auto it = mapShaderNameWithBytecode.find(fullShaderName);
    //if (it != mapShaderNameWithBytecode.end()) {
    //    error("Shader is already recorded in the map: " + fullShaderName);
    //    return false;
    //}

    mapShaderNameWithBytecode[shaderFullName] = spxBytecode;
    return true;
}

static bool ConvertAndLoadRecursif(const string& effectName, unordered_map<string, SpxBytecode*>& mapShaderNameWithBytecode, vector<SpxBytecode*>& listAllocatedBytecodes,
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

            error("convertAndLoadRecursif: failed to convert the XKSL file name: " + xkslInputFilePrefix);
            return false;
        }
        else std::cout << " OK. time: " << (time_after - time_before) << "ms" << endl;

        spxBytecode = new SpxBytecode;
        listAllocatedBytecodes.push_back(spxBytecode);
        vector<uint32_t>& vecBytecode = spxBytecode->getWritableBytecodeStream();
        vecBytecode.assign(pBytecodeBuffer, pBytecodeBuffer + bytecodeLength);
        if (pBytecodeBuffer != nullptr) GlobalFree(pBytecodeBuffer);
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

    //Save the bytecode on the disk
    {
        string prefix = xkslInputFilePrefix;
        if (prefix.size() == 0) prefix = effectName;
        WriteBytecode(spxBytecode->getBytecodeStream(), outputDir, prefix + "_" + shaderName + ".spv", BytecodeFileFormat::Binary);
        WriteBytecode(spxBytecode->getBytecodeStream(), outputDir, prefix + "_" + shaderName + ".hr.spv", BytecodeFileFormat::Text);
    }

    //Query the list of shaders from the bytecode
    if (useXkslangDll)
    {
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

            error("convertAndLoadRecursif: failed to get the shader info from the bytecode");
            return false;
        }

        for (int k = 0; k < countShaders; ++k)
        {
            vecShadersParsed.push_back(shadersInfo[k].ShaderName);
            GlobalFree(shadersInfo[k].ShaderName);
        }
        GlobalFree(shadersInfo);
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
        if (!RecordSPXShaderBytecode(shaderName, spxBytecode, mapShaderNameWithBytecode))
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
        uint32_t mixerHandleId = xkslangDll::CreateSpxShaderMixer();
        if (mixerHandleId == 0) {
            error("Failed to create a new spx mixer");
            return nullptr;
        }
        mixerObject = new EffectMixerObject(mixerHandleId);
    }
    else
    {
        SpxMixer* mixer = new SpxMixer();
        mixerObject = new EffectMixerObject(mixer);
    }

    if (mixerObject == nullptr) return nullptr;
    mixerMap[newMixerName] = mixerObject;

    return mixerObject;
}

static bool MixinShaders(const string& effectName, unordered_map<string, SpxBytecode*>& mapShaderNameWithBytecode,
    vector<SpxBytecode*>& listAllocatedBytecodes, const vector<XkslUserDefinedMacro>& listUserDefinedMacros, XkslParser* parser, bool useXkslangDll,
    EffectMixerObject* mixerTarget, const string& stringShaderAndgenericsValue,
    const string mixinOperationInstructionLineLog, int& operationNum)
{
    std::cout << endl;
    std::cout << "===========================" << endl;
    std::cout << "Process mixin instructions: \"" << mixinOperationInstructionLineLog << "\"" << endl;

    if (stringShaderAndgenericsValue.size() == 0) return error("No shader to mix");
    bool success = true;

    //================================================
    //Parse and get the list of shader defintion
    vector<ShaderParsingDefinition> listshaderDefinition;
    if (!XkslParser::ParseStringWithShaderDefinitions(stringShaderAndgenericsValue.c_str(), listshaderDefinition))
        return error("mixin: failed to parse the shaders definition from: " + stringShaderAndgenericsValue);
    vector<ShaderGenericValues> listShaderAndGenerics;
    for (unsigned int is = 0; is < listshaderDefinition.size(); is++) {
        listShaderAndGenerics.push_back(ShaderGenericValues(listshaderDefinition[is].shaderName, listshaderDefinition[is].genericsValue));
    }
    if (listShaderAndGenerics.size() == 0) return error("mixin: list of shader is empty");
    string shaderName = listShaderAndGenerics[0].shaderName;

    //================================================
    //Get the bytecode file and the fullName of all shader to mix into the mixer
    vector<pair<string, SpxBytecode*>> listShaderBytecodeToMix; //list of shaders to mix, and their corresponding bytecode
    {
        for (auto its = listShaderAndGenerics.begin(); its != listShaderAndGenerics.end(); its++)
        {
            const ShaderGenericValues& shaderAndGenerics = *its;
            string shaderName = shaderAndGenerics.GetName();
            string shaderFullName;   //we can omit to specify the generics when mixin a shader, we will search the best match

            SpxBytecode* shaderBytecode = GetSpxBytecodeForShader(shaderName, shaderFullName, mapShaderNameWithBytecode, true);
            if (shaderBytecode == nullptr)
            {
                if (automaticallyTryToLoadAndConvertUnknownMixinShader)
                {
                    //the shader bytecode does not exist, but we can try to find and generate it
                    success = ConvertAndLoadRecursif(effectName, mapShaderNameWithBytecode, listAllocatedBytecodes,
                        shaderName, "", listUserDefinedMacros,
                        parser, useXkslangDll);

                    if (!success)
                    {
                        error("Failed to recursively convert and load the shaders: " + shaderName);
                        return false;
                    }

                    shaderBytecode = GetSpxBytecodeForShader(shaderName, shaderFullName, mapShaderNameWithBytecode, true);
                }

                if (shaderBytecode == nullptr)
                {
                    error("cannot find or generate a bytecode for the shader: " + shaderName);
                    return false;
                }
            }

            /*if (spxBytecode == nullptr) spxBytecode = aShaderBytecode;
            else
            {
                if (spxBytecode != aShaderBytecode) {
                    error("2 shaders to mix are defined in different bytecode (maybe we could merge them)");
                    return false;
                }
            }*/

            listShaderBytecodeToMix.push_back(pair<string, SpxBytecode*>(shaderFullName, shaderBytecode));
        }
    }

    unsigned int countShadersToMix = listShaderBytecodeToMix.size();
    if (countShadersToMix == 0) { error("No bytecode to mix"); return false; }

    //Previous version could mix several shaders at once into the mixer (provided that all shaders are converted into the same bytecode)
    //We now mix one shader after the previous one (to fix the same bytecode issue)
    //(also mixin shaders all-together won't give the same result at mixin one after another one)
    for (unsigned int cs = 0; cs < countShadersToMix; cs++)
    {
        string shaderToMix = listShaderBytecodeToMix[cs].first;
        SpxBytecode* shaderBytecode = listShaderBytecodeToMix[cs].second;
        vector<string> listShaderToMix = { shaderToMix }; //mixer can accept a list of shaders to mix (we disable it for now)

        //=====================================================
        //Mixin the bytecode into the mixer
        DWORD time_before, time_after;
        vector<string> errorMsgs;
        std::cout << endl;
        std::cout << "mixin: " << shaderToMix << endl;
        if (useXkslangDll)
        {
            if (listShaderToMix.size() == 0) { error("not shader to mix"); return false; }

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
        }
        else
        {
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

        //write the mixer current bytecode
        string outputFileName = effectName + "_op" + to_string(operationNum++) + "_mixin" + ".hr.spv";
        success = GetAndWriteMixerCurrentBytecode(mixerTarget, outputFileName, useXkslangDll);
        if (!success) {
            error("Failed to get and write the mixer current bytecode");
            return false;
        }
    }

    return true;
}

static bool AddCompositionToMixer(const string& effectName, unordered_map<string, SpxBytecode*>& mapShaderNameWithBytecode, unordered_map<string, EffectMixerObject*>& mixerMap,
    vector<SpxBytecode*>& listAllocatedBytecodes, const vector<XkslUserDefinedMacro>& listUserDefinedMacros, XkslParser* parser, bool useXkslangDll, int& operationNum,
    EffectMixerObject* mixerTarget, const string& compositionString)
{
    bool success = false;
    DWORD time_before, time_after;

    //===================================================
    //composition variable target
    string compositionStr = compositionString;
    string compositionVariableTargetStr;
    if (!getNextWord(compositionStr, compositionVariableTargetStr))
        return error("AddCompositionToMixer: Failed to find the composition variable target from: " + compositionStr);
    compositionVariableTargetStr = Utils::trim(compositionVariableTargetStr);

    //We can either have shader.variableName, or only a variableName
    string shaderName, variableName;
    if (!SeparateAdotB(compositionVariableTargetStr, shaderName, variableName))
    {
        variableName = compositionVariableTargetStr;
        shaderName = "";
    }

    //===================================================
    //Expecting '='
    string tmpStr;
    if (!getNextWord(compositionStr, "=", tmpStr, compositionStr)) return error("\"=\" expected");
    if (Utils::trim(tmpStr).size() > 0) return error("Invalid assignation format");

    //===================================================
    //Find or create the composition source mixer
    //We can either have a mixer name, or a mixin instruction
    const string mixerSourceNameOrInstructionStr = Utils::trim(compositionStr);

    EffectMixerObject* mixerSource = nullptr;
    if (Utils::startWith(mixerSourceNameOrInstructionStr, "mixin("))
    {
        const string mixinInstructionStr = mixerSourceNameOrInstructionStr;

        //We create a new, anonymous mixer and directly mix the shader specified in the function parameter
        string anonymousMixerInstruction;
        if (!getFunctionParameterString(mixinInstructionStr, anonymousMixerInstruction, true)) {
            return error("addComposition: Failed to get the instuction parameter from: \"" + mixinInstructionStr + "\"");
        }

        //Create the anonymous mixer
        string anonymousMixerName = "_anonMixer_" + to_string(mixerMap.size());
        EffectMixerObject* anonymousMixer = CreateAndAddNewMixer(mixerMap, anonymousMixerName, useXkslangDll);
        if (anonymousMixer == nullptr) {
            return error("addComposition: Failed to create a new mixer object");
        }

        //Mix the new mixer with the shaders specified in the function parameter
        success = MixinShaders(effectName, mapShaderNameWithBytecode, listAllocatedBytecodes, listUserDefinedMacros, parser, useXkslangDll,
            anonymousMixer, anonymousMixerInstruction, mixinInstructionStr, operationNum);
        if (!success) return error("Mixin failed: " + mixinInstructionStr);

        mixerSource = anonymousMixer;
    }
    else
    {
        const string mixerName = mixerSourceNameOrInstructionStr;

        //find the mixer in our mixer map
        if (mixerMap.find(mixerName) == mixerMap.end()) {
            return error("addComposition: no mixer found with the name:" + mixerName);
        }
        mixerSource = mixerMap[mixerName];
    }

    if (mixerSource == nullptr) {
        return error("addComposition: no mixer source to make the composition");
    }

    //=====================================
    // Add the composition to the mixer
    std::cout << "Adding composition: \"" << compositionString << "\"" << endl;

    if (useXkslangDll)
    {
        time_before = GetTickCount();
        success = xkslangDll::AddComposition(mixerTarget->mixerHandleId, shaderName.c_str(), variableName.c_str(), mixerSource->mixerHandleId);
        time_after = GetTickCount();
    }
    else
    {
        vector<string> errorMsgs;
        time_before = GetTickCount();
        success = mixerTarget->mixer->AddComposition(shaderName, variableName, mixerSource->mixer, errorMsgs);
        time_after = GetTickCount();

        if (!success)
        {
            for (unsigned int k = 0; k < errorMsgs.size(); k++) error(errorMsgs[k]);
        }
    }

    if (success) std::cout << " OK. Time:  " << (time_after - time_before) << "ms" << endl;
    else return error("Failed to add the composition to the mixer");

    //write the mixer current bytecode
    string outputFileName = effectName + "_op" + to_string(operationNum++) + "_compose" + ".hr.spv";
    success = GetAndWriteMixerCurrentBytecode(mixerTarget, outputFileName, useXkslangDll);
    if (!success) return error("Failed to get and write the mixer current bytecode");

    return true;
}

static bool AddCompositionsToMixer(const string& effectName, unordered_map<string, SpxBytecode*>& mapShaderNameWithBytecode, unordered_map<string, EffectMixerObject*>& mixerMap,
    vector<SpxBytecode*>& listAllocatedBytecodes, const vector<XkslUserDefinedMacro>& listUserDefinedMacros, XkslParser* parser, bool useXkslangDll, int& operationNum,
    EffectMixerObject* mixerTarget, const string& compositionsString)
{
    std::cout << endl;
    std::cout << "====================================" << endl;
    std::cout << "Process AddCompositions instructions: \"" << compositionsString << "\"" << endl;

    //split the string
    vector<string> compositions;
    if (!splitPametersString(compositionsString, compositions))
        return error("failed to split the parameters");

    if (compositions.size() == 0)
        return error("No composition found in the string: " + compositionsString);

    for (unsigned int k = 0; k < compositions.size(); ++k)
    {
        const string& compositionStr = compositions[k];
        bool success = AddCompositionToMixer(effectName, mapShaderNameWithBytecode, mixerMap,
            listAllocatedBytecodes, listUserDefinedMacros, parser, useXkslangDll, operationNum,
            mixerTarget, compositionStr);

        if (!success)
            return error("Failed to add the composition into the mixer: " + compositionStr);
    }

    return true;
}

static bool ProcessEffectCommandLine(XkslParser* parser, string effectName, string effectCmdLines, bool useXkslangDll, string& updatedEffectCommandLines)
{
    bool success = true;
    updatedEffectCommandLines = "";

    if (useXkslangDll)
    {
        xkslangDll::InitializeMixer();
    }

    vector<string> errorMsgs;
    DWORD time_before, time_after;
    vector<SpxBytecode*> listAllocatedBytecodes;
    unordered_map<string, SpxBytecode*> mapShaderNameWithBytecode;
    unordered_map<string, EffectMixerObject*> mixerMap;
    int operationNum = 0;

    //init library resource folders
    libraryResourcesFolders.clear();
    libraryResourcesFolders.push_back(inputDir);

    vector<string> listParsedInstructions;
    vector<XkslUserDefinedMacro> listUserDefinedMacros;

    string instructionFullLine, lineItem;
    stringstream ss(effectCmdLines);
    while (getline(ss, instructionFullLine, '\n'))
    {
        listParsedInstructions.push_back(instructionFullLine);

        instructionFullLine = Utils::trim(instructionFullLine);
        if (instructionFullLine.size() == 0) continue;

        stringstream lineSs(instructionFullLine);
        getNextWord(lineSs, " (", lineItem);

        if (lineItem.size() >= 2 && lineItem[0] == '/' && lineItem[1] == '/')
        {
            //a comment: ignore the line
        }
        else if (lineItem.compare("break") == 0)
        {
            //quit parsing the effect
            break;
        }
        else if (lineItem.compare("set") == 0)
        {
            string parameterName;
            if (!getNextWord(lineSs, parameterName)) {
                error("set: failed to get the parameter name");
                success = false; break;
            }

            if (parameterName.compare("automaticallyTryToLoadAndConvertUnknownMixinShader") == 0)
            {
                string parameterValue;
                if (!getNextWord(lineSs, parameterValue)) {
                    error("set: failed to get the parameter value");
                    success = false; break;
                }
                if (parameterValue.compare("true") == 0) automaticallyTryToLoadAndConvertUnknownMixinShader = true;
                else automaticallyTryToLoadAndConvertUnknownMixinShader = false;
            }
            else
            {
                error("set: unknown parameter name: " + parameterName);
                success = false; break;
            }
        }
        else if (lineItem.compare("addResourcesLibrary") == 0)
        {
            string folder;
            if (!getNextWord(lineSs, folder)) {
                error("addResourcesLibrary: failed to get the library resource folder");
                success = false; break;
            }
            folder = Utils::trim(folder, '\"');

            string path = inputDir + folder + "\\";
            libraryResourcesFolders.push_back(path);
        }
        else if (lineItem.compare("setDefine") == 0)
        {
            string strMacrosDefinition;
            if (!getline(lineSs, strMacrosDefinition)) {
                error("Fails to get the macros definition");
                success = false; break;
            }

            if (XkslParser::ParseStringMacroDefinition(strMacrosDefinition.c_str(), listUserDefinedMacros, false) != 1)
            {
                error("Fails to parse the macros definition: " + strMacrosDefinition);
                success = false; break;
            }
        }
        else if (lineItem.compare("convertAndLoadRecursif") == 0)
        {
            /// recursively convert and load xksl shaders

            //================================================
            //Parse the command line parameters
            string stringShaderAndgenericsValue;
            if (!getline(lineSs, stringShaderAndgenericsValue)) {
                error("convertAndLoadRecursif: failed to get the XKSL file parameters");
                success = false; break;
            }
            stringShaderAndgenericsValue = Utils::trim(stringShaderAndgenericsValue, ' ');
            if (stringShaderAndgenericsValue.size() == 0) {
                error("convertAndLoadRecursif: failed to get the XKSL shader parameters");
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
                xkslInputFilePrefix = Utils::trim(xkslInputFilePrefix, '\"');
                stringShaderAndgenericsValue = stringShaderAndgenericsValue.substr(indexEnd + 1);
            }

            //Can convert and load the shader
            success = ConvertAndLoadRecursif(effectName, mapShaderNameWithBytecode, listAllocatedBytecodes,
                stringShaderAndgenericsValue, xkslInputFilePrefix, listUserDefinedMacros,
                parser, useXkslangDll);

            if (!success)
            {
                error("Failed to recursively convert and load the shaders: " + stringShaderAndgenericsValue);
                success = false; break;
            }
        }
        else if (lineItem.compare("convertAndLoad") == 0)
        {
            /// convert and load xksl shaders

            //================================================
            //Parse the command line parameters
            string xkslInputFile;
            if (!getNextWord(lineSs, xkslInputFile)) {
                error("convertAndLoad: failed to get the XKSL file name");
                success = false; break;
            }
            xkslInputFile = Utils::trim(xkslInputFile, '\"');

            //================================================
            //any shader with generic values defined?
            vector<ShaderGenericValues> listShaderAndGenerics;
            string stringShaderAndgenericsValue;
            if (getline(lineSs, stringShaderAndgenericsValue))
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
                //load and init the xksl file
                const string inputFname = inputDir + xkslInputFile;
                string xkslInput;
                if (!Utils::ReadFile(inputFname, xkslInput))
                {
                    error(" Failed to read the file: " + inputFname);
                    success = false; break;
                }
                if (singleXkslShaderToReturn != nullptr) delete[] singleXkslShaderToReturn;
                int len = xkslInput.size() + 1;
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
            }
            else
            {
                spxBytecode = new SpxBytecode;
                listAllocatedBytecodes.push_back(spxBytecode);

                time_before = GetTickCount();
                success = ParseAndConvertXkslFile(parser, xkslInputFile, listShaderAndGenerics, listUserDefinedMacros, *spxBytecode, true);
                time_after = GetTickCount();

                if (success) std::cout << " OK. time: " << (time_after - time_before) << "ms" << endl;
                else {
                    error("convertAndLoad: failed to convert the xksl file name: " + xkslInputFile);
                    success = false; break;
                }
            }
            
            //Save the bytecode on the disk
            {
                WriteBytecode(spxBytecode->getBytecodeStream(), outputDir, xkslInputFile + ".spv", BytecodeFileFormat::Binary);
                WriteBytecode(spxBytecode->getBytecodeStream(), outputDir, xkslInputFile + ".hr.spv", BytecodeFileFormat::Text);
            }

            //Query the list of shaders from the bytecode
            if (useXkslangDll)
            {
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
                if (!RecordSPXShaderBytecode(shaderName, spxBytecode, mapShaderNameWithBytecode))
                {
                    error("Can't add the shader into the bytecode: " + shaderName);
                    success = false; break;
                }
            }
        }
        else if (lineItem.compare("mixer") == 0)
        {
            string mixerName;
            if (!getNextWord(lineSs, mixerName)) {
                error("mixer: failed to get the xksl file name");
                success = false; break;
            }
            mixerName = Utils::trim(mixerName, '\"');

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
            if (!SeparateAdotB(lineItem, mixerName, instruction)) {
                error("Unknown instruction: " + lineItem);
                success = false; break;
            }

            if (mixerMap.find(mixerName) == mixerMap.end()) {
                error(lineItem + ": no mixer found with the name:" + mixerName);
                success = false; break;
            }
            EffectMixerObject* mixerTarget = mixerMap[mixerName];

            if (instruction.compare("mixin") == 0)
            {
                string lineRemainingStr;
                if (!getline(lineSs, lineRemainingStr))
                {
                    error("mixin: Failed to read the line");
                    success = false; break;
                }

                string mixinInstructionsStr;
                if (!getFunctionParameterString(lineRemainingStr, mixinInstructionsStr, false)) {
                    error("mixin: Failed to get the mixin instuction parameters from: \"" + instructionFullLine + "\"");
                    success = false; break;
                }

                success = MixinShaders(effectName, mapShaderNameWithBytecode, listAllocatedBytecodes, listUserDefinedMacros, parser, useXkslangDll,
                    mixerTarget, mixinInstructionsStr, instructionFullLine, operationNum);

                if (!success) { error("Mixin failed"); success = false; break; }
            }
            else if (instruction.compare("addComposition") == 0)
            {
                string lineRemainingStr;
                if (!getline(lineSs, lineRemainingStr)) {
                    error("addComposition: Failed to read the line");
                    success = false; break;
                }

                string compositionsInstructionsStr;
                if (!getFunctionParameterString(lineRemainingStr, compositionsInstructionsStr, false)) {
                    error("addComposition: Failed to get the composition instuction parameters from: \"" + instructionFullLine + "\"");
                    success = false; break;
                }
                
                success = AddCompositionsToMixer(effectName, mapShaderNameWithBytecode, mixerMap,
                    listAllocatedBytecodes, listUserDefinedMacros, parser, useXkslangDll, operationNum,
                    mixerTarget, compositionsInstructionsStr);

                if (!success) { error("Failed to add the compositions instruction to the mixer: " + compositionsInstructionsStr); success = false; break; }
            }
            else if (instruction.compare("setStageEntryPoint") == 0)
            {
                //string& currentInstruction = listParsedInstructions[listParsedInstructions.size() - 1];
                //Utils::replaceAll(currentInstruction, "mixin ", "mixin( ");
                //currentInstruction += " )";
                //Utils::replaceAll(currentInstruction, compositionTargetStr, compositionTargetStr + " =");

                string lineRemainingStr;
                if (!getline(lineSs, lineRemainingStr)) {
                    error("setStageEntryPoint: Failed to read the line");
                    success = false; break;
                }

                string parametersStr;
                if (!getFunctionParameterString(lineRemainingStr, parametersStr, false)) {
                    error("Failed to get the stage entry points parameters from: \"" + instructionFullLine + "\"");
                    success = false; break;
                }

                vector<string> entryPoints;
                if (!splitPametersString(parametersStr, entryPoints))
                    return error("failed to split the entryPoints parameters");

                for (unsigned int e = 0; e < entryPoints.size(); e++)
                {
                    const string& entryPointInstruction = entryPoints[e];

                    string stageStr;
                    string entryPointStr;
                    if (!getNextWord(entryPointInstruction, "=", stageStr, entryPointStr))
                        return error("\"=\" expected");
                    stageStr = Utils::trim(stageStr);
                    entryPointStr = Utils::trim(entryPointStr);
                    entryPointStr = Utils::trim(entryPointStr, '"');

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
                    //Optionnal: get and display all compositions
                    {
                        vector<ShaderCompositionInfo> vecCompositions;
                        success = mixerTarget->mixer->GetListAllCompositions(vecCompositions, errorMsgs);
                        if (!success) { error("Failed to get the list of all compositions from the mixer"); break; }
                        if (vecCompositions.size() > 0)
                        {
                            std::cout << endl;
                            std::cout << "Count Compositions: " << vecCompositions.size() << endl;
                            for (unsigned int c = 0; c < vecCompositions.size(); c++)
                            {
                                ShaderCompositionInfo& composition = vecCompositions[c];
                                std::cout << " " << composition.CompositionShaderType << " " << composition.ShaderOwner << "."
                                    << composition.CompositionVariableName << (composition.IsArray ? "[]" : "")  << " (instances=" << composition.CompositionCountInstances << ")" << endl;
                            }
                            std::cout << endl;
                        }
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
        xkslangDll::ReleaseMixer();
    }

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
    const string inputFname = inputDir + "\\" + effect.inputFileName;
    string effectCmdLines;
    if (!Utils::ReadFile(inputFname, effectCmdLines))
    {
        error(" Failed to read the file: " + inputFname);
        return false;
    }

    //Utils::replaceAll(effectCmdLines, "addComposition ", "addComposition = ");
    //Utils::WriteFile(inputFname, effectCmdLines);

    string updatedEffectCommandLines;
    if (processEffectWithDirectCallToXkslang)
    {
        std::cout << "=================================" << endl;
        std::cout << "Process XKSL File (direct call to Xkslang classes)" << endl;

        success1 = ProcessEffectCommandLine(parser, effectName, effectCmdLines, false, updatedEffectCommandLines);
        if (success1) std::cout << "Effect successfully processed." << endl;
        else error("Failed to process the effect");
    }

    if (processEffectWithDllApi)
    {
        std::cout << endl;
        std::cout << "=================================" << endl;
        std::cout << "Process XKSL File through Xkslang Dll API" << endl;

        success2 = ProcessEffectCommandLine(parser, effectName, effectCmdLines, true, updatedEffectCommandLines);
        if (success2) std::cout << "Effect successfully processed." << endl;
        else error("Failed to process the effect");
    }

    bool success = success1 && success2;

    //if (success)
    //{
    //    Utils::WriteFile(inputFname, updatedEffectCommandLines);
    //}

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
            success = ParseAndConvertXkslFile(&parser, xkslShaderInputFile, listGenericsValue, listUserDefinedMacros, spirXBytecode, true);

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

    if (processEffectWithDllApi) {
        xkslangDll::InitializeParser();
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

            SpxMixer::StartMixinEffect();
            bool success = ProcessEffect(&parser, effect);
            SpxMixer::StopMixinEffect();

            if (success) countEffectsSuccessful++;
            else listEffectsFailed.push_back(effect.effectName);

            std::cout << endl;
        }
    }

    if (processEffectWithDllApi) {
        xkslangDll::ReleaseParser();
    }

    if (vecSpvFileToConvertToGlslAndHlsl.size() > 0)
    {
        std::cout << endl;
        std::cout << "___________________________________________________________________________________" << endl;
        std::cout << "Convert SPV Files:" << endl << endl;

        for (unsigned int n = 0; n < vecSpvFileToConvertToGlslAndHlsl.size(); ++n)
        {
            bool success = true;
            XkfxEffectsToProcess effect = vecSpvFileToConvertToGlslAndHlsl[n];
            string inputFileSpv = inputDir + effect.inputFileName;

            string xkslInput;
            if (!Utils::ReadFile(inputFileSpv, xkslInput))
            {
                error(" Failed to read the file: " + inputFileSpv);
                success = false;
            }

            if (success)
            {
                //======================================================================
                //GLSL
                string outputNameGlsl = effect.inputFileName + ".glsl";
                string outputFullNameGlsl = outputDir + outputNameGlsl;

                std::cout << "Convert into GLSL: " << effect.inputFileName << endl;
                //result = ConvertSpvToShaderLanguage(inputFileSpv, outputFullNameGlsl, ShaderLanguageEnum::GlslLanguage);
                success = ConvertBytecodeToGlsl(inputFileSpv, outputFullNameGlsl);

                if (success) std::cout << " OK." << endl;
                else error(" Failed to convert the SPIRV file to GLSL");

                //======================================================================
                //HLSL
                string outputNameHlsl = effect.inputFileName + ".hlsl";
                string outputFullNameHlsl = outputDir + outputNameHlsl;

                std::cout << "Convert into HLSL: " << effect.inputFileName << endl;
                //result = ConvertSpvToShaderLanguage(inputFileSpv, outputFullNameHlsl, ShaderLanguageEnum::HlslLanguage);
                success = ConvertBytecodeToHlsl(inputFileSpv, outputFullNameHlsl);
            }

            if (success) std::cout << " OK." << endl;
            else error(" Failed to convert the SPIRV file to HLSL");
        }
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

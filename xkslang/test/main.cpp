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
    { "CBuffer11", "CBuffer11.xkfx" },
    
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
    //{ "XenkoForwardShadingEffect", "XenkoForwardShadingEffect.xkfx" },
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

static bool ConvertBytecodeToHlsl(const string& spvFile, const  string& outputFile)
{
    pair<bool, vector<uint32_t>> result = Utils::ReadSpvBinaryFile(spvFile);
    if (!result.first){
        std::cout << " Failed to open the SPV file: \"" << spvFile << "\"" << endl;
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
        std::cout << " Failed to open the SPV file: \"" << spvFile << "\"" << endl;
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
            if (pError != nullptr) std::cout << pError << endl;
            GlobalFree(pError);

            std::cout << "failed to get the mixer current bytecode" << endl;
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
            std::cout << "failed to get the mixer current bytecode" << endl;
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
                else std::cout << " Failed to convert the SPIRV file to GLSL" << endl;

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
                        std::cout << " Failed to read the file: " << fileNameGlsl << endl;
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
                else std::cout << " Failed to convert the SPIRV file to HLSL" << endl;

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
                        std::cout << " Failed to read the file: " << fileNameHlsl << endl;
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
        if (pError != nullptr) std::cout << pError << endl;
        GlobalFree(pError);

        std::cout << "Compilation Failed" << endl;
        return false;
    }

    //get and save the final mixin compiled bytecode
    SpvBytecode compiledBytecode;
    {
        int bytecodeLength = 0;
        uint32_t* pBytecodeBuffer = xkslangDll::GetMixerCompiledBytecode(mixer->mixerHandleId, &bytecodeLength);
        if (pBytecodeBuffer == nullptr || bytecodeLength <= 0) {
            cout << "Failed to get the mixin compiled bytecode" << endl;
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
            if (pError != nullptr) std::cout << pError << endl;
            GlobalFree(pError);

            cout << "Failed to get the Effect Reflection data" << endl;
            return false;
        }*/

        //TMP: directly call xkslang classes
        success = SpxMixer::GetCompiledBytecodeReflection(compiledBytecode, effectReflection, errorMsgs);
        if (!success)
        {
            std::cout << "Failed to get the reflection data from the compiled bytecode" << endl;
            return false;
        }
    }

    //get the output stages' compiled bytecode
    for (unsigned int i = 0; i < outputStages.size(); ++i)
    {
        int bytecodeLength = 0;
        uint32_t* pBytecodeBuffer = xkslangDll::GetMixerCompiledBytecodeForStage(mixer->mixerHandleId, outputStages[i].stage, &bytecodeLength);
        if (pBytecodeBuffer == nullptr || bytecodeLength <= 0) {
            cout << "Failed to get the mixin compiled bytecode" << endl;
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
        std::cout << "Compilation Failed" << endl;
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
            std::cout << "Failed to get the reflection data from the compiled bytecode" << endl;
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
        std::cout << " Failed to read the file: " << inputFname << endl;
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
        std::cout << " Failed to parse the XKSL file" << endl;
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
    
    std::cout << "Cannot find data for shader: " << shaderName << endl;
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

static bool GetShadingStageForString(string& str, ShadingStageEnum& stage)
{
    if (str.compare("Vertex") == 0) {stage = ShadingStageEnum::Vertex; return true;}
    if (str.compare("Pixel") == 0) {stage = ShadingStageEnum::Pixel; return true;}
    if (str.compare("TessControl") == 0) {stage = ShadingStageEnum::TessControl; return true;}
    if (str.compare("TessEvaluation") == 0) {stage = ShadingStageEnum::TessEvaluation; return true;}
    if (str.compare("Geometry") == 0) {stage = ShadingStageEnum::Geometry; return true;}
    if (str.compare("Compute") == 0) {stage = ShadingStageEnum::Compute; return true;}
    return false;
}

static bool getNextWord(stringstream& stream, string& word)
{
    while (stream.peek() == ' ') stream.get(); // skip front spaces

    if (!getline(stream, word, ' ')) {
        return false;
    }

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
                    std::cout << "2 or more shaders match the unmangled shader name: " << anUnmangledShaderName << endl;
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
    //    std::cout << "Shader is already recorded in the map: " << fullShaderName;
    //    return false;
    //}

    mapShaderNameWithBytecode[shaderFullName] = spxBytecode;
    return true;
}

static bool ProcessEffectCommandLine(XkslParser* parser, string effectName, string effectCmdLines, bool useXkslangDll)
{
    bool success = true;

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

    vector<XkslUserDefinedMacro> listUserDefinedMacros;

    string line, lineItem;
    stringstream ss(effectCmdLines);
    while (getline(ss, line, '\n'))
    {
        line = Utils::trim(line);
        if (line.size() == 0) continue;

        stringstream lineSs(line);
        getNextWord(lineSs, lineItem);

        if (lineItem.size() >= 2 && lineItem[0] == '/' && lineItem[1] == '/')
        {
            //a comment: ignore the line
        }
        else if (lineItem.compare("addResourcesLibrary") == 0)
        {
            string folder;
            if (!getNextWord(lineSs, folder)) {
                std::cout << "addResourcesLibrary: failed to get the library resource folder" << endl;
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
                std::cout << "Fails to get the macros definition" << endl;
                success = false; break;
            }

            if (XkslParser::ParseStringMacroDefinition(strMacrosDefinition.c_str(), listUserDefinedMacros, false) != 1)
            {
                std::cout << "Fails to parse the macros definition: " << strMacrosDefinition << endl;
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
                std::cout << "convertAndLoadRecursif: failed to get the XKSL file parameters" << endl;
                success = false; break;
            }
            stringShaderAndgenericsValue = Utils::trim(stringShaderAndgenericsValue, ' ');
            if (stringShaderAndgenericsValue.size() == 0) {
                std::cout << "convertAndLoadRecursif: failed to get the XKSL shader parameters" << endl;
                success = false; break;
            }

            //any search prefix?
            string xkslInputFilePrefix = "";
            if (stringShaderAndgenericsValue[0] == '"')
            {
                unsigned int indexEnd = 1;
                while (indexEnd < stringShaderAndgenericsValue.size() && stringShaderAndgenericsValue[indexEnd] != '"') indexEnd++;
                if (indexEnd == stringShaderAndgenericsValue.size()) {
                    std::cout << "convertAndLoadRecursif: failed to get the prefix parameter" << endl;
                    success = false; break;
                }

                xkslInputFilePrefix = stringShaderAndgenericsValue.substr(0, indexEnd + 1);
                xkslInputFilePrefix = Utils::trim(xkslInputFilePrefix, '\"');
                stringShaderAndgenericsValue = stringShaderAndgenericsValue.substr(indexEnd + 1);
            }

            vector<ShaderGenericValues> listShaderAndGenerics;
            if (!XkslParser::ParseStringShaderAndGenerics(stringShaderAndgenericsValue.c_str(), listShaderAndGenerics)) {
                std::cout << "convertAndLoadRecursif: failed to read the shader and its generics value from: " << stringShaderAndgenericsValue << endl;
                success = false; break;
            }

            if (listShaderAndGenerics.size() != 1) {
                std::cout << "convertAndLoadRecursif: 1 shader name expected" << endl;
                success = false; break;
            }
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
                    if (pError != nullptr) std::cout << pError << endl;
                    GlobalFree(pError);

                    std::cout << "convertAndLoadRecursif: failed to convert the XKSL file name: " << xkslInputFilePrefix << endl;
                    success = false; break;
                }
                else std::cout << " OK. time: " << (time_after - time_before) << "ms" << endl;

                spxBytecode = new SpxBytecode;
                listAllocatedBytecodes.push_back(spxBytecode);
                vector<uint32_t>& vecBytecode  = spxBytecode->getWritableBytecodeStream();
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
                success = RecursivelyParseAndConvertXkslShader(parser, effectName, shaderName, xkslInputFilePrefix, listShaderAndGenerics, listUserDefinedMacros, *spxBytecode, infoMsg);
                time_after = GetTickCount();

                if (infoMsg.size() > 0) std::cout << infoMsg;

                if (success) std::cout << " OK. time: " << (time_after - time_before) << "ms" << endl;
                else
                {
                    std::cout << "convertAndLoadRecursif: failed to parse and convert the XKSL file name: " << xkslInputFilePrefix << endl;
                    success = false; break;
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
                success = xkslangDll::GetBytecodeShadersInformation(pBytecodeBuffer, bytecodeLength, &shadersInfo, &countShaders);
                if (!success)
                {
                    char* pError = xkslangDll::GetErrorMessages();
                    if (pError != nullptr) std::cout << pError << endl;
                    GlobalFree(pError);
                
                    std::cout << "convertAndLoadRecursif: failed to get the shader info from the bytecode" << endl;
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
                    std::cout << "convertAndLoadRecursif: failed to get the list of shader names from: " << xkslInputFilePrefix << endl;
                    success = false; break;
                }
            }

            //Store the new shaders bytecode in our map
            for (unsigned int is = 0; is < vecShadersParsed.size(); ++is)
            {
                string shaderName = vecShadersParsed[is];
                if (!RecordSPXShaderBytecode(shaderName, spxBytecode, mapShaderNameWithBytecode))
                {
                    std::cout << "convertAndLoadRecursif: Can't add the shader into the bytecode: " << shaderName;
                    success = false; break;
                }
            }
        }
        else if (lineItem.compare("convertAndLoad") == 0)
        {
            /// convert and load xksl shaders

            //================================================
            //Parse the command line parameters
            string xkslInputFile;
            if (!getNextWord(lineSs, xkslInputFile)) {
                std::cout << "convertAndLoad: failed to get the XKSL file name" << endl;
                success = false; break;
            }
            xkslInputFile = Utils::trim(xkslInputFile, '\"');

            //any generic value defined?
            vector<ShaderGenericValues> listShaderAndGenerics;
            string stringShaderAndgenericsValue;
            if (getline(lineSs, stringShaderAndgenericsValue))
            {
                if (!XkslParser::ParseStringShaderAndGenerics(stringShaderAndgenericsValue.c_str(), listShaderAndGenerics)) {
                    std::cout << "convertAndLoad: failed to read the shaders and their generics value from: " << stringShaderAndgenericsValue << endl;
                    success = false; break;
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
                    std::cout << " Failed to read the file: " << inputFname << endl;
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
                    if (pError != nullptr) std::cout << pError << endl;
                    GlobalFree(pError);

                    std::cout << "convertAndLoadRecursif: failed to convert the xksl file name: " << xkslInputFile << endl;
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
                    std::cout << "convertAndLoad: failed to convert the xksl file name: " << xkslInputFile << endl;
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
                    if (pError != nullptr) std::cout << pError << endl;
                    GlobalFree(pError);

                    std::cout << "convertAndLoadRecursif: failed to get the shader info from the bytecode" << endl;
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
                    std::cout << "convertAndLoad: failed to get the list of shader names from: " << xkslInputFile << endl;
                    success = false; break;
                }
            }

            for (unsigned int is = 0; is < vecShadersParsed.size(); ++is)
            {
                string shaderName = vecShadersParsed[is];
                if (!RecordSPXShaderBytecode(shaderName, spxBytecode, mapShaderNameWithBytecode))
                {
                    std::cout << "Can't add the shader into the bytecode: " << shaderName;
                    success = false; break;
                }
            }
        }
        else if (lineItem.compare("mixer") == 0)
        {
            string mixerName;
            if (!getNextWord(lineSs, mixerName)) {
                std::cout << "mixer: failed to get the xksl file name" << endl;
                success = false; break;
            }
            mixerName = Utils::trim(mixerName, '\"');

            if (mixerMap.find(mixerName) != mixerMap.end()) {
                std::cout << "mixer: a mixer already exists with the name:" << mixerName << endl;
                success = false; break;
            }

            if (useXkslangDll)
            {
                uint32_t mixerHandleId = xkslangDll::CreateSpxShaderMixer();
                if (mixerHandleId == 0) {
                    cout << "Failed to create a new spx mixer" << endl;
                    success = false; break;
                }
                mixerMap[mixerName] = new EffectMixerObject(mixerHandleId);
            }
            else
            {
                SpxMixer* mixer = new SpxMixer();
                mixerMap[mixerName] = new EffectMixerObject(mixer);
            }
        }
        else
        {
            //mixer operation (mixer.instructions)
            string mixerName, instruction;
            if (!SeparateAdotB(lineItem, mixerName, instruction)) {
                std::cout << "Unknown instruction: " << lineItem << endl;
                success = false; break;
            }

            if (mixerMap.find(mixerName) == mixerMap.end()) {
                std::cout << lineItem << ": no mixer found with the name:" << mixerName << endl;
                success = false; break;
            }
            EffectMixerObject* mixerTarget = mixerMap[mixerName];

            if (instruction.compare("mixin") == 0)
            {
                //Find the bytecode for the shader we want to mix
                SpxBytecode* spxBytecode = nullptr;
                vector<string> listShaderToMix;
                string shaderName;
                string shaderFullName;
                while (getNextWord(lineSs, shaderName))
                {
                    SpxBytecode* aShaderBytecode = GetSpxBytecodeForShader(shaderName, shaderFullName, mapShaderNameWithBytecode, true);
                    if (aShaderBytecode == nullptr)
                    {
                        std::cout << "cannot find a bytecode in the shader librady for the shader: " << shaderName << endl;
                        success = false; break;
                    }
                    if (spxBytecode == nullptr) spxBytecode = aShaderBytecode;
                    else
                    {
                        if (spxBytecode != aShaderBytecode) {
                            std::cout << "2 shaders to mix are defined in different bytecode (maybe we could merge them)" << endl;
                            success = false; break;
                        }
                    }

                    listShaderToMix.push_back(shaderFullName);
                }
                if (spxBytecode == nullptr)
                {
                    std::cout << "No spxBytecode found for the mixin instruction" << endl;
                    success = false; break;
                }

                //Mixin the bytecode into the mixer
                std::cout << "mixin: " << line << endl;
                if (useXkslangDll)
                {
                    if (listShaderToMix.size() != 1) { cout << "not implemented yet" << endl; break; }
                    uint32_t* pBytecodeBuffer = &(spxBytecode->getWritableBytecodeStream().front());
                    int32_t bytecodeLength = spxBytecode->GetBytecodeSize();
                    time_before = GetTickCount();
                    success = xkslangDll::MixinShader(mixerTarget->mixerHandleId, listShaderToMix[0].c_str(), pBytecodeBuffer, bytecodeLength);
                    time_after = GetTickCount();
                }
                else
                {
                    
                    time_before = GetTickCount();
                    success = mixerTarget->mixer->Mixin(*spxBytecode, listShaderToMix, errorMsgs);
                    time_after = GetTickCount();
                }

                if (success) std::cout << " OK. Time:  " << (time_after - time_before) << "ms" << endl;
                else { std::cout << "Mixin failed" << endl; break; }

                //write the mixer current bytecode
                string outputFileName = effectName + "_op" + to_string(operationNum++) + "_mixin" + ".hr.spv";
                success = GetAndWriteMixerCurrentBytecode(mixerTarget, outputFileName, useXkslangDll);
                if (!success) {
                    cout << "Failed to get and write the mixer current bytecode" << endl;
                    break;
                }
            }
            else if (instruction.compare("addComposition") == 0)
            {
                //=====================================
                // read the command line parameters
                string compositionTargetStr;
                if (!getNextWord(lineSs, compositionTargetStr)) {
                    std::cout << "Expecting composition target" << endl;
                    success = false; break;
                }

                //We can either have shader.variableName, or only a variableName
                string shaderName, variableName;
                if (!SeparateAdotB(compositionTargetStr, shaderName, variableName)) {
                    variableName = compositionTargetStr;
                    shaderName = "";
                }

                string mixerSourceName;
                if (!getNextWord(lineSs, mixerSourceName)) {
                    std::cout << "Expecting mixer composition source" << endl;
                    success = false; break;
                }

                if (mixerMap.find(mixerSourceName) == mixerMap.end()) {
                    std::cout << lineItem << ": no mixer found with the name:" << mixerSourceName << endl;
                    success = false; break;
                }
                EffectMixerObject* mixerSource = mixerMap[mixerSourceName];

                //=====================================
                // Add the composition to the mixer
                std::cout << "Adding composition: " << line << endl;

                if (useXkslangDll)
                {
                    time_before = GetTickCount();
                    success = xkslangDll::AddComposition(mixerTarget->mixerHandleId, shaderName.c_str(), variableName.c_str(), mixerSource->mixerHandleId);
                    time_after = GetTickCount();
                }
                else
                {
                    time_before = GetTickCount();
                    success = mixerTarget->mixer->AddComposition(shaderName, variableName, mixerSource->mixer, errorMsgs);
                    time_after = GetTickCount();
                }

                if (success) std::cout << " OK. Time:  " << (time_after - time_before) << "ms" << endl;
                else { std::cout << "Failed to add the composition to the mixer" << endl; break; }

                //write the mixer current bytecode
                string outputFileName = effectName + "_op" + to_string(operationNum++) + "_mixin" + ".hr.spv";
                success = GetAndWriteMixerCurrentBytecode(mixerTarget, outputFileName, useXkslangDll);
                if (!success) {
                    cout << "Failed to get and write the mixer current bytecode" << endl;
                    break;
                }
            }
            else if (instruction.compare("setStageEntryPoint") == 0)
            {
                string stageStr;
                if (!getNextWord(lineSs, stageStr)) {
                    std::cout << "Expecting stage" << endl;
                    success = false; break;
                }
                ShadingStageEnum stage;
                if (!GetShadingStageForString(stageStr, stage)) {
                    std::cout << "Unknown stage:" << stageStr;
                    success = false; break;
                }

                string entryPoint;
                if (!getNextWord(lineSs, entryPoint)) {
                    mixerTarget->stagesEntryPoints[(int)stage] = "";
                }
                else
                {
                    entryPoint = Utils::trim(entryPoint, '\"');
                    mixerTarget->stagesEntryPoints[(int)stage] = entryPoint;
                }
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
                    if (!success) std::cout << "Failed to compile the effect: " << effectName << endl;
                }
                else
                {
                    //Optionnal: get and display all compositions
                    {
                        vector<ShaderCompositionInfo> vecCompositions;
                        success = mixerTarget->mixer->GetListAllCompositions(vecCompositions, errorMsgs);
                        if (!success) { std::cout << "Failed to get the list of all compositions from the mixer" << endl; break; }
                        if (vecCompositions.size() > 0)
                        {
                            cout << endl;
                            cout << "Count Compositions: " << vecCompositions.size() << endl;
                            for (unsigned int c = 0; c < vecCompositions.size(); c++)
                            {
                                ShaderCompositionInfo& composition = vecCompositions[c];
                                cout << " " << composition.CompositionShaderType << " " << composition.ShaderOwner << "."
                                    << composition.CompositionVariableName << (composition.IsArray ? "[]" : "")  << " (instances=" << composition.CompositionCountInstances << ")" << endl;
                            }
                            cout << endl;
                        }
                    }

                    success = CompileMixer(effectName, mixerTarget->mixer, outputStages, errorMsgs);
                    if (!success) std::cout << "Failed to compile the effect: " << effectName << endl;
                }
            }
            else {
                std::cout << "Unknown instruction: " << instruction << endl;
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
        std::cout << " Failed to read the file: " << inputFname << " !!!" << endl;
        return false;
    }

    //Utils::replaceAll(effectCmdLines, "load ", "convertAndLoad ");
    //Utils::WriteFile(inputFname, effectCmdLines);

    if (processEffectWithDirectCallToXkslang)
    {
        std::cout << "=================================" << endl;
        std::cout << "Process XKSL File (direct call to Xkslang classes)" << endl;

        success1 = ProcessEffectCommandLine(parser, effectName, effectCmdLines, false);
        if (success1) std::cout << "Effect successfully processed." << endl;
        else std::cout << "Failed to process the effect" << endl;
    }

    if (processEffectWithDllApi)
    {
        std::cout << endl;
        std::cout << "=================================" << endl;
        std::cout << "Process XKSL File through Xkslang Dll API" << endl;

        success2 = ProcessEffectCommandLine(parser, effectName, effectCmdLines, true);
        if (success2) std::cout << "Effect successfully processed." << endl;
        else { std::cout << "Failed to process the effect" << endl; return false; }
    }

    return success1 && success2;
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
        std::cout << "Failed to setup the directories" << endl;
        return;
    }

    //Init parser
    XkslParser parser;
    if (!parser.InitialiseXkslang())
    {
        std::cout << "Failed to initialize the XkslParser" << endl;
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
                std::cout << " Failed to read the file: " << inputFileSpv << endl;
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
                else std::cout << " Failed to convert the SPIRV file to GLSL" << endl;

                //======================================================================
                //HLSL
                string outputNameHlsl = effect.inputFileName + ".hlsl";
                string outputFullNameHlsl = outputDir + outputNameHlsl;

                std::cout << "Convert into HLSL: " << effect.inputFileName << endl;
                //result = ConvertSpvToShaderLanguage(inputFileSpv, outputFullNameHlsl, ShaderLanguageEnum::HlslLanguage);
                success = ConvertBytecodeToHlsl(inputFileSpv, outputFullNameHlsl);
            }

            if (success) std::cout << " OK." << endl;
            else std::cout << " Failed to convert the SPIRV file to HLSL" << endl;
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

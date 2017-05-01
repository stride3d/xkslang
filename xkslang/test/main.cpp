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

#include "../source/SPIRV-Cross/spirv_cross.hpp"
//#include "../source/XkslangDLL/XkslangDLL.h"

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

    //{ "ShaderWithResources01", "ShaderWithResources01.xkfx" },
    //{ "ShaderWithResources02", "ShaderWithResources02.xkfx" },
    //{ "ShaderWithResources03", "ShaderWithResources03.xkfx" },
    //{ "ShaderWithResources04", "ShaderWithResources04.xkfx" },
    //{ "ShaderWithResources05", "ShaderWithResources05.xkfx" },
    //{ "ShaderWithResources06", "ShaderWithResources06.xkfx" },
    { "ShaderWithResources07", "ShaderWithResources07.xkfx" },
    //{ "ShaderWithResources08", "ShaderWithResources08.xkfx" },

    //{ "testDependency01", "testDependency01.xkfx" },
    //{ "testDependency02", "testDependency02.xkfx" },
    //{ "testDependency03", "testDependency03.xkfx" },
    //{ "testDependency04", "testDependency04.xkfx" },
    //{ "testDependency05", "testDependency05.xkfx" },
    //{ "testDependency06", "testDependency06.xkfx" },
    //{ "testDependency07", "testDependency07.xkfx" },

    //{ "SemanticTest01", "SemanticTest01.xkfx" },
    //{ "SemanticTest02", "SemanticTest02.xkfx" },

    //{ "Effect01", "Effect01.xkfx" },
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

static int ConvertSpvToShaderLanguage(string spvFile, string outputFile, ShaderLanguageEnum language)
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

    return true;
}

enum class BytecodeFileFormat
{
    Binary,
    Text,
};

static void WriteBytecode(const SpvBytecode& bytecode, const string& outputDir, const string& outputFileName, BytecodeFileFormat format)
{
    const vector<uint32_t>& bytecodeList = bytecode.getBytecodeStream();
    const string outputFullName = outputDir + outputFileName;

    if (bytecodeList.size() > 0)
    {
        if (format == BytecodeFileFormat::Binary)
        {
            //spv
            glslang::OutputSpvBin(bytecodeList, outputFullName.c_str());
        }
        else if (format == BytecodeFileFormat::Text)
        {
            //hr spv
            ostringstream disassembly_stream;
            spv::Parameterize();
            spv::Disassemble(disassembly_stream, bytecodeList);

            xkslangtest::Utils::WriteFile(outputFullName, disassembly_stream.str());
        }
        cout << " output: \"" << outputFileName << "\"" << endl;
    }
}

static void SaveCurrentMixerBytecode(SpxMixer* mixer, string outputDirPath, string outputFileName)
{
    vector<string> errorMsgs;
    SpxBytecode mixinBytecode;
    bool canGetBytecode = mixer->GetCurrentMixinBytecode(mixinBytecode, errorMsgs);
    if (!canGetBytecode) {
        cout << " Failed to get the mixin bytecode" << endl;
    }
    else
    {
        const vector<uint32_t>& bytecodeList = mixinBytecode.getBytecodeStream();
        ostringstream disassembly_stream;
        spv::Parameterize();
        spv::Disassemble(disassembly_stream, bytecodeList);

        const string outputFullName = outputDirPath + outputFileName;
        xkslangtest::Utils::WriteFile(outputFullName, disassembly_stream.str());
        cout << " output: \"" << outputFileName << "\"" << endl;
    }
}

static bool CompileMixer(string effectName, SpxMixer* mixer, vector<OutputStageBytecode>& outputStages, vector<string>& errorMsgs)
{
    DWORD time_before, time_after;
    bool success = true;

    SpvBytecode composedSpv;
    SpvBytecode streamsMergedSpv;
    SpvBytecode streamsReshuffledSpv;
    SpvBytecode mergedCBuffersSpv;
    SpvBytecode finalSpv;
    SpvBytecode errorSpv;
    cout << "Compile Mixin: ";
    time_before = GetTickCount();
    success = mixer->Compile(outputStages, errorMsgs, &composedSpv, &streamsMergedSpv, &streamsReshuffledSpv, &mergedCBuffersSpv, &finalSpv, &errorSpv);
    time_after = GetTickCount();

    if (!success)
    {
        cout << "Compilation Failed" << endl;
        WriteBytecode(errorSpv, outputDir, effectName + "_compile_error.hr.spv", BytecodeFileFormat::Text);
    }
    else
    {
        cout << "OK. time: " << (time_after - time_before) << "ms" << endl;
    }

    //output the compiled intermediary bytecodes
    WriteBytecode(composedSpv, outputDir, effectName + "_compile0_composed.hr.spv", BytecodeFileFormat::Text);
    WriteBytecode(streamsMergedSpv, outputDir, effectName + "_compile1_streamsMerged.hr.spv", BytecodeFileFormat::Text);
    WriteBytecode(streamsReshuffledSpv, outputDir, effectName + "_compile2_streamsReshuffled.hr.spv", BytecodeFileFormat::Text);
    WriteBytecode(mergedCBuffersSpv, outputDir, effectName + "_compile3_mergedCBuffers.hr.spv", BytecodeFileFormat::Text);

    {
        //write the final SPIRV bytecode then convert it into GLSL
        string outputFileNameFinalSpv = effectName + "_compile4_final.spv";
        string outputFileNameFinalSpvHr = effectName + "_compile4_final.hr.spv";
        string outputFullnameFinalSpv = outputDir + outputFileNameFinalSpv;
        WriteBytecode(finalSpv, outputDir, outputFileNameFinalSpv, BytecodeFileFormat::Binary);
        WriteBytecode(finalSpv, outputDir, outputFileNameFinalSpvHr, BytecodeFileFormat::Text);

        //if (success)
        //{
        //    string fileNameGlsl = effectName + "_compileFinal.glsl";
        //    string fullNameGlsl = outputDir + fileNameGlsl;
        //
        //    cout << "Final SPV bytecode: Convert into GLSL." << endl;
        //
        //    time_before = GetTickCount();
        //    int result = ConvertSpvToGlsl(outputFullnameFinalSpv, fullNameGlsl);
        //    time_after = GetTickCount();
        //    if (result != 0) success = false;
        //}
    }

    if (!success) return false;

    //convert and output every stages
    string glslAllOutputs;
    string hlslAllOutputs;
    bool someExpectedOutputsDifferent = false;
    bool someExpectedOutputsAreMissing = false;
    for (unsigned int i = 0; i<outputStages.size(); ++i)
    {
        string labelStage = GetShadingStageLabel(outputStages[i].stage);
        cout << "Convert SPIRV bytecode for entry point=\"" << outputStages[i].entryPointName << "\" stage=\"" << labelStage << "\"" << endl;

        ///Save the SPIRV bytecode (and its HR form)
        string outputFileNameSpv = effectName + "_" + labelStage + ".spv";
        string outputFullnameSpv = outputDir + outputFileNameSpv;
        WriteBytecode(outputStages[i].resultingBytecode, outputDir, outputFileNameSpv, BytecodeFileFormat::Binary);
        WriteBytecode(outputStages[i].resultingBytecode, outputDir, effectName + "_" + labelStage + ".hr.spv", BytecodeFileFormat::Text);

        //======================================================================================================
        //convert back the SPIRV bytecode into GLSL / HLSL
        {
            {
                glslAllOutputs += "\n";
                glslAllOutputs += "\\\\=============================\n";
                glslAllOutputs += "\\\\" + labelStage + " Stage\n";
                glslAllOutputs += "\\\\=============================\n";

                string fileNameGlsl = effectName + "_" + labelStage + ".glsl";
                string fullNameGlsl = outputDir + fileNameGlsl;
                cout << labelStage << " stage: Convert into GLSL." << endl;
                time_before = GetTickCount();
                int result = ConvertSpvToShaderLanguage(outputFullnameSpv, fullNameGlsl, ShaderLanguageEnum::GlslLanguage);
                time_after = GetTickCount();
                if (result != 0) success = false;

                if (success) cout << " OK. time: " << (time_after - time_before) << "ms" << endl;
                else cout << " Failed to convert the SPIRV file to GLSL" << endl;

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
                                cout << "expected output:" << endl << glslExpectedOutput;
                                cout << "output:" << endl << glslConvertedOutput;
                                cout << " Glsl output and expected output are different !!!" << endl;
                                someExpectedOutputsDifferent = true;
                            }
                            else {
                                cout << " GLSL output VS expected output: OK" << endl;
                            }
                        }
                        else {
                            cout << " !!! Warning: No expected output file for: " << fileNameGlsl << endl;
                            someExpectedOutputsAreMissing = true;
                        }
                    }
                    else {
                        cout << " Failed to read the file: " << fileNameGlsl << endl;
                        success = false;
                    }
                }
            }

            {
                hlslAllOutputs += "\n";
                hlslAllOutputs += "\\\\=============================\n";
                hlslAllOutputs += "\\\\" + labelStage + " Stage\n";
                hlslAllOutputs += "\\\\=============================\n";

                string fileNameHlsl = effectName + "_" + labelStage + ".hlsl";
                string fullNameHlsl = outputDir + fileNameHlsl;
                cout << labelStage << " stage: Convert into HLSL." << endl;
                time_before = GetTickCount();
                int result = ConvertSpvToShaderLanguage(outputFullnameSpv, fullNameHlsl, ShaderLanguageEnum::HlslLanguage);
                time_after = GetTickCount();
                if (result != 0) success = false;

                if (success) cout << " OK. time: " << (time_after - time_before) << "ms" << endl;
                else cout << " Failed to convert the SPIRV file to HLSL" << endl;

                //======================================================================================================
                //compare the glsl output with the expected output
                if (success)
                {
                    string hlslConvertedOutput;
                    if (Utils::ReadFile(fullNameHlsl, hlslConvertedOutput))
                    {
                        hlslAllOutputs += hlslConvertedOutput;
                    }
                    else {
                        cout << " Failed to read the file: " << fileNameHlsl << endl;
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
        string fileNameAllGlsl = effectName + ".glsl";
        string fullNameAllGlsl = finalResultOutputDir + fileNameAllGlsl;
        xkslangtest::Utils::WriteFile(fullNameAllGlsl, glslAllOutputs);
        cout << " output: \"" << fileNameAllGlsl << "\"" << endl;
    }
    if (hlslAllOutputs.size() > 0)
    {
        string fileNameAllHlsl = effectName + ".hlsl";
        string fullNameAllHlsl = finalResultOutputDir + fileNameAllHlsl;
        xkslangtest::Utils::WriteFile(fullNameAllHlsl, hlslAllOutputs);
        cout << " output: \"" << fileNameAllHlsl << "\"" << endl;
    }

    return success;
}

static bool ParseAndConvertXkslFile(XkslParser* parser, string& xkslInputFile, const vector<ShaderGenericValues>& listGenericsValue, SpxBytecode& spirXBytecode, bool writeOutputsOnDisk)
{
    cout << "Parsing XKSL file \"" << xkslInputFile << "\"" << endl;

    const string inputFname = inputDir + xkslInputFile;
    string xkslInput;
    if (!Utils::ReadFile(inputFname, xkslInput))
    {
        cout << " Failed to read the file: " << inputFname << endl;
        return false;
    }

    //======================================================================================================
    //======================================================================================================
    // Parse and convert XKSL shaders to SPIRX bytecode
    ostringstream errorAndDebugMessages;

    DWORD time_before, time_after;
    time_before = GetTickCount();
    bool success = parser->ConvertXkslFileToSpx(xkslInputFile, xkslInput, listGenericsValue, spirXBytecode, &errorAndDebugMessages);
    time_after = GetTickCount();

    string infoMsg = errorAndDebugMessages.str();
    if (infoMsg.size() > 0) cout << infoMsg;

    if (!success) {
        cout << " Failed to parse the xksl file" << endl;
        return false;
    }
    else
    {
        cout << " OK. time: " << (time_after - time_before) << "ms" << endl;

        //output binary and debug info
        if (writeOutputsOnDisk)
        {
            //write the SPIRX binary
            const vector<uint32_t>& bytecodeList = spirXBytecode.getBytecodeStream();
            if (bytecodeList.size() > 0)
            {
                const string newOutputFname = outputDir + xkslInputFile + ".spv";
                glslang::OutputSpvBin(bytecodeList, newOutputFname.c_str());
            }

            //dissassemble the bytecode to display and save it
            string bytecodeTxt;
            Converter::ConvertBytecodeToAscii(bytecodeList, bytecodeTxt);
            //cout << "SPX bytecode:" << endl;
            //cout << bytecodeTxt << endl;

            //write the dissassembled bytecode
            const string outputFileName = xkslInputFile + ".hr.spv";
            const string outputFullName = outputDir + outputFileName;
            xkslangtest::Utils::WriteFile(outputFullName, bytecodeTxt);
            cout << " output: \"" << outputFileName << "\"" << endl;
        }
    }

    return success;
}

static string shaderFilesPrefix;
static vector<string> libraryResourcesFolders;
static bool callbackRequestDataForShader(const string& shaderName, string& shaderData)
{
    cout << " Parsing shader file: " << shaderName << endl;

    for (int i = 0; i < 2; i++)
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
    
    cout << "Cannot find data for shader: " << shaderName << endl;
    return false;
}

static bool RecursivelyParseAndConvertXkslShader(XkslParser* parser, string& shaderName, string& filesPrefix, const vector<ShaderGenericValues>& listGenericsValue, SpxBytecode& spirXBytecode,
    bool writeOutputsOnDisk, string& spxOutputFileName)
{
    cout << "Parsing XKSL shader \"" << filesPrefix + "\" (" + shaderName + ")" << endl;
    string outputFileName = "";

    ostringstream errorAndDebugMessages;

    shaderFilesPrefix = filesPrefix; //set for the callback function

    DWORD time_before, time_after;
    time_before = GetTickCount();
    bool success = parser->ConvertShaderToSpx(shaderName, callbackRequestDataForShader, listGenericsValue, spirXBytecode, &errorAndDebugMessages);
    time_after = GetTickCount();

    string infoMsg = errorAndDebugMessages.str();
    if (infoMsg.size() > 0) cout << infoMsg;

    if (!success) {
        cout << " Failed to parse the xksl shader" << endl;
        return false;
    }
    else
    {
        cout << " OK. time: " << (time_after - time_before) << "ms" << endl;

        //output binary and debug info
        if (writeOutputsOnDisk)
        {
            //write the SPIRX binary
            const vector<uint32_t>& bytecodeList = spirXBytecode.getBytecodeStream();
            if (bytecodeList.size() > 0)
            {
                const string newOutputFilename = filesPrefix + "_" + shaderName + ".spv";
                const string newOutputFullname = outputDir + newOutputFilename;
                glslang::OutputSpvBin(bytecodeList, newOutputFullname.c_str());
                outputFileName = newOutputFilename;
            }

            //dissassemble the bytecode to display and save it
            string bytecodeTxt;
            Converter::ConvertBytecodeToAscii(bytecodeList, bytecodeTxt);
            //cout << "SPX bytecode:" << endl;
            //cout << bytecodeTxt << endl;

            //write the dissassembled bytecode
            const string outputFileName = filesPrefix + "_" + shaderName + ".hr.spv";
            const string outputFullName = outputDir + outputFileName;
            xkslangtest::Utils::WriteFile(outputFullName, bytecodeTxt);
            cout << " output: \"" << outputFileName << "\"" << endl;
        }
    }

    spxOutputFileName = outputFileName;
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

class EffectMixerObject
{
public:
    SpxMixer* mixer;
    unordered_map<int, string> stagesEntryPoints;

    EffectMixerObject(SpxMixer* mixer): mixer(mixer){}
    ~EffectMixerObject() {delete mixer;}
};

static bool parseShaderWithGenericValuesFromString(vector<ShaderGenericValues>& listGenericsValue, string& txt)
{
    int len = txt.size();
    int pos = 0, end = 0;
    
    while (true)
    {
        while (pos < len && txt[pos] == ' ') pos++;
        if (pos == len) return true;

        //shader name
        end = pos + 1;
        bool hasGenerics = false;
        while (end < len)
        {
            if (txt[end] == '<') {
                hasGenerics = true;
                break;
            }
            else if (txt[end] == ' ') {
                break;
            }
            end++;   
        }

        if (end == pos + 1) return true;

        string shaderName = txt.substr(pos, end - pos);
        shaderName = Utils::trim(shaderName);

        ShaderGenericValues shaderGenerics;
        shaderGenerics.shaderName = shaderName;

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
                aGenericValue = Utils::trim(aGenericValue);

                GenericValue gv("", aGenericValue);
                shaderGenerics.genericsValue.push_back(gv);

                if (txt[end] == '>') break;
            }
        }
        pos = end + 1;

        listGenericsValue.push_back(shaderGenerics);
    }
}

static bool getNextWord(stringstream& stream, string& word)
{
    while (stream.peek() == ' ') stream.get(); // skip spaces
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
                    cout << "2 or more shaders match the unmangled shader name: " << anUnmangledShaderName << endl;
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
    //    cout << "Shader is already recorded in the map: " << fullShaderName;
    //    return false;
    //}

    mapShaderNameWithBytecode[shaderFullName] = spxBytecode;
    return true;
}

static bool ProcessEffect(XkslParser* parser, string effectName, string effectCmdLines, vector<SpxBytecode>& listBytecodeToLoad)
{
    bool success = true;

    vector<string> errorMsgs;
    DWORD time_before, time_after;
    vector<SpxBytecode*> listAllocatedBytecodes;
    unordered_map<string, SpxBytecode*> mapShaderNameWithBytecode;
    unordered_map<string, EffectMixerObject*> mixerMap;
    int operationNum = 0;

    //init library resource folders
    libraryResourcesFolders.clear();
    libraryResourcesFolders.push_back(inputDir);

    cout << "Effect: " << effectName << endl;

    //preload some spx files?
    for (unsigned int k = 0; k < listBytecodeToLoad.size(); k++)
    {
        SpxBytecode& spxBytecode = listBytecodeToLoad[k];

        vector<string> vecShaderName;
        if (!SpxMixer::GetListAllShadersFromBytecode(spxBytecode, vecShaderName, errorMsgs))
        {
            cout << "preload: failed to get the list of shader names from: " << spxBytecode.GetName() << endl;
            success = false; break;
        }

        for (unsigned int is = 0; is < vecShaderName.size(); ++is)
        {
            string shaderName = vecShaderName[is];
            if (!RecordSPXShaderBytecode(shaderName, &spxBytecode, mapShaderNameWithBytecode))
            {
                cout << "Can't add the shader into the bytecode: " << shaderName;
                return false;
            }
        }
    }

    string line, lineItem;
    stringstream ss(effectCmdLines);
    while (getline(ss, line, '\n'))
    {
        line = Utils::trim(line);
        if (line.size() == 0) continue;

        stringstream lineSs(line);
        getNextWord(lineSs, lineItem);

        if (lineItem.compare("addResourcesLibrary") == 0)
        {
            string folder;
            if (!getNextWord(lineSs, folder)) {
                cout << "convertAndLoad: failed to get the library resource folder" << endl;
                success = false; break;
            }
            folder = Utils::trim(folder, '\"');

            string path = inputDir + folder + "\\";
            libraryResourcesFolders.push_back(path);
        }
        else if (lineItem.compare("convertAndLoadRecursif") == 0)
        {
            //recursively convert and load xksl shaders

            //file prefix
            string xkslInputFilePrefix;
            if (!getNextWord(lineSs, xkslInputFilePrefix)) {
                cout << "convertAndLoad: failed to get the xksl file prefix" << endl;
                success = false; break;
            }
            xkslInputFilePrefix = Utils::trim(xkslInputFilePrefix, '\"');

            //any generic value defined?
            vector<ShaderGenericValues> listShaderAndGenerics;
            string genericsValueText;
            if (getline(lineSs, genericsValueText))
            {
                if (!parseShaderWithGenericValuesFromString(listShaderAndGenerics, genericsValueText)) {
                    cout << "convertAndLoadRecursif: failed to read the shader generics value from: " << genericsValueText << endl;
                    success = false; break;
                }
            }

            if (listShaderAndGenerics.size() == 0) {
                cout << "convertAndLoadRecursif: no shadername has been defined" << endl;
                success = false; break;
            }
            string shaderName = listShaderAndGenerics[0].shaderName;

            SpxBytecode* spxBytecode = new SpxBytecode;
            listAllocatedBytecodes.push_back(spxBytecode);
            string spxOutputFileName;
            success = RecursivelyParseAndConvertXkslShader(parser, shaderName, xkslInputFilePrefix, listShaderAndGenerics, *spxBytecode, true, spxOutputFileName);
            if (!success) {
                cout << "convertAndLoadRecursif: failed to convert the xksl file name: " << xkslInputFilePrefix << endl;
                success = false; break;
            }

            vector<string> vecShaderName;
            if (!SpxMixer::GetListAllShadersFromBytecode(*spxBytecode, vecShaderName, errorMsgs))
            {
                cout << "convertAndLoadRecursif: failed to get the list of shader names from: " << xkslInputFilePrefix << endl;
                success = false; break;
            }

            for (unsigned int is = 0; is < vecShaderName.size(); ++is)
            {
                string shaderName = vecShaderName[is];
                if (!RecordSPXShaderBytecode(shaderName, spxBytecode, mapShaderNameWithBytecode))
                {
                    cout << "Can't add the shader into the bytecode: " << shaderName;
                    return false;
                }
            }
        }
        else if (lineItem.compare("convertAndLoad") == 0)
        {
            //convert and load xksl shaders

            //file to convert
            string xkslInputFile;
            if (!getNextWord(lineSs, xkslInputFile)) {
                cout << "convertAndLoad: failed to get the xksl file name" << endl;
                success = false; break;
            }
            xkslInputFile = Utils::trim(xkslInputFile, '\"');

            //any generic value defined?
            vector<ShaderGenericValues> listGenericsValue;
            string genericsValueText;
            if (getline(lineSs, genericsValueText))
            {
                if (!parseShaderWithGenericValuesFromString(listGenericsValue, genericsValueText)) {
                    cout << "convertAndLoad: failed to read the shader generics value from: " << genericsValueText << endl;
                    success = false; break;
                }
            }

            SpxBytecode* spxBytecode = new SpxBytecode;
            listAllocatedBytecodes.push_back(spxBytecode);
            success = ParseAndConvertXkslFile(parser, xkslInputFile, listGenericsValue, *spxBytecode, true);
            if (!success) {
                cout << "convertAndLoad: failed to convert the xksl file name: " << xkslInputFile << endl;
                success = false; break;
            }
            
            vector<string> vecShaderName;
            if (!SpxMixer::GetListAllShadersFromBytecode(*spxBytecode, vecShaderName, errorMsgs))
            {
                cout << "convertAndLoad: failed to get the list of shader names from: " << xkslInputFile << endl;
                success = false; break;
            }

            for (unsigned int is = 0; is < vecShaderName.size(); ++is)
            {
                string shaderName = vecShaderName[is];
                if (!RecordSPXShaderBytecode(shaderName, spxBytecode, mapShaderNameWithBytecode))
                {
                    cout << "Can't add the shader into the bytecode: " << shaderName;
                    return false;
                }
            }
        }
        else if (lineItem.compare("mixer") == 0)
        {
            string mixerName;
            if (!getNextWord(lineSs, mixerName)) {
                cout << "mixer: failed to get the xksl file name" << endl;
                success = false; break;
            }
            mixerName = Utils::trim(mixerName, '\"');

            if (mixerMap.find(mixerName) != mixerMap.end()) {
                cout << "mixer: a mixer already exists with the name:" << mixerName << endl;
                success = false; break;
            }

            SpxMixer* mixer = new SpxMixer();
            mixerMap[mixerName] = new EffectMixerObject(mixer);
        }
        else if (lineItem.size() >= 2 && lineItem[0] == '/' && lineItem[1] == '/')
        {
            //comment: ignore the line
        }
        else
        {
            string mixerName, instruction;
            if (!SeparateAdotB(lineItem, mixerName, instruction)) {
                cout << "Unknown instruction: " << lineItem << endl;
                success = false; break;
            }

            if (mixerMap.find(mixerName) == mixerMap.end()) {
                cout << lineItem << ": no mixer found with the name:" << mixerName << endl;
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
                        cout << "cannot find a bytecode in the shader librady for the shader: " << shaderName << endl;
                        success = false; break;
                    }
                    if (spxBytecode == nullptr) spxBytecode = aShaderBytecode;
                    else
                    {
                        if (spxBytecode != aShaderBytecode) {
                            cout << "2 shaders to mix are defined in different bytecode (maybe we could merge them)" << endl;
                            success = false; break;
                        }
                    }

                    listShaderToMix.push_back(shaderFullName);
                }
                if (spxBytecode == nullptr)
                {
                    cout << "No spxBytecode found for the mixin instruction" << endl;
                    success = false; break;
                }

                cout << "mixin: " << line << endl;
                time_before = GetTickCount();
                success = mixerTarget->mixer->Mixin(*spxBytecode, listShaderToMix, errorMsgs);
                time_after = GetTickCount();

                //write the current bytecode
                const string outputFileName = effectName + "_op" + to_string(operationNum++) + "_mixin" + ".hr.spv";
                SaveCurrentMixerBytecode(mixerTarget->mixer, outputDir, outputFileName);

                if (success)
                    cout << " OK. Time:  " << (time_after - time_before) << "ms" << endl;
                else
                    cout << "Mixin failed" << endl;
            }
            else if (instruction.compare("addComposition") == 0)
            {
                string compositionTargetStr;
                if (!getNextWord(lineSs, compositionTargetStr)) {
                    cout << "Expecting composition target" << endl;
                    success = false; break;
                }

                string shaderName, variableName;
                if (!SeparateAdotB(compositionTargetStr, shaderName, variableName)) {
                    cout << "Unknown instruction: " << compositionTargetStr << endl;
                    success = false; break;
                }

                string mixerSourceName;
                if (!getNextWord(lineSs, mixerSourceName)) {
                    cout << "Expecting mixer composition source" << endl;
                    success = false; break;
                }

                if (mixerMap.find(mixerSourceName) == mixerMap.end()) {
                    cout << lineItem << ": no mixer found with the name:" << mixerSourceName << endl;
                    success = false; break;
                }
                EffectMixerObject* mixerSource = mixerMap[mixerSourceName];

                cout << "Adding composition: " << line << endl;
                time_before = GetTickCount();
                success = mixerTarget->mixer->AddComposition(shaderName, variableName, mixerSource->mixer, errorMsgs);
                time_after = GetTickCount();

                //write the current bytecode
                const string outputFileName = effectName + "_op" + to_string(operationNum++) + "_compose" + ".hr.spv";
                SaveCurrentMixerBytecode(mixerTarget->mixer, outputDir, outputFileName);

                if (success)
                    cout << " OK. Time:  " << (time_after - time_before) << "ms" << endl;
                else
                    cout << "Failed to add the composition to the mixer" << endl;
            }
            else if (instruction.compare("setStageEntryPoint") == 0)
            {
                string stageStr;
                if (!getNextWord(lineSs, stageStr)) {
                    cout << "Expecting stage" << endl;
                    success = false; break;
                }
                ShadingStageEnum stage;
                if (!GetShadingStageForString(stageStr, stage)) {
                    cout << "Unknown stage:" << stageStr;
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
                vector<OutputStageBytecode> outputStages;
                for (auto its = mixerTarget->stagesEntryPoints.begin(); its != mixerTarget->stagesEntryPoints.end(); its++){
                    if (its->second.size() > 0)
                        outputStages.push_back(OutputStageBytecode(ShadingStageEnum(its->first), its->second));
                }

                success = CompileMixer(effectName, mixerTarget->mixer, outputStages, errorMsgs);
                if (!success)
                {
                    cout << "Failed to compile the effect: " << effectName << endl;
                }
            }
            else {
                cout << "Unknown instruction: " << instruction << endl;
                success = false; break;
            }
        }

        if (!success) break;
    }

    for (auto itm = mixerMap.begin(); itm != mixerMap.end(); itm++)
        delete (*itm).second;

    for (auto itv = listAllocatedBytecodes.begin(); itv != listAllocatedBytecodes.end(); itv++)
        delete (*itv);

    if (errorMsgs.size() > 0)
    {
        cout << "   Messages:" << endl;
        for (unsigned int m = 0; m<errorMsgs.size(); m++) cout << "   " << errorMsgs[m] << "" << endl;
    }

    return success;
}

static bool ProcessEffect(XkslParser* parser, XkfxEffectsToProcess& effect)
{
    string effectName = effect.effectName;

    bool success = true;
    const string inputFname = inputDir + "\\" + effect.inputFileName;
    string effectCmdLines;
    if (!Utils::ReadFile(inputFname, effectCmdLines))
    {
        cout << " Failed to read the file: " << inputFname << " !!!" << endl;
        return false;
    }

    //Utils::replaceAll(effectCmdLines, "load ", "convertAndLoad ");
    //Utils::WriteFile(inputFname, effectCmdLines);

    vector<SpxBytecode> listBytecodeToLoad;
    return ProcessEffect(parser, effectName, effectCmdLines, listBytecodeToLoad);
}

void main(int argc, char** argv)
{
#ifdef _DEBUG
    cout << "DEBUG mode" << endl << endl;
#else
    cout << "RELEASE mode" << endl << endl;
#endif

    //Xkslang::XkslangDLL::TestAdd(0, 1);

    if (!SetupTestDirectories())
    {
        cout << "Failed to setup the directories" << endl;
        return;
    }

    XkslParser parser;
    if (!parser.InitialiseXkslang())
    {
        cout << "Failed to initialize the XkslParser" << endl;
        return;
    }

    //====================================================================================================================
    //====================================================================================================================
    cout << "___________________________________________________________________________________" << endl;
    cout << "Parse and convert XKSL Files:" << endl << endl;
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

            // parse and convert all xksl files
            SpxBytecode spirXBytecode;
            success = ParseAndConvertXkslFile(&parser, xkslShaderInputFile, listGenericsValue, spirXBytecode, true);

            if (success) countParsingSuccessful++;
            else listXkslParsingFailed.push_back(xkslShaderInputFile);

            cout << endl;
        }
    }

    //====================================================================================================================
    //====================================================================================================================
    cout << endl;
    cout << "___________________________________________________________________________________" << endl;
    cout << "Process XKFX Effect  Files:" << endl << endl;

    int countEffectsProcessed = 0;
    int countEffectsSuccessful = 0;
    vector<string> listEffectsFailed;
    //Parse the effects
    {
        for (unsigned int n = 0; n < vecXkfxEffectToProcess.size(); ++n)
        {
            XkfxEffectsToProcess effect = vecXkfxEffectToProcess[n];
            countEffectsProcessed++;

            SpxMixer::StartMixin();
            bool success = ProcessEffect(&parser, effect);
            SpxMixer::ReleaseMixin();

            if (success) countEffectsSuccessful++;
            else listEffectsFailed.push_back(effect.effectName);

            cout << endl;
        }
    }

    if (vecSpvFileToConvertToGlslAndHlsl.size() > 0)
    {
        cout << endl;
        cout << "___________________________________________________________________________________" << endl;
        cout << "Convert SPV Files:" << endl << endl;

        for (unsigned int n = 0; n < vecSpvFileToConvertToGlslAndHlsl.size(); ++n)
        {
            bool success = true;
            int result = 0;
            XkfxEffectsToProcess effect = vecSpvFileToConvertToGlslAndHlsl[n];
            string inputFileSpv = inputDir + effect.inputFileName;

            string xkslInput;
            if (!Utils::ReadFile(inputFileSpv, xkslInput))
            {
                cout << " Failed to read the file: " << inputFileSpv << endl;
                success = false;
            }

            if (success)
            {
                //======================================================================
                //GLSL
                string outputNameGlsl = effect.inputFileName + ".glsl";
                string outputFullNameGlsl = outputDir + outputNameGlsl;

                cout << "Convert into GLSL: " << effect.inputFileName << endl;
                result = ConvertSpvToShaderLanguage(inputFileSpv, outputFullNameGlsl, ShaderLanguageEnum::GlslLanguage);
                if (result != 0) success = false;

                if (success) cout << " OK." << endl;
                else cout << " Failed to convert the SPIRV file to GLSL" << endl;

                //======================================================================
                //HLSL
                string outputNameHlsl = effect.inputFileName + ".hlsl";
                string outputFullNameHlsl = outputDir + outputNameHlsl;

                cout << "Convert into GLSL: " << effect.inputFileName << endl;
                result = ConvertSpvToShaderLanguage(inputFileSpv, outputFullNameHlsl, ShaderLanguageEnum::HlslLanguage);
                if (result != 0) success = false;
            }

            if (success) cout << " OK." << endl;
            else cout << " Failed to convert the SPIRV file to HLSL" << endl;
        }
    }
    
    cout << endl;
    cout << "___________________________________________________________________________________" << endl;
    cout << "Results:" << endl << endl;

    //==========================================================
    cout << "Count Xksl files parsed: " << countParsingProcessed << endl;
    cout << "Count Xksl files successful: " << countParsingSuccessful << endl;
    if (listXkslParsingFailed.size() > 0)
    {
        cout << "  Failed Xksl Files:" << endl;
        for (unsigned int i = 0; i<listXkslParsingFailed.size(); ++i) cout << listXkslParsingFailed[i] << endl;
    }
    cout << endl;

    //==========================================================
    cout << "Count Effects processed: " << countEffectsProcessed << endl;
    cout << "Count Effects successful: " << countEffectsSuccessful << endl;
    if (listEffectsFailed.size() > 0)
    {
        cout << "  Failed Effects:" << endl;
        for (unsigned int i = 0; i<listEffectsFailed.size(); ++i) cout << listEffectsFailed[i] << endl;
    }
    cout << endl;

    parser.Finalize();

}

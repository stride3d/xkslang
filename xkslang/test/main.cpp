//
// Copyright (C)

#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include <iostream>
#include <memory>
#include <string>

#include "SPIRV/doc.h"
#include "SPIRV/disassemble.h"
#include "SPIRV/GlslangToSpv.h"

#include "Utils.h"
#include "../source/SpxBytecode.h"
#include "../source/XkslParser.h"
#include "../source/XkslMixer.h"

using namespace std;
using namespace xkslangtest;
using namespace xkslang;

struct FileNameEntryPointPair {
    const char* fileName;
    const char* entryPoint;
    const char* expectedGlslOutput;
};

static string testDir = "glslang\\source\\Test\\xksl";

vector<FileNameEntryPointPair> testFiles = {
    //{"shaderWithVariable.xksl", "", nullptr},
    //{"shaderWithManyVariables.xksl", "", nullptr},
    //{"manySimpleShaders.xksl", "", nullptr},
    //{"simpleShaderWithFunction.xksl", "", nullptr},
    //{"declarationMixOfFunctionsAndVariables.xksl", "", nullptr},
    //{"2ShaderWithSameFunctionNames.xksl", "", nullptr},
    //{"shaderInheritance.xksl", "", nullptr},
    //{"postDeclaration.xksl", "", nullptr},
    //{"classAccessor.xksl", "", nullptr},
    //{"streamsSimple.xksl", "", nullptr},
    //{"streamsWithClassAccessor.xksl", "", nullptr},
    //{"shaderWithDefinedConsts.xksl", "", nullptr},
    //{"shaderWithUnresolvedConsts.xksl", "", nullptr},
    //{"intrisicsHlslFunctions.xksl", "" },
    //{"methodReferingToShaderVariable.xksl", "", nullptr},
    //{"methodsWithSimpleClassAccessor.xksl", "", nullptr},

    //{"cbuffers.xksl", "", nullptr},

    { "TestMixin01_Base.xksl", "main", "TestMixin01_Base.xksl_Pixel.glsl" },
    { "TestMixin01_Override.xksl", "main", "TestMixin01_Override.xksl_Pixel.glsl" },
    { "TestMixin01_OverridePlusCallBase.xksl", "main", "TestMixin01_OverridePlusCallBase.xksl_Pixel.glsl" },

    //{"textureAndSampler.xksl", "", nullptr},
    //{"shaderTexturing.xksl", "", nullptr},
    //{"shaderBase.xksl", "", nullptr},
    //{"shaderSimple.xksl", "", nullptr},

    //{"shaderCustomEffect.xksl", "", nullptr},
    //{"methodsPrototypes.xksl", "", nullptr},
};

#ifdef _DEBUG
static string spirvCrossExe = "D:/Prgms/glslang/source/bin/spirv-cross/SPIRV-Cross_d.exe";
#else
static string spirvCrossExe = "D:/Prgms/glslang/source/bin/spirv-cross/SPIRV-Cross.exe";
#endif

int ConvertSpvToGlsl(string spvFile, string glslFile)
{
    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    string commandLine = spirvCrossExe + string(" --output ") + glslFile + string(" ") + spvFile;

    const char* cl = commandLine.c_str();
    wchar_t wtext[256];
    mbstowcs(wtext, cl, strlen(cl) + 1);
    LPWSTR ptr = wtext;

    // Start the child process. 
    if (!CreateProcess(NULL,     // No module name (use command line)
        ptr,                     // Command line
        NULL,                    // Process handle not inheritable
        NULL,                    // Thread handle not inheritable
        FALSE,                   // Set handle inheritance to FALSE
        0,                       // No creation flags
        NULL,                    // Use parent's environment block
        NULL,                    // Use parent's starting directory 
        &si,                     // Pointer to STARTUPINFO structure
        &pi)                     // Pointer to PROCESS_INFORMATION structure
        )
    {
        printf("CreateProcess failed (%d).\n", GetLastError());
        return GetLastError();
    }

    // Wait until child process exits.
    WaitForSingleObject(pi.hProcess, INFINITE);

    // Close process and thread handles. 
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    return 0;
}

void SetupTestDirectories()
{
    //Get app directories (unclean and hardcoded code for now)
    WCHAR fileName[MAX_PATH];
    int bytes = GetModuleFileName(NULL, fileName, MAX_PATH);
    wstring ws(fileName);
    string fullName(ws.begin(), ws.end());
    const size_t pos = fullName.find("glslang");
    string dir = fullName.substr(0, pos);
    testDir = dir + testDir;
}

void main(int argc, char** argv)
{
#ifdef _DEBUG
    cout << "DEBUG mode" << endl << endl;
#else
    cout << "RELEASE mode" << endl << endl;
#endif

    SetupTestDirectories();
    DWORD time_before, time_after;

    XkslParser parser;
    if (!parser.InitialiseXkslang())
    {
        cout << "Failed to initialize the parser" << endl;
        return;
    }

    //Parse the shaders using XkslParser library
    {
        for (int i = 0; i < testFiles.size(); ++i)
        {
            //======================================================================================================
            //======================================================================================================
            //======================================================================================================
            // load shader file into a string
            string shaderFileName = testFiles[i].fileName;

            cout << "Parsing XKSL shader \"" << shaderFileName << "\"" << endl;

            const string inputFname = testDir + "/" + shaderFileName;
            string xkslInput;
            if (!Utils::ReadFile(inputFname, xkslInput))
            {
                cout << " Failed to read the file: " << inputFname << " !!!" << endl;
                continue;
            }

            //======================================================================================================
            //======================================================================================================
            //======================================================================================================
            // Parse and convert XKSL shaders to SPIRX bytecode
            SpxBytecode spirXBytecode;
            ostringstream errorAndDebugMessages;
            ostringstream outputHumanReadableASTAndSPV;

            time_before = GetTickCount();
            bool success = parser.ConvertXkslToSpirX(shaderFileName, xkslInput, spirXBytecode, &errorAndDebugMessages, &outputHumanReadableASTAndSPV);
            time_after = GetTickCount();

            if (!success) {
                cout << " Failed to parse the shader: " << shaderFileName << " !!!!!!!" << endl;
                continue;
            }
            cout << " OK. time: " << (time_after - time_before) << " ms" << endl;

            //output binary and debug info
            {
                //output the SPIRX binary
                const vector<uint32_t>& bytecodeList = spirXBytecode.getBytecodeStream();
                if (bytecodeList.size() > 0)
                {
                    const string newOutputFname = testDir + "/" + shaderFileName + ".spv";
                    glslang::OutputSpvBin(bytecodeList, newOutputFname.c_str());
                }

                //output the AST and debug and HR spirv binary
                const string newOutputFname = testDir + "/" + shaderFileName + ".hr.spv";
                errorAndDebugMessages << outputHumanReadableASTAndSPV.str();
                xkslangtest::Utils::WriteFile(newOutputFname, errorAndDebugMessages.str());
            }

            //======================================================================================================
            //======================================================================================================
            // Mixin
            string entryPoint = testFiles[i].entryPoint;
            if (entryPoint.length() > 0)
            {
                cout << "Mixin shader \"" << shaderFileName << "\"" << endl;

                time_before = GetTickCount();

                //Add mixin files
                XkslMixer mixer;
                mixer.AddMixin(&spirXBytecode);

                //Create mixin
                vector<string> errorMsgs;
                bool success = mixer.MergeAllMixin(errorMsgs);

                time_after = GetTickCount();

                if (success) cout << " OK. time: " << (time_after - time_before) << " ms" << endl;
                else cout << " Mixin Failed !!!" << endl;
                
                //Save the mixin SPIRX bytecode (HR form)
                if (success)
                {
                    // dissassemble the binary
                    SpxBytecode mixinBytecode;
                    mixer.GetMixinBytecode(mixinBytecode, errorMsgs);
                    const vector<uint32_t>& bytecodeList = mixinBytecode.getBytecodeStream();
                    ostringstream disassembly_stream;
                    spv::Parameterize();
                    spv::Disassemble(disassembly_stream, bytecodeList);

                    const string newOutputFname = testDir + "/" + shaderFileName + "_mixin" + ".hr.spv";
                    xkslangtest::Utils::WriteFile(newOutputFname, disassembly_stream.str());
                }

                //======================================================================================================
                //======================================================================================================
                //Generate stage SPIRV bytecode
                if (success)
                {
                    SpvBytecode stageBytecode;
                    ShadingStage stage = ShadingStage::Pixel;

                    cout << "Generate SPIRV bytecode for entry point=\"" << entryPoint << "\" stage=\"" << GetStageLabel(stage) << "\"" << endl;

                    time_before = GetTickCount();
                    success = mixer.GenerateStageBytecode(stage, entryPoint, stageBytecode, errorMsgs);
                    time_after = GetTickCount();

                    if (success) cout << " OK. time: " << (time_after - time_before) << " ms" << endl;
                    else cout << " Fail to generate the SPIRV bytecode !!!" << endl;

                    //Save the SPIRV bytecode (and its HR form)
                    if (success)
                    {
                        //output the binary
                        const vector<uint32_t>& bytecodeList = stageBytecode.getBytecodeStream();
                        string outputNameSpv = testDir + "/" + shaderFileName + "_" + GetStageLabel(stage) + ".spv";
                        glslang::OutputSpvBin(bytecodeList, outputNameSpv.c_str());

                        // dissassemble the binary
                        ostringstream disassembly_stream;
                        spv::Parameterize();
                        spv::Disassemble(disassembly_stream, bytecodeList);

                        string outputNameHrSpv = testDir + "/" + shaderFileName + "_" + GetStageLabel(stage) + ".hr.spv";
                        xkslangtest::Utils::WriteFile(outputNameHrSpv, disassembly_stream.str());

                        //======================================================================================================
                        //======================================================================================================
                        //convert back the SPIRV bytecode into GLSL
                        {
                            string outputNameGlsl = testDir + "/" + shaderFileName + "_" + GetStageLabel(stage) + ".rv.glsl";

                            cout << "Convert SPIRV bytecode to GLSL." << endl;

                            time_before = GetTickCount();
                            int result = ConvertSpvToGlsl(outputNameSpv, outputNameGlsl);
                            time_after = GetTickCount();

                            if (success) cout << " OK. time: " << (time_after - time_before) << " ms" << endl;
                            else cout << " Fail to convert the SPIRV file to GLSL" << endl;

                            //compare the glsl output with the expected output
                            const char* expectedGlslOutput = testFiles[i].expectedGlslOutput;
                            if (expectedGlslOutput != nullptr)
                            {
                                string expectedOutputFileName = testDir + "\\expectedOutput\\" + string(expectedGlslOutput);
                                string glslExpectedOutput;
                                if (Utils::ReadFile(expectedOutputFileName, glslExpectedOutput))
                                {
                                    string glslConvertedOutput;
                                    if (Utils::ReadFile(outputNameGlsl, glslConvertedOutput))
                                    {
                                        if (glslExpectedOutput.compare(glslConvertedOutput) != 0)
                                        {
                                            cout << "expected output:" << endl << glslExpectedOutput;
                                            cout << "output:" << endl << glslConvertedOutput;
                                            cout << " Glsl output and expected output are different !!!" << endl;
                                        }
                                        else cout << " GLSL output VS expected output: OK" << endl;                                    }
                                    else cout << " Failed to read the file: " << outputNameGlsl << " !!!" << endl;
                                }
                                else cout << " Failed to read the file: " << expectedOutputFileName << " !!!" << endl;
                            }
                        }
                    }
                }

                if (errorMsgs.size() > 0)
                {
                    cout << "   Messages:" << endl;
                    for (int m=0; m<errorMsgs.size(); m++) cout << "   " << errorMsgs[m] << "" << endl;
                }
            }

            cout << endl;
        }
    }

    parser.Finalize();

}


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
};

static string testDir = "glslang\\source\\Test\\xksl";

vector<FileNameEntryPointPair> testFiles = {
    //{"shaderWithVariable.xksl", ""},
    //{"shaderWithManyVariables.xksl", ""},
    //{"manySimpleShaders.xksl", ""},
    //{"simpleShaderWithFunction.xksl", ""},
    //{"declarationMixOfFunctionsAndVariables.xksl", ""},
    //{"2ShaderWithSameFunctionNames.xksl", ""},
    //{"shaderInheritance.xksl", ""},
    //{"postDeclaration.xksl", ""},
    //{"classAccessor.xksl", ""},
    //{"streamsSimple.xksl", ""},
    //{"streamsWithClassAccessor.xksl", ""},
    //{"shaderWithDefinedConsts.xksl", ""},
    //{"shaderWithUnresolvedConsts.xksl", ""},
    //{"intrisicsHlslFunctions.xksl", "" },
    //{"methodReferingToShaderVariable.xksl", ""},
    //{"methodsWithSimpleClassAccessor.xksl", ""},

    //{"cbuffers.xksl", ""},

    //{ "TestMixin01_Base.xksl", "main" },
    //{ "TestMixin01_Override.xksl", "main" },
    { "TestMixin01_OverridePlusCallBase.xksl", "main" },

    //{"textureAndSampler.xksl", ""},
    //{"shaderTexturing.xksl", ""},
    //{"shaderBase.xksl", ""},
    //{"shaderSimple.xksl", ""},

    //{"shaderCustomEffect.xksl", ""},
    //{"methodsPrototypes.xksl", ""},
};


int ConvertSpvToGlsl(string spvFile, string glslFile)
{
    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    string spirvCrossExe = "D:/Prgms/glslang/source/bin/spirv-cross/SPIRV-Cross.exe";
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
    SetupTestDirectories();

    XkslParser parser;
    if (!parser.InitialiseXkslang())
    {
        cout << "Failed to initialize the parser\n";
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

            cout << "Parsing shader:" << shaderFileName << "...\n";

            const string inputFname = testDir + "/" + shaderFileName;
            string xkslInput;
            if (!Utils::ReadFile(inputFname, xkslInput))
            {
                cout << "  Failed to read the file: " << inputFname << " !!!\n";
                continue;
            }

            //======================================================================================================
            //======================================================================================================
            //======================================================================================================
            // Parse and convert XKSL shaders to SPIRX bytecode
            SpxBytecode spirXBytecode;
            ostringstream errorAndDebugMessages;
            ostringstream outputHumanReadableASTAndSPV;

            bool success = parser.ConvertXkslToSpirX(shaderFileName, xkslInput, spirXBytecode, &errorAndDebugMessages, &outputHumanReadableASTAndSPV);

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

            if (!success) {
                cout << "  Failed to parse the shader: " << shaderFileName << " !!!!!!!\n";
                continue;
            }
            cout << "  Parsing successful\n";

            //======================================================================================================
            //======================================================================================================
            //======================================================================================================
            // Mixin
            string entryPoint = testFiles[i].entryPoint;
            if (entryPoint.length() > 0)
            {
                cout << "Mixin shader:" << shaderFileName << "...\n";

                //Add mixin files
                XkslMixer mixer;
                mixer.AddMixin(&spirXBytecode);

                //Create mixin
                vector<string> errorMsgs;
                bool success = mixer.MergeAllMixin(errorMsgs);

                if (success) cout << " Mixin successful\n";
                else cout << " Mixin Failed !!!\n";

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

                //Generate stage SPIRV bytecode
                if (success)
                {
                    SpvBytecode stageBytecode;
                    ShadingStage stage = ShadingStage::Pixel;

                    cout << " Generate SPIRV bytecode for entry point:" << entryPoint << " stage:" << GetStageLabel(stage) << "\n";
                    success = mixer.GenerateStageBytecode(stage, entryPoint, stageBytecode, errorMsgs);

                    if (success) cout << " Bytecode successfully generated\n";
                    else cout << " Fail to generate the bytecode !!!\n";

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

                        //convert back the SPIRV bytecode into GLSL
                        {
                            string outputNameGlsl = testDir + "/" + shaderFileName + "_" + GetStageLabel(stage) + ".glsl";
                            int result = ConvertSpvToGlsl(outputNameSpv, outputNameGlsl);
                            if (result != 0) {
                                cout << " Fail to convert the SPV file to GLSL\n";
                            }
                        }
                    }
                }

                if (errorMsgs.size() > 0)
                {
                    cout << "   Messages:\n";
                    for (int m=0; m<errorMsgs.size(); m++) cout << "   " << errorMsgs[m] << "\n";
                }
            }

            cout << "\n";
        }
    }

    parser.Finalize();

}


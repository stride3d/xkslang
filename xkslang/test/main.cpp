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

struct XkslInput {
    const char* fileName;
};

struct MixinOutput {
    const char* entryPoint;
    ShadingStage stage;
    const char* expectedGlslOutput;
};

//To test single file parsing and convertion
struct XkslFilesToParseAndConvert {
    vector<XkslInput> inputs;
    vector<MixinOutput> outputs;
};

static string testDir = "glslang\\source\\Test\\xksl";

vector<XkslFilesToParseAndConvert> vecXkslFilesToConvert = {
    //{ {{"shaderWithVariable.xksl"}}, {} },
    //{ {{"shaderWithManyVariables.xksl"}},{} },
    //{ {{"manySimpleShaders.xksl"}},{} },
    //{ {{"simpleShaderWithFunction.xksl"}},{} },
    //{ {{"declarationMixOfFunctionsAndVariables.xksl"}},{} },
    //{ {{"2ShaderWithSameFunctionNames.xksl"}},{} },
    //{ {{"shaderInheritance.xksl"}},{} },
    //{ {{"postDeclaration.xksl"}},{} },
    //{ {{"classAccessor.xksl"}},{} },
    //{ {{"streamsSimple.xksl"}},{} },
    //{ {{"streamsWithClassAccessor.xksl"}},{} },
    //{ {{"shaderWithDefinedConsts.xksl"}},{} },
    //{ {{"shaderWithUnresolvedConsts.xksl"}},{} },
    //{ {{"intrisicsHlslFunctions.xksl"}},{} },
    //{ {{"methodReferingToShaderVariable.xksl"}},{} },
    //{ {{"methodsWithSimpleClassAccessor.xksl"}},{} },
    //{ {{"cbuffers.xksl"}},{} },

    //{ {{"TestMixin01_Base.xksl"}}, {{"main", ShadingStage::Pixel, "TestMixin01_Base.xksl_Pixel.glsl"}} },
    //{ {{"TestMixin01_Override.xksl"}}, {{"main", ShadingStage::Pixel, "TestMixin01_Override.xksl_Pixel.glsl"}} },
    { {{"TestMixin01_OverridePlusCallBase.xksl"}}, {{"main", ShadingStage::Pixel, "TestMixin01_OverridePlusCallBase.xksl_Pixel.glsl"}} },
    //{ {"TestMixin01_2Overrides.xksl"}, {"main", nullptr}/*"TestMixin01_OverridePlusCallBase.xksl_Pixel.glsl"*/ },

    //{{"textureAndSampler.xksl"}, {"", nullptr}},
    //{{"shaderTexturing.xksl"}, {"", nullptr}},
    //{{"shaderBase.xksl"}, {"", nullptr}},
    //{{"shaderSimple.xksl"}, {"", nullptr}},

    //{{"shaderCustomEffect.xksl"}, {"", nullptr}},
    //{{"methodsPrototypes.xksl"}, {"", nullptr}},
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

    int countTestProcessed = 0;
    int countTestSuccessful = 0;

    //Parse the shaders using XkslParser library
    {
        for (int n = 0; n < vecXkslFilesToConvert.size(); ++n)
        {
            XkslFilesToParseAndConvert& xkslFilesToParseAndConvert = vecXkslFilesToConvert[n];
            vector<XkslInput>& inputs = xkslFilesToParseAndConvert.inputs;
            vector<MixinOutput>& outputs = xkslFilesToParseAndConvert.outputs;
            if (inputs.size() == 0) continue;

            countTestProcessed++;
            bool success = true;

            vector<SpxBytecode> listInputBytecodes;

            //======================================================================================================
            //======================================================================================================
            // load shader file into a string
            string xkslShaderFileName = inputs[0].fileName;

            cout << "Parsing XKSL shader \"" << xkslShaderFileName << "\"" << endl;

            const string inputFname = testDir + "/" + xkslShaderFileName;
            string xkslInput;
            if (!Utils::ReadFile(inputFname, xkslInput))
            {
                cout << " Failed to read the file: " << inputFname << " !!!" << endl;
                success = false;
            }

            //======================================================================================================
            //======================================================================================================
            // Parse and convert XKSL shaders to SPIRX bytecode
            if (success)
            {
                SpxBytecode spirXBytecode;
                ostringstream errorAndDebugMessages;
                ostringstream outputHumanReadableASTAndSPV;

                time_before = GetTickCount();
                bool success = parser.ConvertXkslToSpirX(xkslShaderFileName, xkslInput, spirXBytecode, &errorAndDebugMessages, &outputHumanReadableASTAndSPV);
                time_after = GetTickCount();

                if (!success) {
                    cout << " Failed to parse the shader: " << xkslShaderFileName << " !!!!!!!" << endl;
                }
                else
                {
                    cout << " OK. time: " << (time_after - time_before) << " ms" << endl;

                    //output binary and debug info
                    {
                        //output the SPIRX binary
                        const vector<uint32_t>& bytecodeList = spirXBytecode.getBytecodeStream();
                        if (bytecodeList.size() > 0)
                        {
                            const string newOutputFname = testDir + "/" + xkslShaderFileName + ".spv";
                            glslang::OutputSpvBin(bytecodeList, newOutputFname.c_str());
                        }

                        //output the AST and debug and HR spirv binary
                        const string newOutputFname = testDir + "/" + xkslShaderFileName + ".hr.spv";
                        errorAndDebugMessages << outputHumanReadableASTAndSPV.str();
                        xkslangtest::Utils::WriteFile(newOutputFname, errorAndDebugMessages.str());
                    }
                }

                listInputBytecodes.push_back(spirXBytecode);
            }

            //======================================================================================================
            //======================================================================================================
            // Mixin all inputs
            if (success && outputs.size() > 0)
            {
                cout << "Mixin SPIRX shaders" << endl;

                time_before = GetTickCount();
                XkslMixer mixer;
                vector<string> errorMsgs;
                for (int i=0; i<listInputBytecodes.size(); ++i)
                {
                    const SpxBytecode& spirXBytecode = listInputBytecodes[i];

                    cout << " Mixin" << spirXBytecode.GetName() << ": ";

                    //Add mixin files
                    success = mixer.Mixin(spirXBytecode, errorMsgs);
                    if (!success){
                        cout << "Mixin failed !!" << endl;
                        break;
                    }

                    cout << "OK" << endl;
                }
                time_after = GetTickCount();

                /////Create mixin
                ///bool success = mixer.MergeAllMixin(errorMsgs);
                ///time_after = GetTickCount();

                if (!success) cout << " Mixin Failed !!!" << endl;
                else
                {
                    cout << " Mixin completed. time: " << (time_after - time_before) << " ms" << endl;

                    //Save the mixin SPIRX bytecode (HR form)
                    SpxBytecode mixinBytecode;
                    success = mixer.GetMixinBytecode(mixinBytecode, errorMsgs);
                    if (!success) cout << " Failed to get the mixin bytecode" << endl;

                    const vector<uint32_t>& bytecodeList = mixinBytecode.getBytecodeStream();
                    ostringstream disassembly_stream;
                    spv::Parameterize();
                    spv::Disassemble(disassembly_stream, bytecodeList);

                    const string newOutputFname = testDir + "/" + xkslShaderFileName + "_mixin" + ".hr.spv";
                    xkslangtest::Utils::WriteFile(newOutputFname, disassembly_stream.str());
                }

                //======================================================================================================
                //======================================================================================================
                //Generate outputs SPIRV bytecode
                if (success)
                {
                    for (int i=0; i<outputs.size(); ++i)
                    {
                        SpvBytecode stageBytecode;
                        ShadingStage stage = outputs[i].stage;
                        const char* entryPoint = outputs[i].entryPoint;

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
                            string outputNameSpv = testDir + "/" + xkslShaderFileName + "_" + GetStageLabel(stage) + ".spv";
                            glslang::OutputSpvBin(bytecodeList, outputNameSpv.c_str());

                            // dissassemble the binary
                            ostringstream disassembly_stream;
                            spv::Parameterize();
                            spv::Disassemble(disassembly_stream, bytecodeList);

                            string outputNameHrSpv = testDir + "/" + xkslShaderFileName + "_" + GetStageLabel(stage) + ".hr.spv";
                            xkslangtest::Utils::WriteFile(outputNameHrSpv, disassembly_stream.str());

                            //======================================================================================================
                            //======================================================================================================
                            //convert back the SPIRV bytecode into GLSL
                            {
                                string outputNameGlsl = testDir + "/" + xkslShaderFileName + "_" + GetStageLabel(stage) + ".rv.glsl";

                                cout << "Convert SPIRV bytecode to GLSL." << endl;

                                time_before = GetTickCount();
                                int result = ConvertSpvToGlsl(outputNameSpv, outputNameGlsl);
                                time_after = GetTickCount();

                                if (success) cout << " OK. time: " << (time_after - time_before) << " ms" << endl;
                                else cout << " Fail to convert the SPIRV file to GLSL" << endl;

                                //compare the glsl output with the expected output
                                const char* expectedGlslOutput = outputs[i].expectedGlslOutput;
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
                }

                if (errorMsgs.size() > 0)
                {
                    cout << "   Messages:" << endl;
                    for (int m=0; m<errorMsgs.size(); m++) cout << "   " << errorMsgs[m] << "" << endl;
                }
            }

            if (success) countTestSuccessful++;

            cout << endl;
        }

        cout << "Count tests processed: " << countTestProcessed << endl;
        cout << "Count tests successful: " << countTestSuccessful << endl;
    }

    parser.Finalize();

}


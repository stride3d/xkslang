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
    ShadingStageEnum stage;
    const char* expectedGlslOutput;
};

//To test single file parsing and convertion
struct XkslFilesToParseAndConvert {
    string effectName;
    vector<XkslInput> inputs;
    vector<MixinOutput> outputs;
};

static string inputDir = "glslang\\source\\Test\\xksl\\";
static string outputDir;
static string expectedOutputDir;

vector<XkslFilesToParseAndConvert> vecXkslFilesToConvert = {
    //{ "", {{"shaderWithVariable.xksl"}}, {} },
    //{ "", {{"shaderWithManyVariables.xksl"}},{} },
    //{ "", {{"manySimpleShaders.xksl"}},{} },
    //{ "", {{"simpleShaderWithFunction.xksl"}},{} },
    //{ "", {{"declarationMixOfFunctionsAndVariables.xksl"}},{} },
    //{ "", {{"2ShaderWithSameFunctionNames.xksl"}},{} },
    //{ "", {{"shaderInheritance.xksl"}},{} },
    //{ "", {{"postDeclaration.xksl"}},{} },
    //{ "", {{"classAccessor.xksl"}},{} },
    //{ "", {{"typeDeclarationOnly.xksl"}},{} },
    //{ "", {{"streamsSimple.xksl"}},{} },
    //{ "", {{"streamsWithClassAccessor.xksl"}},{} },
    //{ "", {{"shaderWithDefinedConsts.xksl"}},{} },
    //{ "", {{"shaderWithUnresolvedConsts.xksl"}},{} },
    //{ "", {{"intrisicsHlslFunctions.xksl"}},{} },
    //{ "", {{"methodReferingToShaderVariable.xksl"}},{} },
    //{ "", {{"methodsWithSimpleClassAccessor.xksl"}},{} },
    //{ "", {{"cbuffers.xksl"}},{} },

    //{ "", {{"TestMixin01_Base.xksl"}}, {{"main", ShadingStageEnum::Pixel, "TestMixin01_Base_Pixel.rv.glsl"}} },
    //{ "", {{"TestMixin01_Override.xksl"}}, {{"main", ShadingStageEnum::Pixel, "TestMixin01_Override_Pixel.rv.glsl"}} },
    //{ "", {{"TestMixin01_OverridePlusCallBase.xksl"}}, {{"main", ShadingStageEnum::Pixel, "TestMixin01_OverridePlusCallBase_Pixel.rv.glsl"}} },
    //{ "", {{"TestMixin01_OverridePlusCallBase2.xksl"}}, {{"main", ShadingStageEnum::Pixel, "TestMixin01_OverridePlusCallBase2_Pixel.rv.glsl"}} },
    //{ "", {{"TestMixin01_OverridePlusCallBase3.xksl"}}, {{"main", ShadingStageEnum::Pixel, "TestMixin01_OverridePlusCallBase3_Pixel.rv.glsl"}} },

    //{ "TestMerge01", {{"TestMerge01_Base.xksl"}, {"TestMerge01_ShaderA.xksl"}}, {{"main", ShadingStageEnum::Pixel, "TestMerge01_Pixel.rv.glsl" } } },
    //{ "TestMerge02", {{"TestMerge02_Base.xksl"}, {"TestMerge02_ShaderA.xksl"}}, {{"main", ShadingStageEnum::Pixel, "TestMerge02_Pixel.rv.glsl"}} },
    //{ "TestMerge03", {{"TestMerge03_Base.xksl"}, {"TestMerge03_ShaderA.xksl"}}, {{"main", ShadingStageEnum::Pixel, "TestMerge03_Pixel.rv.glsl"}} },
    //{ "TestMerge04", {{"TestMerge04_Base.xksl"}, {"TestMerge04_ShaderA.xksl"}}, {{"main", ShadingStageEnum::Pixel, "TestMerge04_Pixel.rv.glsl"}} },
    //{ "TestMerge05", {{"TestMerge05_Base.xksl"}, {"TestMerge05_ShaderA.xksl"}}, {{"main", ShadingStageEnum::Pixel, "TestMerge05_Pixel.rv.glsl"}} },
    //{ "TestMerge06", {{"TestMerge06_Base.xksl"}, {"TestMerge06_ShaderA.xksl"}}, {{"main", ShadingStageEnum::Pixel, "TestMerge06_Pixel.rv.glsl"}} },
    //{ "TestMerge07", {{"TestMerge07_Base.xksl"}, {"TestMerge07_ShaderA.xksl"}}, {{"main", ShadingStageEnum::Pixel, "TestMerge07_Pixel.rv.glsl"}} },
    //{ "TestMerge08", {{"TestMerge08_Base.xksl"}, {"TestMerge08_ShaderA.xksl"}}, {{"main", ShadingStageEnum::Pixel, "TestMerge08_Pixel.rv.glsl"}} },
    //{ "TestMerge09", {{"TestMerge09_ShaderA.xksl"}, {"TestMerge09_ShaderB.xksl" }}, {{"main", ShadingStageEnum::Pixel, "TestMerge09_Pixel.rv.glsl"}} },
    //{ "TestMerge10", {{"TestMerge10_ShaderA.xksl"}, {"TestMerge10_ShaderB.xksl" }}, {{"main", ShadingStageEnum::Pixel, "TestMerge10_Pixel.rv.glsl"}} },
    //{ "TestMerge11", {{"TestMerge11_Base.xksl"}, {"TestMerge11_ShaderA.xksl"}, {"TestMerge11_ShaderB.xksl" }}, {{"main", ShadingStageEnum::Pixel, "TestMerge11_Pixel.rv.glsl"}} },
    //{ "TestMerge12", {{"TestMerge12_B1.xksl"}, {"TestMerge12_B2.xksl"}, {"TestMerge12_C.xksl" }}, {{"main", ShadingStageEnum::Pixel, "TestMerge12_Pixel.rv.glsl"}} },

    //{ "TestCompose01", {{"TestComposeDeclaration.xksl"}},{} },
    //{ "TestCompose02",{ { "TestCompose02.xksl" } }, {{"main", ShadingStageEnum::Pixel, ""}} },
        //{ "",{ { "TestComposeShaderWithParents.xksl" } },{} },
        //{ "",{ { "TestCompose.xksl" } },{} },

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
    inputDir = dir + inputDir;

    outputDir = inputDir + string("outputs\\");
    expectedOutputDir = inputDir + string("expectedOutputs\\");
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
    vector<string> listFailedTest;

    //Parse the shaders using XkslParser library
    {
        for (int n = 0; n < vecXkslFilesToConvert.size(); ++n)
        {
            XkslFilesToParseAndConvert& xkslFilesToParseAndConvert = vecXkslFilesToConvert[n];
            vector<XkslInput>& inputs = xkslFilesToParseAndConvert.inputs;
            vector<MixinOutput>& outputs = xkslFilesToParseAndConvert.outputs;
            if (inputs.size() == 0) continue;

            string effectName = xkslFilesToParseAndConvert.effectName;
            if (effectName.size() == 0){
                effectName = inputs[0].fileName;
                effectName = Utils::RemoveSuffix(effectName);
            }
            countTestProcessed++;
            bool success = true;

            vector<SpxBytecode> listInputBytecodes;

            for (int i=0; i<inputs.size(); ++i)
            {
                //======================================================================================================
                //======================================================================================================
                // load shader file into a string
                string xkslShaderInputFile = inputs[i].fileName;

                cout << "Parsing XKSL shader \"" << xkslShaderInputFile << "\"" << endl;

                const string inputFname = inputDir + "\\" + xkslShaderInputFile;
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
                    success = parser.ConvertXkslToSpirX(xkslShaderInputFile, xkslInput, spirXBytecode, &errorAndDebugMessages, &outputHumanReadableASTAndSPV);
                    time_after = GetTickCount();

                    if (!success) {
                        cout << " Failed to parse the shader" << endl;
                        cout << errorAndDebugMessages.str() << endl;
                        break;
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
                                const string newOutputFname = outputDir + xkslShaderInputFile + ".spv";
                                glslang::OutputSpvBin(bytecodeList, newOutputFname.c_str());
                            }

                            //output the AST and debug and HR spirv binary
                            const string newOutputFname = outputDir + xkslShaderInputFile + ".hr.spv";
                            errorAndDebugMessages << outputHumanReadableASTAndSPV.str();
                            xkslangtest::Utils::WriteFile(newOutputFname, errorAndDebugMessages.str());
                        }
                    }

                    listInputBytecodes.push_back(spirXBytecode);
                }
            }

            //======================================================================================================
            //======================================================================================================
            // Mixin all inputs
            if (success)
            {
                cout << "Mixin SPIRX shaders" << endl;

                XkslMixer mixer;
                vector<string> errorMsgs;
                DWORD mixinTotalTime = 0;
                for (int i=0; i<listInputBytecodes.size(); ++i)
                {
                    const SpxBytecode& spirXBytecode = listInputBytecodes[i];
                    
                    cout << " Mixin " << spirXBytecode.GetName() << ": ";

                    time_before = GetTickCount();
                    success = mixer.Mixin(spirXBytecode, errorMsgs);    //Add mixin files
                    time_after = GetTickCount();
                    mixinTotalTime += (time_after - time_before);

                    {
                        //Save the mixin resulting SPIRX bytecode (HR form), whether it was a success or not
                        SpxBytecode mixinBytecode;
                        bool canGetBytecode = mixer.GetCurrentMixinBytecode(mixinBytecode, errorMsgs);
                        if (!canGetBytecode) {
                            cout << " Failed to get the mixin bytecode" << endl;
                        }
                        else
                        {
                            const vector<uint32_t>& bytecodeList = mixinBytecode.getBytecodeStream();
                            ostringstream disassembly_stream;
                            spv::Parameterize();
                            spv::Disassemble(disassembly_stream, bytecodeList);

                            const string newOutputFname = outputDir + effectName + "_mixin_" + to_string(i) + "_" + spirXBytecode.GetName() + ".hr.spv";
                            xkslangtest::Utils::WriteFile(newOutputFname, disassembly_stream.str());
                        }
                    }

                    if (success)
                    {
                        cout << " OK. Time:  " << (time_after - time_before) << " ms" << endl;
                    }
                    else
                    {
                        cout << "Mixin failed !!" << endl;
                        break;
                    }
                }
                
                //compile the mixin
                if (success && outputs.size() > 0)
                {
                    SpvBytecode compiledSpv;
                    vector<XkslMixer::XkslMixerOutputStage> outputStages;
                    for (int i = 0; i<outputs.size(); ++i)
                        outputStages.push_back(XkslMixer::XkslMixerOutputStage(outputs[i].stage, outputs[i].entryPoint));

                    cout << " Compile Mixin: ";
                    time_before = GetTickCount();
                    success = mixer.Compile(outputStages, compiledSpv, errorMsgs);
                    time_after = GetTickCount();

                    mixinTotalTime += (time_after - time_before);
                    if (success){
                        cout << "OK. time: " << (time_after - time_before) << " ms" << endl;
                        cout << "Mixin done. total time: " << mixinTotalTime << " ms" << endl;
                    }
                    else cout << "Compilation Failed" << endl;

                    if (success)
                    {
                        //output compiled spirv
                        const vector<uint32_t>& bytecodeList = compiledSpv.getBytecodeStream();
                        ostringstream disassembly_stream;
                        spv::Parameterize();
                        spv::Disassemble(disassembly_stream, bytecodeList);

                        const string newOutputFname = outputDir + effectName + "_mixin_compiled_" + ".hr.spv";
                        xkslangtest::Utils::WriteFile(newOutputFname, disassembly_stream.str());

                        //convert and output every stages
                        for (int i = 0; i<outputStages.size(); ++i)
                        {
                            string labelStage = GetShadingStageLabel(outputStages[i].stage);
                            cout << "Convert SPIRV bytecode for entry point=\"" << outputStages[i].entryPoint << "\" stage=\"" << labelStage << "\"" << endl;

                            ///Save the SPIRV bytecode (and its HR form)
                            //output the binary
                            const vector<uint32_t>& bytecodeList = outputStages[i].resultingBytecode.getBytecodeStream();
                            string outputNameSpv = outputDir + effectName + "_" + labelStage + ".spv";
                            glslang::OutputSpvBin(bytecodeList, outputNameSpv.c_str());

                            // dissassemble the binary
                            ostringstream disassembly_stream;
                            spv::Parameterize();
                            spv::Disassemble(disassembly_stream, bytecodeList);

                            string outputNameHrSpv = outputDir + effectName + "_" + labelStage + ".hr.spv";
                            xkslangtest::Utils::WriteFile(outputNameHrSpv, disassembly_stream.str());

                            //======================================================================================================
                            //convert back the SPIRV bytecode into GLSL
                            {
                                string outputNameGlsl = outputDir + effectName + "_" + labelStage + ".rv.glsl";

                                cout << "Convert into GLSL." << endl;

                                time_before = GetTickCount();
                                int result = ConvertSpvToGlsl(outputNameSpv, outputNameGlsl);
                                time_after = GetTickCount();

                                if (success) cout << " OK. time: " << (time_after - time_before) << " ms" << endl;
                                else cout << " Fail to convert the SPIRV file to GLSL" << endl;

                                //======================================================================================================
                                //compare the glsl output with the expected output
                                const char* expectedGlslOutput = outputs[i].expectedGlslOutput;
                                if (success && expectedGlslOutput != nullptr && strlen(expectedGlslOutput) > 0)
                                {
                                    success = false;
                                    string expectedOutputFileName = expectedOutputDir + string(expectedGlslOutput);
                                    string glslExpectedOutput;
                                    if (Utils::ReadFile(expectedOutputFileName, glslExpectedOutput))
                                    {
                                        string glslConvertedOutput;
                                        if (Utils::ReadFile(outputNameGlsl, glslConvertedOutput))
                                        {
                                            if (glslExpectedOutput.compare(glslConvertedOutput) != 0){
                                                cout << "expected output:" << endl << glslExpectedOutput;
                                                cout << "output:" << endl << glslConvertedOutput;
                                                cout << " Glsl output and expected output are different !!!" << endl;
                                            }
                                            else{
                                                cout << " GLSL output VS expected output: OK" << endl;
                                                success = true;
                                            }
                                        }
                                        else{
                                            cout << " Failed to read the file: " << outputNameGlsl << " !!!" << endl;
                                        }
                                    }
                                    else{
                                        cout << " Failed to read the file: " << expectedOutputFileName << " !!!" << endl;
                                    }
                                }
                                //======================================================================================================
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
            else listFailedTest.push_back(effectName);

            cout << endl;
        }

        cout << "Count tests processed: " << countTestProcessed << endl;
        cout << "Count tests successful: " << countTestSuccessful << endl;
        if (listFailedTest.size() > 0)
        {
            cout << endl;
            cout << "<======================  Failed tests ======================>" << endl;
            for (int i=0; i<listFailedTest.size(); ++i) cout << listFailedTest[i] << endl;
            cout << endl;
        }
    }

    parser.Finalize();

}

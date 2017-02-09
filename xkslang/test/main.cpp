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
    //{ "", {{"shaderOnly.xksl"}}, {} },
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

    //{ "TestMixin01", {{"TestMixin01_Base.xksl"}}, {{"main", ShadingStageEnum::Pixel}} },
    //{ "TestMixin02", {{"TestMixin02_Base.xksl"}}, {{"main", ShadingStageEnum::Pixel}} },
    //{ "TestMixin03", {{"TestMixin03_Base.xksl"}}, {{"main", ShadingStageEnum::Pixel}} },
    //{ "TestMixin04", {{"TestMixin04_Base.xksl"}}, {{"main", ShadingStageEnum::Pixel}} },
    //{ "TestMixin05", {{"TestMixin05_Base.xksl"}}, {{"main", ShadingStageEnum::Pixel}} },

    //{ "TestMerge01", {{"TestMerge01_Base.xksl"}, {"TestMerge01_ShaderA.xksl"}}, {{"main", ShadingStageEnum::Pixel}} },
    //{ "TestMerge02", {{"TestMerge02_Base.xksl"}, {"TestMerge02_ShaderA.xksl"}}, {{"main", ShadingStageEnum::Pixel}} },
    //{ "TestMerge03", {{"TestMerge03_Base.xksl"}, {"TestMerge03_ShaderA.xksl"}}, {{"main", ShadingStageEnum::Pixel}} },
    //{ "TestMerge04", {{"TestMerge04_Base.xksl"}, {"TestMerge04_ShaderA.xksl"}}, {{"main", ShadingStageEnum::Pixel}} },
    //{ "TestMerge05", {{"TestMerge05_Base.xksl"}, {"TestMerge05_ShaderA.xksl"}}, {{"main", ShadingStageEnum::Pixel}} },
    //{ "TestMerge06", {{"TestMerge06_Base.xksl"}, {"TestMerge06_ShaderA.xksl"}}, {{"main", ShadingStageEnum::Pixel}} },
    //{ "TestMerge07", {{"TestMerge07_Base.xksl"}, {"TestMerge07_ShaderA.xksl"}}, {{"main", ShadingStageEnum::Pixel}} },
    //{ "TestMerge08", {{"TestMerge08_Base.xksl"}, {"TestMerge08_ShaderA.xksl"}}, {{"main", ShadingStageEnum::Pixel}} },
    //{ "TestMerge09", {{"TestMerge09_ShaderA.xksl"}, {"TestMerge09_ShaderB.xksl" }}, {{"main", ShadingStageEnum::Pixel}} },
    //{ "TestMerge10", {{"TestMerge10_ShaderA.xksl"}, {"TestMerge10_ShaderB.xksl" }}, {{"main", ShadingStageEnum::Pixel}} },
    //{ "TestMerge11", {{"TestMerge11_Base.xksl"}, {"TestMerge11_ShaderA.xksl"}, {"TestMerge11_ShaderB.xksl" }}, {{"main", ShadingStageEnum::Pixel}} },
    //{ "TestMerge12", {{"TestMerge12_B1.xksl"}, {"TestMerge12_B2.xksl"}, {"TestMerge12_C.xksl" }}, {{"main", ShadingStageEnum::Pixel}} },

    //{ "TestCompose01", {{"TestCompose01.xksl"}},{} },

    //{{"textureAndSampler.xksl"}, {"", nullptr}},
    //{{"shaderTexturing.xksl"}, {"", nullptr}},
    //{{"shaderBase.xksl"}, {"", nullptr}},
    //{{"shaderSimple.xksl"}, {"", nullptr}},

    //{{"shaderCustomEffect.xksl"}, {"", nullptr}},
    //{{"methodsPrototypes.xksl"}, {"", nullptr}},
};

struct XkfxEffectsToProcess {
    string effectName;
    string input;
};
vector<XkfxEffectsToProcess> vecXkfxEffectToProcess = {
    //{ "TestMixin01", "TestMixin01.xkfx" },
    //{ "TestMixin02", "TestMixin02.xkfx" },
    //{ "TestMixin03", "TestMixin03.xkfx" },
    //{ "TestMixin04", "TestMixin04.xkfx" },
    //{ "TestMixin05", "TestMixin05.xkfx" },

    //{ "TestMerge01", "TestMerge01.xkfx" },
    //{ "TestMerge02", "TestMerge02.xkfx" },
    //{ "TestMerge03b", "TestMerge03.xkfx" },
    //{ "TestMerge04", "TestMerge04.xkfx" },
    //{ "TestMerge05", "TestMerge05.xkfx" },
    //{ "TestMerge06", "TestMerge06.xkfx" },
  
    { "TestMerge07B", "TestMerge07.xkfx" },
    //{ "TestMerge08", "TestMerge08.xkfx" },
    //{ "TestMerge09", "TestMerge09.xkfx" },
    //{ "TestMerge10", "TestMerge10.xkfx" },
    //{ "TestMerge11", "TestMerge11.xkfx" },
    //{ "TestMerge12", "TestMerge12.xkfx" },

    //{ "TestCompose02", "TestCompose02.xkfx" },
    //{ "TestCompose02", {{"TestCompose02.xksl"}}, {{"main", ShadingStageEnum::Pixel}} },
    //{ "TestCompose03", {{"TestCompose03.xksl"}}, {{"main", ShadingStageEnum::Pixel}} },
    //{ "TestCompose04", {{"TestCompose04.xksl"}}, {{"main", ShadingStageEnum::Pixel}} },
    //{ "TestCompose05", {{"TestCompose05.xksl"}}, {{"main", ShadingStageEnum::Pixel}} },
    //{ "TestCompose06", {{"TestCompose06.xksl"}}, {{"main", ShadingStageEnum::Pixel}} },
    //{ "TestCompose07", {{"TestCompose07.xksl"}}, {{"main", ShadingStageEnum::Pixel}} },
    //{ "TestCompose08", {{"TestCompose08.xksl"}}, {{"main", ShadingStageEnum::Pixel}} },
    //{ "TestCompose09", {{"TestCompose09.xksl"}}, {{"main", ShadingStageEnum::Pixel}} },
};

#ifdef _DEBUG
static string spirvCrossExe = "D:/Prgms/glslang/source/bin/spirv-cross/SPIRV-Cross_d.exe";
//static string spirvCrossExe = "D:/Prgms/glslang/source/bin/spirv-cross/SPIRV-Cross_d.exe --hlsl";
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
        int error = GetLastError();
        printf("CreateProcess failed (%d).\n", error);
        return error;
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

bool CompileMixer(string effectName, XkslMixer* mixer, vector<XkslMixer::XkslMixerOutputStage>& outputStages, vector<string>& errorMsgs)
{
    DWORD time_before, time_after;
    bool success = true;

    SpvBytecode compiledSpv;
    SpvBytecode finalizedSpv;
    SpvBytecode errorSpv;
    cout << "Compile Mixin: ";
    time_before = GetTickCount();
    success = mixer->Compile(outputStages, errorMsgs, &compiledSpv, &finalizedSpv, &errorSpv);
    time_after = GetTickCount();

    if (!success)
    {
        cout << "Compilation Failed" << endl;

        //Try to output the latest SPV bytecode
        const vector<uint32_t>& bytecodeList = errorSpv.getBytecodeStream();
        if (bytecodeList.size() > 0)
        {
            ostringstream disassembly_stream;
            spv::Parameterize();
            spv::Disassemble(disassembly_stream, bytecodeList);

            const string outputFileName = effectName + "_mixin_error_" + ".hr.spv";
            const string outputFileFullName = outputDir + outputFileName;
            xkslangtest::Utils::WriteFile(outputFileFullName, disassembly_stream.str());
            cout << " output: \"" << outputFileName << "\"" << endl;
        }
    }
    else
    {
        cout << "OK. time: " << (time_after - time_before) << " ms" << endl;
    }

    //output compiled spirv whtether compilation succeded or not
    {
        const vector<uint32_t>& bytecodeList = compiledSpv.getBytecodeStream();
        ostringstream disassembly_stream;
        spv::Parameterize();
        spv::Disassemble(disassembly_stream, bytecodeList);

        const string outputFileName = effectName + "_mixin_compiled_" + ".hr.spv";
        const string outputFullName = outputDir + outputFileName;
        xkslangtest::Utils::WriteFile(outputFullName, disassembly_stream.str());
        cout << " output: \"" << outputFileName << "\"" << endl;
    }

    //output finalized spirv
    {
        const vector<uint32_t>& bytecodeList = finalizedSpv.getBytecodeStream();
        ostringstream disassembly_stream;
        spv::Parameterize();
        spv::Disassemble(disassembly_stream, bytecodeList);

        const string outputFileName = effectName + "_mixin_finalized_" + ".hr.spv";
        const string outputFullName = outputDir + outputFileName;
        xkslangtest::Utils::WriteFile(outputFullName, disassembly_stream.str());
        cout << " output: \"" << outputFileName << "\"" << endl;
    }

    if (!success) return false;

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

        {
            string outputFileName = effectName + "_" + labelStage + ".hr.spv";
            string outputFullName = outputDir + outputFileName;
            xkslangtest::Utils::WriteFile(outputFullName, disassembly_stream.str());
            cout << " output: \"" << outputFileName << "\"" << endl;
        }

        //======================================================================================================
        //convert back the SPIRV bytecode into GLSL
        {
            string fileNameGlsl = effectName + "_" + labelStage + ".rv.glsl";
            string fullNameGlsl = outputDir + fileNameGlsl;

            cout << "Convert into GLSL." << endl;

            time_before = GetTickCount();
            int result = ConvertSpvToGlsl(outputNameSpv, fullNameGlsl);
            time_after = GetTickCount();
            if (result != 0) success = false;

            if (success) cout << " OK. time: " << (time_after - time_before) << " ms" << endl;
            else cout << " Failed to convert the SPIRV file to GLSL" << endl;

            //======================================================================================================
            //compare the glsl output with the expected output
            if (success)
            {
                string expectedOutputFullNameGlsl = expectedOutputDir + string(fileNameGlsl);
                string glslExpectedOutput;
                if (Utils::ReadFile(expectedOutputFullNameGlsl, glslExpectedOutput))
                {
                    string glslConvertedOutput;
                    if (Utils::ReadFile(fullNameGlsl, glslConvertedOutput))
                    {
                        if (glslExpectedOutput.compare(glslConvertedOutput) != 0) {
                            cout << "expected output:" << endl << glslExpectedOutput;
                            cout << "output:" << endl << glslConvertedOutput;
                            cout << " Glsl output and expected output are different !!!" << endl;
                            success = false;
                        }
                        else {
                            cout << " GLSL output VS expected output: OK" << endl;
                        }
                    }
                    else {
                        cout << " Failed to read the file: " << fileNameGlsl << endl;
                        success = false;
                    }
                }
                else {
                    cout << "Warning: No expected output file for: " << fileNameGlsl << endl;
                }
            }
            //======================================================================================================
        }
    }
    
    return success;
}

bool ParseAndConvertXkslFile(XkslParser* parser, string& xkslInputFile, SpxBytecode& spirXBytecode, bool writeOutputsOnDisk)
{
    cout << "Parsing XKSL shader \"" << xkslInputFile << "\"" << endl;

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
    ostringstream outputHumanReadableASTAndSPV;

    DWORD time_before, time_after;
    time_before = GetTickCount();
    bool success = parser->ConvertXkslToSpirX(xkslInputFile, xkslInput, spirXBytecode, &errorAndDebugMessages, &outputHumanReadableASTAndSPV);
    time_after = GetTickCount();

    if (!success) {
        cout << " Failed to parse the xksl file" << endl;
        cout << errorAndDebugMessages.str() << endl;
        return false;
    }
    else
    {
        cout << " OK. time: " << (time_after - time_before) << " ms" << endl;

        //output binary and debug info
        if (writeOutputsOnDisk)
        {
            //output the SPIRX binary
            const vector<uint32_t>& bytecodeList = spirXBytecode.getBytecodeStream();
            if (bytecodeList.size() > 0)
            {
                const string newOutputFname = outputDir + xkslInputFile + ".spv";
                glslang::OutputSpvBin(bytecodeList, newOutputFname.c_str());
            }

            //output the AST and debug and HR spirv binary
            const string outputFileName = xkslInputFile + ".hr.spv";
            const string outputFullName = outputDir + outputFileName;
            errorAndDebugMessages << outputHumanReadableASTAndSPV.str();
            xkslangtest::Utils::WriteFile(outputFullName, errorAndDebugMessages.str());
            cout << " output: \"" << outputFileName << "\"" << endl;
        }
    }
}

bool SeparateNameAndInstuction(const string str, string& name, string& instruction)
{
    size_t pdot = str.find_first_of('.');
    if (pdot == string::npos) return false;
    name = str.substr(0, pdot);
    instruction = str.substr(pdot + 1);
}

bool GetShadingStageForString(string& str, ShadingStageEnum& stage)
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
    XkslMixer* mixer;
    unordered_map<int, string> stagesEntryPoints;

    EffectMixerObject(XkslMixer* mixer): mixer(mixer){}
    ~EffectMixerObject() {delete mixer;}
};

bool ProcessEffect(XkslParser* parser, XkfxEffectsToProcess& effect)
{
    string effectName = effect.effectName;

    bool success = true;
    const string inputFname = inputDir + "\\" + effect.input;
    string effectCmdLines;
    if (!Utils::ReadFile(inputFname, effectCmdLines))
    {
        cout << " Failed to read the file: " << inputFname << " !!!" << endl;
        return false;
    }

    vector<string> errorMsgs;
    DWORD time_before, time_after;
    SpxBytecode spirxShaderLibrary;
    unordered_map<string, EffectMixerObject*> mixerMap;
    int mixinNum = 0;

    string line, lineItem;
    stringstream ss(effectCmdLines);
    while (getline(ss, line, '\n'))
    {
        line = Utils::trim(line);
        if (line.size() == 0) continue;

        stringstream lineSs(line);
        getline(lineSs, lineItem, ' ');
        if (lineItem.compare("load") == 0)
        {
            //convert and load xksl shaders
            string xkslInputFile;
            if (!getline(lineSs, xkslInputFile, ' ')) {
                cout << "load: failed to get the xksl file name" << endl;
                success = false; break;
            }
            xkslInputFile = Utils::trim(xkslInputFile, '\"');

            success = ParseAndConvertXkslFile(parser, xkslInputFile, spirxShaderLibrary, true);
            if (!success) {
                cout << "load: failed to convert the xksl file name" << endl;
                success = false; break;
            }
        }
        else if (lineItem.compare("mixer") == 0)
        {
            string mixerName;
            if (!getline(lineSs, mixerName, ' ')) {
                cout << "load: failed to get the xksl file name" << endl;
                success = false; break;
            }
            mixerName = Utils::trim(mixerName, '\"');

            if (mixerMap.find(mixerName) != mixerMap.end()) {
                cout << "mixer: a mixer already exists with the name:" << mixerName << endl;
                success = false; break;
            }

            XkslMixer* mixer = new XkslMixer();
            mixerMap[mixerName] = new EffectMixerObject(mixer);
        }
        else if (lineItem.size() >= 2 && lineItem[0] == '/' && lineItem[1] == '/')
        {
            //comment: ignore the line
        }
        else
        {
            string mixerName, instruction;
            if (!SeparateNameAndInstuction(lineItem, mixerName, instruction)) {
                cout << "Unknown instruction: " << lineItem << endl;
                success = false; break;
            }

            if (mixerMap.find(mixerName) == mixerMap.end()) {
                cout << lineItem << ": no mixer found with the name:" << mixerName << endl;
                success = false; break;
            }
            EffectMixerObject* mixerObj = mixerMap[mixerName];

            if (instruction.compare("mixin") == 0)
            {
                vector<string> listShaderToMix;
                string shaderName;
                while (getline(lineSs, shaderName, ' ')) {
                    listShaderToMix.push_back(shaderName);
                }

                cout << "mixin: " << line << endl;
                time_before = GetTickCount();
                success = mixerObj->mixer->Mixin(spirxShaderLibrary, listShaderToMix, errorMsgs);
                time_after = GetTickCount();

                {
                    //Save the mixin resulting SPIRX bytecode (HR form), whether it was a success or not
                    SpxBytecode mixinBytecode;
                    bool canGetBytecode = mixerObj->mixer->GetCurrentMixinBytecode(mixinBytecode, errorMsgs);
                    if (!canGetBytecode) {
                        cout << " Failed to get the mixin bytecode" << endl;
                    }
                    else
                    {
                        const vector<uint32_t>& bytecodeList = mixinBytecode.getBytecodeStream();
                        ostringstream disassembly_stream;
                        spv::Parameterize();
                        spv::Disassemble(disassembly_stream, bytecodeList);

                        const string outputFileName = effectName + "_mixin_" + to_string(mixinNum++) + ".hr.spv";
                        const string outputFullName = outputDir + outputFileName;
                        xkslangtest::Utils::WriteFile(outputFullName, disassembly_stream.str());
                        cout << " output: \"" << outputFileName << "\"" << endl;
                    }
                }

                if (success)
                    cout << " OK. Time:  " << (time_after - time_before) << " ms" << endl;
                else
                    cout << "Mixin failed" << endl;
            }
            else if (instruction.compare("addComposition") == 0)
            {
                cout << "Not implemented yet: " << instruction << endl;
                success = false; break;
            }
            else if (instruction.compare("setStageEntryPoint") == 0)
            {
                string stageStr;
                if (!getline(lineSs, stageStr, ' ')) {
                    cout << "Expecting stage" << endl;
                    success = false; break;
                }
                ShadingStageEnum stage;
                if (!GetShadingStageForString(stageStr, stage)) {
                    cout << "Unknown stage:" << stageStr;
                    success = false; break;
                }

                string entryPoint;
                if (!getline(lineSs, entryPoint, ' ')) {
                    mixerObj->stagesEntryPoints[(int)stage] = "";
                }
                else
                {
                    entryPoint = Utils::trim(entryPoint, '\"');
                    mixerObj->stagesEntryPoints[(int)stage] = entryPoint;
                }
            }
            else if (instruction.compare("compile") == 0)
            {
                vector<XkslMixer::XkslMixerOutputStage> outputStages;
                for (auto its = mixerObj->stagesEntryPoints.begin(); its != mixerObj->stagesEntryPoints.end(); its++){
                    if (its->second.size() > 0)
                        outputStages.push_back(XkslMixer::XkslMixerOutputStage(ShadingStageEnum(its->first), its->second));
                }

                success = CompileMixer(effectName, mixerObj->mixer, outputStages, errorMsgs);
                if (!success)
                {
                    cout << "Failed to compile the mixer: " << effectName << endl;
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

    if (errorMsgs.size() > 0)
    {
        cout << "   Messages:" << endl;
        for (int m = 0; m<errorMsgs.size(); m++) cout << "   " << errorMsgs[m] << "" << endl;
    }

    return success;
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

    cout << "___________________________________________________________________________________" << endl;
    cout << "Process XKSL Files:" << endl << endl;
    //Parse the shaders using XkslParser library
    {
        int countTestProcessed = 0;
        int countTestSuccessful = 0;
        vector<string> listFailedTest;
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
                // parse and convert all xksl files
                string xkslShaderInputFile = inputs[i].fileName;
                SpxBytecode spirXBytecode;
                bool success = ParseAndConvertXkslFile(&parser, xkslShaderInputFile, spirXBytecode, true);
                if (!success) break;

                listInputBytecodes.push_back(spirXBytecode);
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
                    
                    cout << " Mixin " << spirXBytecode.GetName() << ": " << endl;

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

                            const string outputFileName = effectName + "_mixin_" + to_string(i) + "_" + spirXBytecode.GetName() + ".hr.spv";
                            const string outputFullName = outputDir + outputFileName;
                            xkslangtest::Utils::WriteFile(outputFullName, disassembly_stream.str());
                            cout << " output: \"" << outputFileName << "\"" << endl;
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
                    vector<XkslMixer::XkslMixerOutputStage> outputStages;
                    for (int i = 0; i<outputs.size(); ++i)
                        outputStages.push_back(XkslMixer::XkslMixerOutputStage(outputs[i].stage, outputs[i].entryPoint));

                    success = CompileMixer(effectName, &mixer, outputStages, errorMsgs);
                    if (!success)
                    {
                        cout << "Failed to compile the mixer: " << effectName << endl;
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

    cout << endl;
    cout << "___________________________________________________________________________________" << endl;
    cout << "Process XKFX Effect  Files:" << endl << endl;
    //Parse the effects
    {
        int countTestProcessed = 0;
        int countTestSuccessful = 0;
        vector<string> listFailedTest;
        for (int n = 0; n < vecXkfxEffectToProcess.size(); ++n)
        {
            XkfxEffectsToProcess effect = vecXkfxEffectToProcess[n];
            countTestProcessed++;
            bool success = ProcessEffect(&parser, effect);

            if (success) countTestSuccessful++;
            else listFailedTest.push_back(effect.effectName);

            cout << endl;
        }

        cout << "Count tests processed: " << countTestProcessed << endl;
        cout << "Count tests successful: " << countTestSuccessful << endl;
        if (listFailedTest.size() > 0)
        {
            cout << endl;
            cout << "<======================  Failed tests ======================>" << endl;
            for (int i = 0; i<listFailedTest.size(); ++i) cout << listFailedTest[i] << endl;
            cout << endl;
        }
    }

    parser.Finalize();

}

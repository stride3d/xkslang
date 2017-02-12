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

//To test single file parsing and convertion
struct XkslFilesToParseAndConvert {
    const char* fileName;
};

static string inputDir = "glslang\\source\\Test\\xksl\\";
static string outputDir;
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
    //{ "methodReferingToShaderVariable.xksl" },
    //{ "methodsWithSimpleClassAccessor.xksl" },
    //{ "cbuffers.xksl" },
    //{ "TestCompose01.xksl" },

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

    //{ "TestCompose02", "TestCompose02.xkfx" },
    //{ "TestCompose03", "TestCompose03.xkfx" },
    //{ "TestCompose04", "TestCompose04.xkfx" },
    //{ "TestCompose05", "TestCompose05.xkfx" },
    //{ "TestCompose06", "TestCompose06.xkfx" },
    //{ "TestCompose07", "TestCompose07.xkfx" },
    { "TestCompose08", "TestCompose08.xkfx" },
    //{ "TestCompose09", "TestCompose09.xkfx" },
    //{ "TestCompose10", "TestCompose10.xkfx" },
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

void SaveCurrentMixerBytecode(XkslMixer* mixer, string outputDirPath, string outputFileName)
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

bool CompileMixer(string effectName, XkslMixer* mixer, vector<XkslMixerOutputStage>& outputStages, vector<string>& errorMsgs)
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
        cout << "Convert SPIRV bytecode for entry point=\"" << outputStages[i].entryPointName << "\" stage=\"" << labelStage << "\"" << endl;

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

bool SeparateAdotB(const string str, string& A, string& B)
{
    size_t pdot = str.find_first_of('.');
    if (pdot == string::npos) return false;
    A = str.substr(0, pdot);
    B = str.substr(pdot + 1);
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
    vector<SpxBytecode*> listAllParsedBytecode;
    unordered_map<string, SpxBytecode*> mapShaderWithBytecode;
    unordered_map<string, EffectMixerObject*> mixerMap;
    int mixinNum = 0;
    int composeNum = 0;

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

            SpxBytecode* spxBytecode = new SpxBytecode;
            listAllParsedBytecode.push_back(spxBytecode);
            success = ParseAndConvertXkslFile(parser, xkslInputFile, *spxBytecode, true);
            if (!success) {
                cout << "load: failed to convert the xksl file name: " << xkslInputFile << endl;
                success = false; break;
            }
            
            vector<string> vecShaderName;
            if (!XkslMixer::GetListAllShadersFromBytecode(*spxBytecode, vecShaderName, errorMsgs))
            {
                cout << "load: failed to get the list of shader names from: " << xkslInputFile << endl;
                success = false; break;
            }

            for (int is = 0; is < vecShaderName.size(); ++is)
            {
                string shaderName = vecShaderName[is];
                mapShaderWithBytecode[shaderName] = spxBytecode;
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
                while (getline(lineSs, shaderName, ' ')) {
                    listShaderToMix.push_back(shaderName);

                    auto it = mapShaderWithBytecode.find(shaderName);
                    if (it == mapShaderWithBytecode.end())
                    {
                        cout << "No spxBytecode found in the librady for the shader: " << shaderName << endl;
                        success = false; break;
                    }
                    SpxBytecode* aShaderBytecode = it->second;
                    if (spxBytecode == nullptr) spxBytecode = aShaderBytecode;
                    else
                    {
                        if (spxBytecode != aShaderBytecode) {
                            cout << "2 shader to mix are defined in different bytecode (we could merge them)" << endl;
                            success = false; break;
                        }
                    }
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
                const string outputFileName = effectName + "_mixin" + to_string(mixinNum++) + ".hr.spv";
                SaveCurrentMixerBytecode(mixerTarget->mixer, outputDir, outputFileName);

                if (success)
                    cout << " OK. Time:  " << (time_after - time_before) << " ms" << endl;
                else
                    cout << "Mixin failed" << endl;
            }
            else if (instruction.compare("addComposition") == 0)
            {
                string compositionTargetStr;
                if (!getline(lineSs, compositionTargetStr, ' ')) {
                    cout << "Expecting composition target" << endl;
                    success = false; break;
                }

                string shaderName, variableName;
                if (!SeparateAdotB(compositionTargetStr, shaderName, variableName)) {
                    cout << "Unknown instruction: " << compositionTargetStr << endl;
                    success = false; break;
                }

                string mixerSourceName;
                if (!getline(lineSs, mixerSourceName, ' ')) {
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
                const string outputFileName = effectName + "_compose" + to_string(composeNum++) + "_" + compositionTargetStr + ".hr.spv";
                SaveCurrentMixerBytecode(mixerTarget->mixer, outputDir, outputFileName);

                if (success)
                    cout << " OK. Time:  " << (time_after - time_before) << " ms" << endl;
                else
                    cout << "Failed to add the composition to the mixer" << endl;
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
                vector<XkslMixerOutputStage> outputStages;
                for (auto its = mixerTarget->stagesEntryPoints.begin(); its != mixerTarget->stagesEntryPoints.end(); its++){
                    if (its->second.size() > 0)
                        outputStages.push_back(XkslMixerOutputStage(ShadingStageEnum(its->first), its->second));
                }

                success = CompileMixer(effectName, mixerTarget->mixer, outputStages, errorMsgs);
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

    for (auto itv = listAllParsedBytecode.begin(); itv != listAllParsedBytecode.end(); itv++)
        delete (*itv);

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
    cout << "Parse and convert XKSL Files:" << endl << endl;
    //Parse the shaders using XkslParser library
    {
        int countTestProcessed = 0;
        int countTestSuccessful = 0;
        vector<string> listFailedTest;
        for (int n = 0; n < vecXkslFilesToConvert.size(); ++n)
        {
            countTestProcessed++;
            bool success = true;

            XkslFilesToParseAndConvert& xkslFilesToParseAndConvert = vecXkslFilesToConvert[n];
            string xkslShaderInputFile = xkslFilesToParseAndConvert.fileName;
            
            // parse and convert all xksl files
            SpxBytecode spirXBytecode;
            success = ParseAndConvertXkslFile(&parser, xkslShaderInputFile, spirXBytecode, true);
            if (!success) break;

            if (success) countTestSuccessful++;
            else listFailedTest.push_back(xkslShaderInputFile);

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

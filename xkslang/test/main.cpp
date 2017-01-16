//
// Copyright (C)

#include <iostream>
#include <memory>
#include <string>

#include "SPIRV/doc.h"
#include "SPIRV/disassemble.h"

#include "Utils.h"
#include "XkslangTest.h"
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

std::string testDir = "D:/Prgms/glslang/source/Test/xksl";
Source source = Source::HLSL;
Semantics semantics = Semantics::Vulkan;
Target target = Target::BothASTAndSpv;

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
    { "TestMixin01_Override.xksl", "main" },
    //{ "TestMixin01_OverridePlusCallBase.xksl", "" },

    //{"textureAndSampler.xksl", ""},
    //{"shaderTexturing.xksl", ""},
    //{"shaderBase.xksl", ""},
    //{"shaderSimple.xksl", ""},

    //{"shaderCustomEffect.xksl", ""},
    //{"methodsPrototypes.xksl", ""},
};

void main(int argc, char** argv)
{
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
            //============================================================================
            // load shader file into a string
            std::string shaderFileName = testFiles[i].fileName;

            cout << "Parsing shader:" << shaderFileName << "...\n";

            const string inputFname = testDir + "/" + shaderFileName;
            string xkslInput;
            if (!Utils::ReadFile(inputFname, xkslInput))
            {
                cout << "  Failed to read the file: " << inputFname << " !!!\n";
                continue;
            }

            //===============================================================================================
            //===============================================================================================
            // Parse and convert HLSL to SPIRX bytecode
            SpxBytecode spirXBytecode;
            bool success = parser.ConvertXkslToSpirX(shaderFileName, xkslInput, spirXBytecode);
            if (!success)
            {
                cout << "  Failed to parse the shader: " << shaderFileName << " !!!!!!!!!!!!!!!!\n";
                continue;
            }

            cout << "  Parsing successful\n";

            //===============================================================================================
            //===============================================================================================
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

                if (success) cout << "  Mixin successful\n";
                else cout << "  Mixin Failed !!!\n";

                //TMP: Save the SPIRV remapped mixin
                if (success)
                {
                    // dissassemble the binary
                    SpxBytecode mixinBytecode;
                    mixer.GetMixinBytecode(mixinBytecode, errorMsgs);
                    const std::vector<uint32_t>& bytecodeList = mixinBytecode.getBytecodeStream();
                    ostringstream disassembly_stream;
                    spv::Parameterize();
                    spv::Disassemble(disassembly_stream, bytecodeList);

                    const string newOutputFname = testDir + "/" + shaderFileName + "_mixin" + ".hr.spv";
                    xkslangtest::Utils::WriteFile(newOutputFname, disassembly_stream.str());
                }

                //Generate stage SPIRV bytecode
                if (success)
                {
                    SpvBytecode bytecode;
                    ShadingStage stage = ShadingStage::Pixel;

                    cout << "   Generate SPIRV bytecode for entry point:" << entryPoint << " stage:" << GetStageLabel(stage) << "\n";
                    success = mixer.GenerateStageBytecode(stage, entryPoint, bytecode, errorMsgs);

                    if (success) cout << "  Bytecode successfully generated\n";
                    else cout << "  Fail to generate the bytecode !!!\n";

                    //TMP: Save the SPIRV bytecode on the disk
                    if (success)
                    {
                        // dissassemble the binary
                        const std::vector<uint32_t>& bytecodeList = bytecode.getBytecodeStream();
                        ostringstream disassembly_stream;
                        spv::Parameterize();
                        spv::Disassemble(disassembly_stream, bytecodeList);

                        const string newOutputFname = testDir + "/" + shaderFileName + "_" + GetStageLabel(stage) + ".hr.spv";
                        xkslangtest::Utils::WriteFile(newOutputFname, disassembly_stream.str());
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

#if 0
    //Parse the shaders by calling glslang functions
    {
        glslang::InitializeProcess();

        XkslangTest xkslangTest;

        for (int i = 0; i < testFiles.size(); ++i)
        {
            std::string testName = testFiles[i].fileName;
            std::string entryPointName = testFiles[i].entryPoint;

            bool success = xkslangTest.loadFileCompileAndCheck(
                testDir, testName, source, semantics, target, entryPointName
            );

            if (!success)
            {
                cout << testName << ": Error\n";
            }
            else
            {
                cout << testName << ": OK\n";
            }
        }

        glslang::FinalizeProcess();
    }
#endif

}


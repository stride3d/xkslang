//
// Copyright (C)

#include <iostream>
#include <memory>
#include <string>

#include "Utils.h"
#include "XkslangTest.h"
#include "../source/SPXBytecode.h"
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
    { "TestMixin01_Base.xksl", "main" },
    //{ "TestMixin01_Override.xksl", "" },
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

            //============================================================================
            // Build SPIRX bytecode
            SPXBytecode spirXBytecode;
            if (!parser.ConvertXkslToSpirX(shaderFileName, xkslInput, spirXBytecode))
            {
                cout << "  Failed to parse the shader: " << shaderFileName << " !!!\n";
                continue;
            }

            cout << "  Parsing successful\n";

            //============================================================================
            // Mixin
            string entryPoint = testFiles[i].entryPoint;
            if (entryPoint.length() > 0)
            {
                SPVBytecode bytecode;
                ShadingStage stage = ShadingStage::PixelStage;
                vector<string> msgs;
                XkslMixer mixer;
                mixer.AddMixin(&spirXBytecode);

                cout << "Mixin shader:" << shaderFileName << ". entry point=" << entryPoint << " stage=" << GetStageLabel(stage) << "...\n";

                bool success = mixer.GenerateBytecode(bytecode, stage, entryPoint, msgs);

                if (success) cout << "  Mixin successful\n";
                else cout << "  Mixin Failed !!!\n";

                if (msgs.size() > 0)
                {
                    cout << "   Messages:\n";
                    for (int m=0; m<msgs.size(); m++)
                        cout << "   " << msgs[m] << "\n";
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


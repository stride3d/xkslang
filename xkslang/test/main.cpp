//
// Copyright (C)

#include <iostream>
#include <memory>
#include <string>

#include "Utils.h"
#include "XkslangTest.h"
#include "../source/XkslParser.h"

using namespace std;
using namespace xkslangtest;
using namespace xkslparser;

struct FileNameEntryPointPair {
    const char* fileName;
    const char* entryPoint;
};

std::string testDir = "D:/Prgms/glslang/source/Test/xksl";
Source source = Source::HLSL;
Semantics semantics = Semantics::Vulkan;
Target target = Target::BothASTAndSpv;

vector<FileNameEntryPointPair> testFiles = {
    //{"xksl_shaderWithVariable.frag", ""},
    //{"xksl_shaderWithManyVariables.frag", ""},
    //{"xksl_manySimpleShaders.frag", ""},
    //{"xksl_simpleShaderWithFunction.frag", ""},
    //{"xksl_declarationMixOfFunctionsAndVariables.frag", ""},
    //{"xksl_2ShaderWithSameFunctionNames.frag", ""},
    //{"xksl_shaderInheritance.frag", ""},
    //{"xksl_postDeclaration.frag", "" },

    {"xksl_functionReferingToShaderVariable.frag", ""},
    //{ "xksl_classAccessor.frag", "" },

    //{"xksl_constAssignment.frag", ""},
    //{"xksl_unresolvedVariable.frag", ""},
    //{"xksl_functionDeclaration.frag", ""},
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
            //load shader file into a string
            std::string shaderFileName = testFiles[i].fileName;
            const string inputFname = testDir + "/" + shaderFileName;
            string xkslInput;

            if (!Utils::ReadFile(inputFname, xkslInput))
            {
                cout << "Failed to read the file: " << inputFname << "\n";
                continue;
            }

            if (!parser.ParseXkslShader(shaderFileName, xkslInput))
            {
                cout << "Failed to parse the shader: " << shaderFileName << "\n";
                continue;
            }

            cout << "Shader " << shaderFileName << ": Successfully parsed\n";
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

//
// Copyright (C)

#include <iostream>
#include <memory>
#include <string>

#include "XkslangTest.h"

using namespace std;
using namespace xkslangtest;

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
	{"xksl_functionReferingToShaderVariable.frag", ""},
	//{"xksl_shaderInheritance.frag", ""},
	//{"xksl_postDeclaration.frag", ""},
	//{"xksl_constAssignment.frag", ""},
	//{"xksl_unresolvedVariable.frag", ""},
	//{"xksl_classAccessor.frag", "" },
	//{"xksl_functionDeclaration.frag", ""},
};

void main(int argc, char** argv)
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

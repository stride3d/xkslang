//
// Copyright (C) 

#ifndef XKSLANG_TESTS_TEST_FIXTURE_H
#define XKSLANG_TESTS_TEST_FIXTURE_H

#include <cstdint>
#include <fstream>
#include <sstream>
#include <streambuf>
#include <tuple>
#include <string>
#include <vector>

#include "glslang/Public/ShaderLang.h"
#include "StandAlone/ResourceLimits.h"

#include "define.h"

using namespace std;

namespace xkslangtest
{

enum class Source {
  GLSL,
  HLSL,
};

// Enum for shader compilation semantics.
enum class Semantics {
    OpenGL,
    Vulkan,
};

// Enum for compilation target.
enum class Target {
    AST,
    Spv,
    BothASTAndSpv,
};

struct ShaderResult {
	string shaderName;
	string output;
	string error;
};

// A struct for holding all the information returned by glslang compilation and linking.
struct XkslangResult {
	vector<ShaderResult> shaderResults;
	string linkingOutput;
	string linkingError;
	string spirvWarningsErrors;
	string spirv;  // Optional SPIR-V disassembly text.
	bool success;
};

class XkslangTest
{
private:
	const int defaultVersion;
	const EProfile defaultProfile;
	const bool forceVersionProfile;
	const bool isForwardCompatible;

public:
	XkslangTest() :
		defaultVersion(100),
		defaultProfile(ENoProfile),
		forceVersionProfile(false),
		isForwardCompatible(false) {}

	bool loadFileCompileAndCheck(const string& testDir,
		const string& testName,
		Source source,
		Semantics semantics,
		Target target,
		const string& entryPointName = "");

	// Compiles and links the given source |code| of the given shader
	// |stage| into the target under the semantics specified via |controls|.
	// Returns a GlslangResult instance containing all the information generated
	// during the process. If the target includes SPIR-V, also disassembles
	// the result and returns disassembly text.
	XkslangResult compileAndLink(
		const string shaderName, const string& code,
		const string& entryPointName, EShMessages controls,
		bool flattenUniformArrays = false);

	// Compiles and the given source |code| of the given shader |stage| into
	// the target under the semantics conveyed via |controls|. Returns true
	// and modifies |shader| on success.
	bool compile(glslang::TShader* shader, const string& code,
		const string& entryPointName, EShMessages controls,
		const TBuiltInResource* resources = nullptr);

	//========================================================================================
	//Utils
	void outputResultToStream(ostringstream* stream, const XkslangResult& result, EShMessages controls);
	EShMessages DeriveOptions(Source source, Semantics semantics, Target target);
	EShLanguage GetShaderStage(const string& stage);

	//========================================================================================
	//Files IO

	// Reads the content of the file at the given |path|. On success, returns true
	// and the contents; otherwise, returns false and an empty string.
	bool ReadFile(const string& path, string& output);
	pair<bool, vector<uint32_t> > ReadSpvBinaryFile(const string& path);

	// Writes the given |contents| into the file at the given |path|. Returns true on successful output.
	bool WriteFile(const string& path, const string& contents);

	// Returns the suffix of the given |name|.
	string GetSuffix(const string& name);
	//========================================================================================
};

}  // namespace glslangtest

#endif  // XKSLANG_TESTS_TEST_FIXTURE_H

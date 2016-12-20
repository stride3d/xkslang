//
// Copyright (C) 

#include <cassert>

#include "XkslangTest.h"

#include "SPIRV/GlslangToSpv.h"
#include "SPIRV/disassemble.h"
#include "SPIRV/doc.h"
#include "SPIRV/SPVRemapper.h"

using namespace xkslangtest;

bool XkslangTest::loadFileCompileAndCheck(const string& testDir,
	const string& testName,
	Source source,
	Semantics semantics,
	Target target,
	const string& entryPointName)
{
	const string inputFname = testDir + "/" + testName;
	const string expectedOutputFname = testDir + "/baseResults/" + testName + ".out";
	string xkslInput;

	if (!ReadFile(inputFname, xkslInput))
	{
		return false;
	}

	const EShMessages controls = DeriveOptions(source, semantics, target);
	XkslangResult result = compileAndLink(testName, xkslInput, entryPointName, controls);

	// Generate the hybrid output in the way of glslangValidator.
	ostringstream stream;
	outputResultToStream(&stream, result, controls);

	// Write the stream output on the disk
	const string newOutputFname = testDir + "/" + testName + ".latest.spv";
	WriteFile(newOutputFname, stream.str());

	//Compare with an expected output?
	///string expectedOutput;
	///ReadFile(expectedOutputFname, expectedOutput);
	///checkEqAndUpdateIfRequested(expectedOutput, stream.str(), expectedOutputFname);

	return result.success;
}

XkslangResult XkslangTest::compileAndLink(
	const string shaderName, const string& code,
	const string& entryPointName, EShMessages controls,
	bool flattenUniformArrays)
{
	const EShLanguage kind = GetShaderStage(GetSuffix(shaderName));

	glslang::TShader shader(kind);
	shader.setFlattenUniformArrays(flattenUniformArrays);

	bool success = compile(&shader, code, entryPointName, controls);

	glslang::TProgram program;
	program.addShader(&shader);
	success &= program.link(controls);

	spv::SpvBuildLogger logger;

	if (success && (controls & EShMsgSpvRules)) {
		vector<uint32_t> spirv_binary;
		glslang::GlslangToSpv(*program.getIntermediate(kind),
			spirv_binary, &logger);

		ostringstream disassembly_stream;
		spv::Parameterize();
		spv::Disassemble(disassembly_stream, spirv_binary);
		return{ { { shaderName, shader.getInfoLog(), shader.getInfoDebugLog() }, },
			program.getInfoLog(), program.getInfoDebugLog(),
			logger.getAllMessages(), disassembly_stream.str(), true };
	}
	else {
		return{ { { shaderName, shader.getInfoLog(), shader.getInfoDebugLog() }, },
			program.getInfoLog(), program.getInfoDebugLog(), "", "", false };
	}
}

bool XkslangTest::compile(glslang::TShader* shader, const string& code,
	const string& entryPointName, EShMessages controls,
	const TBuiltInResource* resources)
{
	const char* shaderStrings = code.data();
	const int shaderLengths = static_cast<int>(code.size());

	shader->setStringsWithLengths(&shaderStrings, &shaderLengths, 1);
	if (!entryPointName.empty()) shader->setEntryPoint(entryPointName.c_str());
	return shader->parse(
		(resources ? resources : &glslang::DefaultTBuiltInResource),
		defaultVersion, isForwardCompatible, controls);
}

//=======================================================================================================
//=======================================================================================================
//utils
void XkslangTest::outputResultToStream(ostringstream* stream, const XkslangResult& result, EShMessages controls)
{
	const auto outputIfNotEmpty = [&stream](const std::string& str) {
		if (!str.empty()) *stream << str << "\n";
	};

	for (const auto& shaderResult : result.shaderResults) {
		*stream << shaderResult.shaderName << "\n";
		outputIfNotEmpty(shaderResult.output);
		outputIfNotEmpty(shaderResult.error);
	}
	outputIfNotEmpty(result.linkingOutput);
	outputIfNotEmpty(result.linkingError);
	*stream << result.spirvWarningsErrors;

	if (controls & EShMsgSpvRules) {
		*stream
			<< (result.spirv.empty()
				? "SPIR-V is not generated for failed compile or link\n"
				: result.spirv);
	}
}

EShMessages XkslangTest::DeriveOptions(Source source, Semantics semantics, Target target)
{
	EShMessages result = EShMsgCascadingErrors;

	switch (source) {
	case Source::GLSL:
		break;
	case Source::HLSL:
		result = static_cast<EShMessages>(result | EShMsgReadHlsl);
		break;
	}

	switch (target) {
	case Target::AST:
		result = static_cast<EShMessages>(result | EShMsgAST);
		break;
	case Target::Spv:
		result = static_cast<EShMessages>(result | EShMsgSpvRules);
		result = static_cast<EShMessages>(result | EShMsgKeepUncalled);
		break;
	case Target::BothASTAndSpv:
		result = static_cast<EShMessages>(result | EShMsgSpvRules | EShMsgAST);
		result = static_cast<EShMessages>(result | EShMsgKeepUncalled);
		break;
	};

	switch (semantics) {
	case Semantics::OpenGL:
		break;
	case Semantics::Vulkan:
		result = static_cast<EShMessages>(result | EShMsgVulkanRules | EShMsgSpvRules);
		break;
	}

	return result;
}

EShLanguage XkslangTest::GetShaderStage(const std::string& stage)
{
	if (stage == "vert") {
		return EShLangVertex;
	}
	else if (stage == "tesc") {
		return EShLangTessControl;
	}
	else if (stage == "tese") {
		return EShLangTessEvaluation;
	}
	else if (stage == "geom") {
		return EShLangGeometry;
	}
	else if (stage == "frag") {
		return EShLangFragment;
	}
	else if (stage == "comp") {
		return EShLangCompute;
	}
	else {
		assert(0 && "Unknown shader stage");
		return EShLangCount;
	}
}

//========================================================================================
//========================================================================================
//files IO
bool XkslangTest::ReadFile(const string& path, string& output)
{
	ifstream fstream(path, ios::in);
	if (fstream) {
		string contents;
		fstream.seekg(0, ios::end);
		contents.reserve((string::size_type)fstream.tellg());
		fstream.seekg(0, ios::beg);
		contents.assign((istreambuf_iterator<char>(fstream)),
			istreambuf_iterator<char>());

		output = contents;
		return true;
	}

	return false;
}

pair<bool, vector<uint32_t> > XkslangTest::ReadSpvBinaryFile(const string& path)
{
	ifstream fstream(path, fstream::in | fstream::binary);

	if (!fstream)
		return make_pair(false, vector<uint32_t>());

	vector<uint32_t> contents;

	// Reserve space (for efficiency, not for correctness)
	fstream.seekg(0, fstream.end);
	contents.reserve(size_t(fstream.tellg()) / sizeof(uint32_t));
	fstream.seekg(0, fstream.beg);

	// There is no istream iterator traversing by uint32_t, so we must loop.
	while (!fstream.eof()) {
		uint32_t inWord;
		fstream.read((char *)&inWord, sizeof(inWord));

		if (!fstream.eof())
			contents.push_back(inWord);
	}

	return make_pair(true, contents); // hopefully, c++11 move semantics optimizes the copy away.
}

bool XkslangTest::WriteFile(const string& path, const string& contents)
{
	ofstream fstream(path, ios::out);
	if (!fstream) return false;
	fstream << contents;
	fstream.flush();
	return true;
}

string XkslangTest::GetSuffix(const string& name)
{
	const size_t pos = name.rfind('.');
	return (pos == string::npos) ? "" : name.substr(name.rfind('.') + 1);
}

//========================================================================================
//========================================================================================

//
// Copyright (C)

#include <cassert>
#include <iostream>
#include <memory>
#include <string>

#include "glslang/Public/ShaderLang.h"
#include "StandAlone/ResourceLimits.h"

#include "XkslParser.h"

using namespace std;
using namespace xkslparser;

XkslParser::XkslParser()
{}

XkslParser::~XkslParser()
{}

static bool isInitialized = false;
bool XkslParser::InitialiseXkslang()
{
	assert(!isInitialized);

	bool res = glslang::InitializeProcess();
	isInitialized = res;
	return res;
}

void XkslParser::Finalize()
{
	isInitialized = false;
	glslang::FinalizeProcess();
}

bool XkslParser::ParseXkslShader(const std::string& shaderString)
{
	const char* shaderStrings = shaderString.data();
	const int shaderLengths = static_cast<int>(shaderString.size());

	//TMP  TODO: check the impact of these parameters
	const EShLanguage kind = EShLangFragment;
	bool flattenUniformArrays = false;
	bool isForwardCompatible = false;
	int defaultVersion = 100;
	TBuiltInResource* resources = nullptr;
	bool buildSPRV = false;

	EShMessages controls = static_cast<EShMessages>(EShMsgCascadingErrors | EShMsgReadHlsl | EShMsgAST | EShMsgVulkanRules);
	if (buildSPRV) controls = static_cast<EShMessages>(controls | EShMsgSpvRules | EShMsgKeepUncalled);

	glslang::TShader shader(kind);
	shader.setFlattenUniformArrays(flattenUniformArrays);

	shader.setStringsWithLengths(&shaderStrings, &shaderLengths, 1);
	//shader.setEntryPoint(entryPointName.c_str());

	shader.parse(
		(resources ? resources : &glslang::DefaultTBuiltInResource),
		defaultVersion, isForwardCompatible, controls);

	return true;
}
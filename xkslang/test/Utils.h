//
// Copyright (C) 

#ifndef XKSLANG_TESTS_UTILS_H
#define XKSLANG_TESTS_UTILS_H

#include <cstdint>
#include <fstream>
#include <sstream>
#include <streambuf>
#include <tuple>
#include <string>
#include <vector>

#include "define.h"

#include "glslang/Public/ShaderLang.h"
//#include "StandAlone/ResourceLimits.h"

namespace xkslangtest
{

class Utils
{
public:
	//Utils
	static void OutputResultToStream(std::ostringstream* stream, const GlslangResult& result, EShMessages controls);
	static EShMessages DeriveOptions(Source source, Semantics semantics, Target target);
	static EShLanguage GetShaderStage(const std::string& stage);

	//Files IO
	// Reads the content of the file at the given |path|. On success, returns true
	// and the contents; otherwise, returns false and an empty string.
	static bool ReadFile(const std::string& path, std::string& output);
	static std::pair<bool, std::vector<uint32_t> > ReadSpvBinaryFile(const std::string& path);

	// Writes the given |contents| into the file at the given |path|. Returns true on successful output.
	static bool WriteFile(const std::string& path, const std::string& contents);

	// Returns the suffix of the given |name|.
	static std::string GetSuffix(const std::string& name);
	//========================================================================================
};

}  // namespace glslangtest

#endif  // XKSLANG_TESTS_UTILS_H

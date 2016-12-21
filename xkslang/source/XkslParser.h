//
// Copyright (C) 

#ifndef XKSLANG_XKSLPARSER_H
#define XKSLANG_XKSLPARSER_H

#include <cstdint>
#include <fstream>
#include <sstream>
#include <streambuf>
#include <tuple>
#include <string>
#include <vector>

//#include "glslang/Public/ShaderLang.h"
//#include "StandAlone/ResourceLimits.h"

#include "define.h"

using namespace std;

namespace xkslparser
{

class XkslParser
{
private:

public:
	XkslParser();
	~XkslParser();

	bool InitialiseXkslang();
};

}  // namespace xkslparser

#endif  // XKSLANG_XKSLPARSER_H

//
// Copyright (C)

#include <iostream>
#include <memory>
#include <string>

#include "glslang/Public/ShaderLang.h"
#include "StandAlone/ResourceLimits.h"

#include "XkslParser.h"

using namespace xkslparser;

XkslParser::XkslParser()
{}

XkslParser::~XkslParser()
{}

bool InitialiseXkslang()
{
	glslang::InitializeProcess();

	return true;
}


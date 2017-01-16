//
// Copyright (C)

#include <string>

#include "SpxBytecode.h"

//TMP
#include "../test/Utils.h"
#include "../test/define.h"

using namespace std;
using namespace xkslang;

//=================================================================================================================//
//=================================================================================================================//
//=================================================================================================================//
SpvBytecode::SpvBytecode()
{}

SpvBytecode::~SpvBytecode()
{}

void SpvBytecode::clear()
{
    bytecode.clear();
}

void SpvBytecode::SetBytecode(const vector<uint32_t>& bytecode)
{
    clear();
    this->bytecode = bytecode;
}

const std::vector<uint32_t>& SpvBytecode::getBytecodeStream() const
{
    return bytecode;
}

std::vector<uint32_t>& SpvBytecode::getWritableBytecodeStream()
{
    return bytecode;
}

//=================================================================================================================//
//=================================================================================================================//
//=================================================================================================================//
SpxBytecode::SpxBytecode() : SpvBytecode()
{}

SpxBytecode::~SpxBytecode()
{}

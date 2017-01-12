//
// Copyright (C)

#include <string>

#include "SPXBytecode.h"

//TMP
#include "../test/Utils.h"
#include "../test/define.h"

using namespace std;
using namespace xkslang;

//=================================================================================================================//
//=================================================================================================================//
//=================================================================================================================//
SPVBytecode::SPVBytecode()
{}

SPVBytecode::~SPVBytecode()
{}

void SPVBytecode::clear()
{
    spirXBytecode.clear();
}

void SPVBytecode::SetBytecode(const vector<uint32_t>& bytecode)
{
    clear();
    spirXBytecode = bytecode;
}

const std::vector<uint32_t>& SPVBytecode::getBytecodeStream() const
{
    return spirXBytecode;
}

//=================================================================================================================//
//=================================================================================================================//
//=================================================================================================================//
SPXBytecode::SPXBytecode() : SPVBytecode()
{}

SPXBytecode::~SPXBytecode()
{}

//
// Copyright (C) 

#ifndef XKSLANG_XKSLANG_H__
#define XKSLANG_XKSLANG_H__

#include "define.h"
//#include "SpxBytecode.h"

namespace xkslang
{

class Xkslang
{
public:
    static const unsigned int MagicNumber = 0x07230203;  //SPIRX bytecode magic number (equal to SPV::MagicNumber, can be changed later on)
};

}  // namespace xkslang

#endif  // XKSLANG_XKSLANG_H__

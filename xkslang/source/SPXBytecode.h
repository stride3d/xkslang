//
// Copyright (C) 

#ifndef XKSLANG_SPXBYTECODE_H
#define XKSLANG_SPXBYTECODE_H

#include <vector>

#include "define.h"

namespace xkslang
{

class SPVBytecode
{
public:
    SPVBytecode();
    virtual ~SPVBytecode();

    void clear();
    void SetBytecode(const std::vector<uint32_t>& bytecode);

    const std::vector<uint32_t>& getBytecodeStream() const;

protected:
    std::vector<uint32_t> spirXBytecode;
};

class SPXBytecode : public SPVBytecode
{
public:
    SPXBytecode();
    virtual ~SPXBytecode();
};

}  // namespace xkslang

#endif  // XKSLANG_SPXBYTECODE_H

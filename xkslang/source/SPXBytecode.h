//
// Copyright (C) 

#ifndef XKSLANG_SPX_BYTECODE_H__
#define XKSLANG_SPX_BYTECODE_H__

#include <vector>

#include "define.h"

namespace xkslang
{

class SpvBytecode
{
public:
    SpvBytecode();
    virtual ~SpvBytecode();

    void clear();
    void SetBytecode(const std::vector<uint32_t>& bytecode);

    const std::vector<uint32_t>& getBytecodeStream() const;
    std::vector<uint32_t>& getWritableBytecodeStream();

protected:
    std::vector<uint32_t> bytecode;
};

class SpxBytecode : public SpvBytecode
{
public:
    SpxBytecode();
    virtual ~SpxBytecode();
};

}  // namespace xkslang

#endif  // XKSLANG_SPX_BYTECODE_H__

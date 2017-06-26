//
// Copyright (C) 

#ifndef XKSLANG_SPX_BYTECODE_H__
#define XKSLANG_SPX_BYTECODE_H__

#include <vector>
#include <string>

#include "xkslangDefine.h"

namespace xkslang
{

class SpvBytecode
{
public:
    SpvBytecode();
    SpvBytecode(const std::string& name);
    virtual ~SpvBytecode();

    void clear();
    void SetBytecode(const std::vector<uint32_t>& bytecode);

    const std::vector<uint32_t>& getBytecodeStream() const;
    std::vector<uint32_t>& getWritableBytecodeStream();

    void SetName(const std::string& str) {name = str;}
    const std::string& GetName() const {return name;}

    bool IsEmpty() const { return bytecode.size() == 0; }
    unsigned int GetBytecodeSize() const { return (unsigned int)bytecode.size(); }

protected:
    std::vector<uint32_t> bytecode;
    std::string name;
};

class SpxBytecode : public SpvBytecode
{
public:
    SpxBytecode();
    SpxBytecode(const std::string& name);
    virtual ~SpxBytecode();
};

}  // namespace xkslang

#endif  // XKSLANG_SPX_BYTECODE_H__

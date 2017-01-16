//
// Copyright (C) 

#ifndef XKSLANG_XKSL_SPX_STREAM_REMAPPER_H__
#define XKSLANG_XKSL_SPX_STREAM_REMAPPER_H__

#include <string>
#include <vector>
#include <stack>
#include <sstream>

#include "SPIRV/SPVRemapper.h"

#include "define.h"
#include "SpxBytecode.h"

namespace xkslang
{
//==============================================================================================================//
//===========================================  SpxStreamRemapper  ==============================================//
//==============================================================================================================//

class SpxStreamRemapper : public spv::spirvbin_t
{
public:
    enum class RemapperStatusEnum
    {
        Invalid,
        Valid
    };

public:
    SpxStreamRemapper(int verbose = 0) : spirvbin_t(verbose), status(RemapperStatusEnum::Invalid){ }
    virtual ~SpxStreamRemapper() { }

    bool MapSpxStream(const SpxBytecode& bytecode);

    bool GetMappedSpxBytecode(SpxBytecode& bytecode);
    bool GenerateSpvStageBytecode(ShadingStage stage, std::string entryPoint, SpvBytecode& output);

    virtual void error(const std::string& txt) const;
    void copyMessagesTo(std::vector<std::string>& list);

    RemapperStatusEnum GetStatus() { return status; }

private:
    RemapperStatusEnum status;
    void dceXkslDecorateAndName();

    std::vector<std::string> errorMessages;
};

}  // namespace xkslang

#endif  // XKSLANG_XKSL_SPX_STREAM_REMAPPER_H__

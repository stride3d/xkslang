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
    SpxStreamRemapper(int verbose = 0) : spirvbin_t(verbose){ }
    virtual ~SpxStreamRemapper() { }

    bool MixSpxBytecodeStream(const SpxBytecode& bytecode);

    bool GetMappedSpxBytecode(SpxBytecode& bytecode);
    bool GenerateSpvStageBytecode(ShadingStage stage, std::string entryPointName, SpvBytecode& output);

    virtual void error(const std::string& txt) const;
    void copyMessagesTo(std::vector<std::string>& list);

    spv::ExecutionModel GetShadingStageExecutionMode(ShadingStage stage);

private:
    std::unordered_map<spv::Id, std::string> declarationNameMap;  // delaration name from functions and shaders (XKSL extensions)

    void dceXkslData();
    void dceEntryPoints();

    virtual void buildLocalMaps();
    bool BuildAndSetShaderStageHeader(ShadingStage stage, spv::Id entryFunctionId, std::string unmangledFunctionName);
    bool ProcessOverridingMethods();

    std::vector<std::string> errorMessages;
};

}  // namespace xkslang

#endif  // XKSLANG_XKSL_SPX_STREAM_REMAPPER_H__

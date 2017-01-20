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

enum class SpxRemapperStatusEnum
{
    Undefined,
    MixinInProgress,
    MixinBeingFinalized,
    MixinFinalized
};

class SpxStreamRemapper : public spv::spirvbin_t
{

public:
    SpxStreamRemapper(int verbose = 0);
    virtual ~SpxStreamRemapper() { }

    bool MixSpxBytecodeStream(const SpxBytecode& bytecode);
    bool FinalizeMixin();

    bool GetMappedSpxBytecode(SpxBytecode& bytecode);
    bool GenerateSpvStageBytecode(ShadingStage stage, std::string entryPointName, SpvBytecode& output);

    virtual void error(const std::string& txt) const;
    void copyMessagesTo(std::vector<std::string>& list);

    spv::ExecutionModel GetShadingStageExecutionMode(ShadingStage stage);

private:
    void dceXkslData();
    void dceEntryPoints();

    virtual void buildLocalMaps();
    bool BuildAndSetShaderStageHeader(ShadingStage stage, spv::Id entryFunctionId, std::string unmangledFunctionName);
    bool BuildOverridenFunctionMap();
    bool RemapAllOverridenFunctions();
    bool ConvertSpirxToSpirVBytecode();

private:
    SpxRemapperStatusEnum status;

    std::vector<std::string> errorMessages;

    std::unordered_map<spv::Id, std::string> declarationNameMap;     // delaration name from functions and shaders (XKSL extensions)

    //map of methods being overriden (first id), associated with a pair<id of the overriding method, method level in the tree inheritance>
    std::unordered_map<spv::Id, std::pair<spv::Id, int>> mapOverridenFunctions;
};

}  // namespace xkslang

#endif  // XKSLANG_XKSL_SPX_STREAM_REMAPPER_H__

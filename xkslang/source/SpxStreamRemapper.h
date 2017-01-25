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
    WaitingForMixin,
    MixinInProgress,
    MixinBeingFinalized,
    MixinFinalized
};

class SpxStreamRemapper : public spv::spirvbin_t
{
    typedef std::pair<spv::Id, int> pairIdPos;

    class FunctionData;
    class ShaderClassData;
    class TypeData
    {
    public:
        spv::Op opCode;
        spv::Id id;
        unsigned int pos;
        ShaderClassData* owner;
        std::string debugName;  //for Debug

        TypeData(spv::Op op, spv::Id id, unsigned int pos) : opCode(op), id(id), pos(pos), owner(nullptr){}
        std::string& GetName(){return debugName;}
    };

    class ConstData
    {
    public:
        spv::Op opCode;
        spv::Id id;
        unsigned int pos;

        ConstData(spv::Op op, spv::Id id, unsigned int pos) : opCode(op), id(id), pos(pos) {}
    };

    class ShaderClassData
    {
    public:
        spv::Id id;
        std::string name;
        int level;
        std::vector<ShaderClassData*> parentsList;
        std::vector<FunctionData*> functionsList;
        std::vector<TypeData*> typesList;

        ShaderClassData(spv::Id id, std::string name): id(id), name(name), level(-1){}
        std::string& GetName() { return name; }

        void AddParent(ShaderClassData* parent) { parentsList.push_back(parent); }
        bool HasParent(ShaderClassData* parent) {
            for (int i = 0; i<parentsList.size(); ++i) if (parentsList[i] == parent) return true;
            return false;
        }
        
        void AddFunction(FunctionData* function) { functionsList.push_back(function); }
        bool HasFunction(FunctionData* function) {
            for (int i = 0; i<functionsList.size(); ++i) if (functionsList[i] == function) return true;
            return false;
        }

        void AddType(TypeData* type) { typesList.push_back(type); }
        bool HasType(TypeData* type) {
            for (int i = 0; i<typesList.size(); ++i) if (typesList[i] == type) return true;
            return false;
        }
    };

    class FunctionData
    {
    public:
        spv::Id id;
        std::string mangledName;
        ShaderClassData* owner;
        bool hasOverride;  //has the override attribute
        FunctionData* overridenBy;  //the function is being overriden by another function

        FunctionData(spv::Id id, std::string mangledName) : id(id), mangledName(mangledName), owner(nullptr), hasOverride(false), overridenBy(nullptr){}
    };

public:
    SpxStreamRemapper(int verbose = 0);
    virtual ~SpxStreamRemapper();

    bool MixWithSpxBytecode(const SpxBytecode& bytecode);
    bool FinalizeMixin();

    bool GetMappedSpxBytecode(SpxBytecode& bytecode);
    bool GenerateSpvStageBytecode(ShadingStage stage, std::string entryPointName, SpvBytecode& output);

    virtual void error(const std::string& txt) const;
    bool error(const std::string& txt);
    void copyMessagesTo(std::vector<std::string>& list);

    spv::ExecutionModel GetShadingStageExecutionMode(ShadingStage stage);

private:
    bool MergeWithBytecode(const SpxBytecode& bytecode);
    bool SetBytecode(const SpxBytecode& bytecode);

    void ClearAllMaps();
    bool BuildDeclarationName_ConstType_FunctionPos_Maps(std::unordered_map<spv::Id, range_t>& functionPos);
    bool BuildTypesAndConstsHashmap(std::unordered_map<uint32_t, pairIdPos>& mapHashPos);
    bool BuildAllMaps();
    bool BuildOverridenFunctionMap();

    void GetBytecodeTypeMap(std::unordered_map<spv::Id, unsigned>& mapTypes);

    bool BuildAndSetShaderStageHeader(ShadingStage stage, FunctionData* entryFunction, std::string unmangledFunctionName);
    bool RemapAllOverridenFunctions();
    bool ConvertSpirxToSpirVBytecode();

private:
    SpxRemapperStatusEnum status;

    std::vector<std::string> errorMessages;
   
    std::unordered_map<spv::Id, std::string> mapDeclarationName;            // delaration name (user defined name) for methods, shaders and variables

    std::unordered_map<spv::Id, ShaderClassData*> mapShadersById;
    std::unordered_map<std::string, ShaderClassData*> mapShadersByName;
    std::unordered_map<spv::Id, FunctionData*> mapFunctionsById;
    std::unordered_map<spv::Id, TypeData*> mapTypeById;
    std::unordered_map<spv::Id, ConstData*> mapConstById;

    std::string GetDeclarationNameForId(spv::Id id);
    bool GetDeclarationNameForId(spv::Id id, std::string& name);
    ShaderClassData* GetShaderByName(const std::string& name);
    ShaderClassData* HasShader(const std::string& name);
    ShaderClassData* GetShaderById(spv::Id id);
    FunctionData* GetFunctionById(spv::Id id);
    FunctionData* IsFunction(spv::Id id);
    TypeData* IsType(spv::Id id);
    ConstData* IsConst(spv::Id id);
    //ConstData* HasConstEqualTo(ConstData*);
    //TypeData* HasSimilarTypeDefinedInBytecode(TypeData*);

    void stripBytecode(std::vector<range_t>& ranges);

    void CopyInstructionAtEndOfVector(std::vector<spirword_t>& vec, unsigned opStart){
        auto start = spv.begin() + opStart;
        auto end = start + asWordCount(opStart);
        vec.insert(vec.end(), start, end);
    }
    void CopyInstructionAtBeginningOfVector(std::vector<spirword_t>& vec, unsigned opStart){
        auto start = spv.begin() + opStart;
        auto end = start + asWordCount(opStart);
        vec.insert(vec.begin(), start, end);
    }
};

}  // namespace xkslang

#endif  // XKSLANG_XKSL_SPX_STREAM_REMAPPER_H__

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
    class TypeData;
    class VariableData
    {
    public:
        spv::Op opCode;
        spv::Id id;
        unsigned int pos;
        TypeData* type;
        std::string variableName;  //we need the name to identify variables when mixin shaders

        VariableData(spv::Op op, spv::Id id, TypeData* type, const std::string& variableName, unsigned int pos) : opCode(op), id(id), type(type), variableName(variableName), pos(pos) {}
    };

    class TypeData
    {
    public:
        spv::Op opCode;
        spv::Id id;
        unsigned int pos;
        ShaderClassData* owner;
        TypeData* pointerToType;  //if the type is a pointer to another type
        std::string debugName;  //for Debug

        TypeData(spv::Op op, spv::Id id, unsigned int pos) : opCode(op), id(id), pos(pos), owner(nullptr), pointerToType(nullptr){}
        std::string& GetName(){return debugName;}
        void SetPointerToType(TypeData* type){ pointerToType  = type;}
    };

    //A type declared by a shader, we store the type definition, plus the variable and pointer to access it
    class ShaderTypeData
    {
    public:
        TypeData* type;
        TypeData* pointerToType;
        VariableData* variable;

        ShaderTypeData(TypeData* type, TypeData* pointerToType, VariableData* variable) : type(type), pointerToType(pointerToType), variable(variable){}
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
        TypeData* shaderType;
        std::string name;
        int level;
        std::vector<ShaderClassData*> parentsList;
        std::vector<FunctionData*> functionsList;
        std::vector<ShaderTypeData*> shaderTypesList;

        ShaderClassData(TypeData* shaderType, std::string name): shaderType(shaderType), name(name), level(-1){}
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

        void AddShaderType(ShaderTypeData* type) { shaderTypesList.push_back(type); }
        bool HasType(TypeData* type) {
            for (int i = 0; i<shaderTypesList.size(); ++i) if (shaderTypesList[i]->type == type) return true;
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
        uint32_t posStart;
        uint32_t posEnd;

        FunctionData(spv::Id id, std::string mangledName)
            : id(id), mangledName(mangledName), owner(nullptr), hasOverride(false), overridenBy(nullptr), posStart(0), posEnd(0){}

        void SetRangePos(uint32_t start, uint32_t end) {
            posStart = start; posEnd = end;
        }
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

    void GetBytecodeTypeMap(std::unordered_map<spv::Id, uint32_t>& mapTypes);

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
    std::unordered_map<spv::Id, VariableData*> mapVariablesById;

    std::string GetDeclarationNameForId(spv::Id id);
    bool GetDeclarationNameForId(spv::Id id, std::string& name);
    ShaderClassData* GetShaderByName(const std::string& name);
    ShaderClassData* HasShader(const std::string& name);
    ShaderClassData* GetShaderById(spv::Id id);
    FunctionData* GetFunctionById(spv::Id id);
    FunctionData* IsFunction(spv::Id id);
    TypeData* HasATypeForId(spv::Id id);
    TypeData* HasPointerToType(TypeData* type);
    VariableData* HasVariableForType(TypeData* type);
    VariableData* HasVariableForId(spv::Id id);
    ConstData* HasAConstForId(spv::Id id);


    void stripBytecode(std::vector<range_t>& ranges);

    void CopyInstructionAtEndOfVector(std::vector<spirword_t>& vec, uint32_t opStart){
        auto start = spv.begin() + opStart;
        auto end = start + asWordCount(opStart);
        vec.insert(vec.end(), start, end);
    }
    void CopyInstructionsAtEndOfVector(std::vector<spirword_t>& vec, uint32_t opStart, uint32_t opEnd) {
        auto start = spv.begin() + opStart;
        auto end = spv.begin() + opEnd;
        vec.insert(vec.end(), start, end);
    }
    void CopyInstructionAtBeginningOfVector(std::vector<spirword_t>& vec, uint32_t opStart){
        auto start = spv.begin() + opStart;
        auto end = start + asWordCount(opStart);
        vec.insert(vec.begin(), start, end);
    }
};

}  // namespace xkslang

#endif  // XKSLANG_XKSL_SPX_STREAM_REMAPPER_H__

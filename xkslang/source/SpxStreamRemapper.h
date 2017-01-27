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

    //============================================================================================================================================
    //============================================================================================================================================
    enum class ObjectInstructionTypeEnum
    {
        Shader,
        Const,
        Type,
        Variable,
        Function,
    };

    //Generic, simplified data type that we build while parsing the bytecode
    class ParsedObjectData
    {
    public:
        ObjectInstructionTypeEnum kind;
        spv::Op opCode;
        spv::Id resultId;
        spv::Id typeId;
        spv::Id targetId;  //for some data we already read the target (for example with OpTypePointer)
        uint32_t bytecodeStartPosition;
        uint32_t bytecodeEndPosition;

        ParsedObjectData(){}
        ParsedObjectData(ObjectInstructionTypeEnum kind, spv::Op op, spv::Id resultId, spv::Id typeId, uint32_t startPos, uint32_t endPos)
            : kind(kind), opCode(op), resultId(resultId), typeId(typeId), bytecodeStartPosition(startPos), bytecodeEndPosition(endPos), targetId(spv::NoResult){}

        void SetTargetId(spv::Id id){targetId  = id;}
    };

    //More advanced data structure to process the different obejcts and their relationship
    //Base class for SPX object
    class ShaderClassData;
    class ObjectInstructionBase
    {
    public:
        ObjectInstructionBase(const ParsedObjectData& parsedData, std::string name)
            : kind(parsedData.kind), opCode(parsedData.opCode), resultId(parsedData.resultId), typeId(parsedData.typeId),
              bytecodeStartPosition(parsedData.bytecodeStartPosition), bytecodeEndPosition(parsedData.bytecodeEndPosition), name(name), shaderOwner(nullptr) {}
        virtual ~ObjectInstructionBase(){}

        ObjectInstructionTypeEnum GetKind() const {return kind;}
        const std::string& GetName() const {return name;}
        spv::Op GetOpCode() const {return opCode;}
        spv::Id GetResultId() const { return resultId; }
        
        uint32_t GetBytecodeStartPosition() const {return bytecodeStartPosition;}
        uint32_t GetBytecodeEndPosition() const { return bytecodeEndPosition; }
        void SetBytecodeStartPosition(uint32_t pos) {bytecodeStartPosition = pos;}
        void SetBytecodeRangePositions(uint32_t start, uint32_t end) {bytecodeStartPosition = start; bytecodeEndPosition = end;}

        void SetShaderOwner(ShaderClassData* owner) { shaderOwner = owner; }
        ShaderClassData* GetShaderOwner() const { return shaderOwner; }

    private:
        ObjectInstructionTypeEnum kind;
        std::string name;
        spv::Op opCode;
        spv::Id resultId;
        spv::Id typeId;
        ShaderClassData* shaderOwner;  //some object can belong to a shader

        //those fields can change when we mix bytecodes
        uint32_t bytecodeStartPosition;
        uint32_t bytecodeEndPosition;
    };

    class ConstInstruction : public ObjectInstructionBase
    {
    public:
        ConstInstruction(const ParsedObjectData& parsedData, std::string name)
            : ObjectInstructionBase(parsedData, name) {}
        virtual ~ConstInstruction() {}
    };

    class TypeInstruction : public ObjectInstructionBase
    {
    public:
        TypeInstruction(const ParsedObjectData& parsedData, std::string name)
            : ObjectInstructionBase(parsedData, name), pointerTo(nullptr){}
        virtual ~TypeInstruction() {}

        void SetTypePointed(TypeInstruction* type) { pointerTo = type; }
        TypeInstruction* GetTypePointed() const { return pointerTo; }

    private:
        TypeInstruction* pointerTo;
    };

    class VariableInstruction : public ObjectInstructionBase
    {
    public:
        VariableInstruction(const ParsedObjectData& parsedData, std::string name)
            : ObjectInstructionBase(parsedData, name), variableTo(nullptr) {}
        virtual ~VariableInstruction() {}

        void SetTypePointed(TypeInstruction* type) { variableTo = type; }
        TypeInstruction* GetTypePointed() const { return variableTo; }

    private:
        TypeInstruction* variableTo;
    };

    class FunctionInstruction : public ObjectInstructionBase
    {
    public:
        FunctionInstruction(const ParsedObjectData& parsedData, std::string name)
            : ObjectInstructionBase(parsedData, name), hasAttributeOverride(false), overridenBy(nullptr){}
        virtual ~FunctionInstruction() {}

        const std::string& GetMangledName() const { return GetName(); }
        void SetOverridingFunction(FunctionInstruction* function) { overridenBy = function; }
        FunctionInstruction* GetOverridingFunction() const { return overridenBy; }
        void SetAttributeOverride(bool b) { hasAttributeOverride = b; }
        bool HasAttributeOverride() const { return hasAttributeOverride; }

    private:
        bool hasAttributeOverride;
        FunctionInstruction* overridenBy;  //the function is being overriden by another function
    };

    //This is a type declared by a shader: we store the type definition, plus the variable and pointer to access it
    class ShaderTypeData
    {
    public:
        TypeInstruction* type;
        TypeInstruction* pointerToType;
        VariableInstruction* variable;

        ShaderTypeData(TypeInstruction* type, TypeInstruction* pointerToType, VariableInstruction* variable) : type(type), pointerToType(pointerToType), variable(variable){}
    };

    class ShaderClassData : public ObjectInstructionBase
    {
    public:
        ShaderClassData(const ParsedObjectData& parsedData, std::string name)
            : ObjectInstructionBase(parsedData, name), level(-1) {
        }
        virtual ~ShaderClassData() {}

        void AddParent(ShaderClassData* parent) { parentsList.push_back(parent); }
        bool HasParent(ShaderClassData* parent) {
            for (int i = 0; i<parentsList.size(); ++i) if (parentsList[i] == parent) return true;
            return false;
        }

        void AddFunction(FunctionInstruction* function) { functionsList.push_back(function); }
        bool HasFunction(FunctionInstruction* function) {
            for (int i = 0; i<functionsList.size(); ++i) if (functionsList[i] == function) return true;
            return false;
        }

        void AddShaderType(ShaderTypeData* type) { shaderTypesList.push_back(type); }
        bool HasType(TypeInstruction* type) {
            for (int i = 0; i<shaderTypesList.size(); ++i) if (shaderTypesList[i]->type == type) return true;
            return false;
        }

    public:
        int level;
        std::vector<ShaderClassData*> parentsList;
        std::vector<ShaderTypeData*> shaderTypesList;
        std::vector<FunctionInstruction*> functionsList;
    };
    //============================================================================================================================================
    //============================================================================================================================================

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
    bool SetBytecode(const SpxBytecode& bytecode);
    bool MergeWithBytecode(const SpxBytecode& bytecode);

    void ReleaseAllMaps();
    bool BuildDeclarationNameMapsAndObjectsDataList(std::vector<ParsedObjectData>& listParsedObjectsData);
    bool BuildAllMaps();
    //bool UpdateAllMaps();

    bool BuildOverridenFunctionMap();
    bool UpdateOpFunctionCallTargetsInstructionsToOverridingFunctions();

    bool BuildAndSetShaderStageHeader(ShadingStage stage, FunctionInstruction* entryFunction, std::string unmangledFunctionName);
    bool ConvertSpirxToSpirVBytecode();

private:
    SpxRemapperStatusEnum status;

    std::vector<std::string> errorMessages;

    std::unordered_map<spv::Id, std::string> mapDeclarationName;            // delaration name (user defined name) for methods, shaders and variables

    std::vector<ObjectInstructionBase*> listAllObjects;
    std::vector<ShaderClassData*> vecAllShaders;
    std::vector<FunctionInstruction*> vecAllShaderFunctions;  //vec of all functions declared by a shader

    //std::unordered_map<spv::Id, ShaderClassData*> mapShadersById;
    //
    //
    //std::unordered_map<spv::Id, TypeData*> mapTypeById;
    //std::unordered_map<spv::Id, ConstData*> mapConstById;
    //std::unordered_map<spv::Id, VariableData*> mapVariablesById;

    std::string GetDeclarationNameForId(spv::Id id);
    bool GetDeclarationNameForId(spv::Id id, std::string& name);
    ShaderClassData* GetShaderByName(const std::string& name);
    ShaderClassData* GetShaderById(spv::Id id);
    FunctionInstruction* GetFunctionById(spv::Id id);
    TypeInstruction* GetTypeById(spv::Id id);
    TypeInstruction* GetTypePointingTo(TypeInstruction* targetType);
    VariableInstruction* GetVariablePointingTo(TypeInstruction* targetType);

    //ShaderClassData* HasShader(const std::string& name);
    //ShaderClassData* GetShaderById(spv::Id id);
    //TypeData* HasATypeForId(spv::Id id);
    //
    //VariableData* HasVariableForType(TypeData* type);
    //VariableData* HasVariableForId(spv::Id id);
    //ConstData* HasAConstForId(spv::Id id);


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

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
#include "OutputStageBytecode.h"

namespace xkslang
{
//==============================================================================================================//
//===========================================  SpxStreamRemapper  ==============================================//
//==============================================================================================================//

static const unsigned int MagicNumber = 0x07230203;
static const unsigned int Version = 0x00010000;
static const unsigned int Revision = 8;
static const unsigned int builderNumber = 0x00080001;

enum class SpxRemapperStatusEnum
{
    WaitingForMixin,
    MixinInProgress,

    //define the order of compilation
    MixinBeingCompiled_UnusedShaderRemoved,
    MixinBeingCompiled_CompositionInstancesProcessed,
    MixinFinalized
};

class XkslMixerOutputStage;
class SpxStreamRemapper : public spv::spirvbin_t
{
public:
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
        HeaderProperty,
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
        ObjectInstructionBase(const ParsedObjectData& parsedData, std::string name, SpxStreamRemapper* source)
            : kind(parsedData.kind), opCode(parsedData.opCode), resultId(parsedData.resultId), typeId(parsedData.typeId), name(name), shaderOwner(nullptr),
            bytecodeStartPosition(parsedData.bytecodeStartPosition), bytecodeEndPosition(parsedData.bytecodeEndPosition), bytecodeSource(source){}
        virtual ~ObjectInstructionBase(){}
        virtual ObjectInstructionBase* CloneBasicData() {
            return new ObjectInstructionBase(ParsedObjectData(kind, opCode, resultId, typeId, bytecodeStartPosition, bytecodeEndPosition), name, nullptr);
        }

        ObjectInstructionTypeEnum GetKind() const {return kind;}
        const std::string& GetName() const {return name;}
        spv::Op GetOpCode() const {return opCode;}
        spv::Id GetResultId() const { return resultId; }
        spv::Id GetId() const { return resultId; }
        
        uint32_t GetBytecodeStartPosition() const {return bytecodeStartPosition;}
        uint32_t GetBytecodeEndPosition() const { return bytecodeEndPosition; }
        void SetBytecodeStartPosition(uint32_t pos) {bytecodeStartPosition = pos;}
        void SetBytecodeRangePositions(uint32_t start, uint32_t end) {bytecodeStartPosition = start; bytecodeEndPosition = end;}

        void SetShaderOwner(ShaderClassData* owner) { shaderOwner = owner; }
        ShaderClassData* GetShaderOwner() const { return shaderOwner; }

    protected:
        ObjectInstructionTypeEnum kind;
        std::string name;
        spv::Op opCode;
        spv::Id resultId;
        spv::Id typeId;
        ShaderClassData* shaderOwner;  //some object can belong to a shader
        SpxStreamRemapper* bytecodeSource;

        //those fields can change when we mix bytecodes
        uint32_t bytecodeStartPosition;
        uint32_t bytecodeEndPosition;

        friend class SpxStreamRemapper;
    };

    class HeaderPropertyInstruction : public ObjectInstructionBase
    {
    public:
        HeaderPropertyInstruction(const ParsedObjectData& parsedData, std::string name, SpxStreamRemapper* source)
            : ObjectInstructionBase(parsedData, name, source) {}
        virtual ~HeaderPropertyInstruction() {}
        virtual ObjectInstructionBase* CloneBasicData() {
            HeaderPropertyInstruction* obj = new HeaderPropertyInstruction(ParsedObjectData(kind, opCode, resultId, typeId, bytecodeStartPosition, bytecodeEndPosition), name, nullptr);
            return obj;
        }
    };

    class ConstInstruction : public ObjectInstructionBase
    {
    public:
        ConstInstruction(const ParsedObjectData& parsedData, std::string name, SpxStreamRemapper* source)
            : ObjectInstructionBase(parsedData, name, source) {}
        virtual ~ConstInstruction() {}
        virtual ObjectInstructionBase* CloneBasicData() {
            ConstInstruction* obj = new ConstInstruction(ParsedObjectData(kind, opCode, resultId, typeId, bytecodeStartPosition, bytecodeEndPosition), name, nullptr);
            return obj;
        }
    };

    class TypeInstruction : public ObjectInstructionBase
    {
    public:
        TypeInstruction(const ParsedObjectData& parsedData, std::string name, SpxStreamRemapper* source)
            : ObjectInstructionBase(parsedData, name, source), pointerTo(nullptr){}
        virtual ~TypeInstruction() {}
        virtual ObjectInstructionBase* CloneBasicData() {
            TypeInstruction* obj = new TypeInstruction(ParsedObjectData(kind, opCode, resultId, typeId, bytecodeStartPosition, bytecodeEndPosition), name, nullptr);
            return obj;
        }

        void SetTypePointed(TypeInstruction* type) { pointerTo = type; }
        TypeInstruction* GetTypePointed() const { return pointerTo; }

    private:
        TypeInstruction* pointerTo;
    };

    class VariableInstruction : public ObjectInstructionBase
    {
    public:
        VariableInstruction(const ParsedObjectData& parsedData, std::string name, SpxStreamRemapper* source)
            : ObjectInstructionBase(parsedData, name, source), variableTo(nullptr) {}
        virtual ~VariableInstruction() {}
        virtual ObjectInstructionBase* CloneBasicData() {
            VariableInstruction* obj = new VariableInstruction(ParsedObjectData(kind, opCode, resultId, typeId, bytecodeStartPosition, bytecodeEndPosition), name, nullptr);
            return obj;
        }

        void SetTypePointed(TypeInstruction* type) { variableTo = type; }
        TypeInstruction* GetTypePointed() const { return variableTo; }

    private:
        TypeInstruction* variableTo;
    };

    class FunctionInstruction : public ObjectInstructionBase
    {
    public:
        enum class OverrideAttributeStateEnum
        {
            Undefined,
            Defined,
            Processed,
        };

        FunctionInstruction(const ParsedObjectData& parsedData, std::string name, SpxStreamRemapper* source)
            : ObjectInstructionBase(parsedData, name, source), isStatic(false), overrideAttributeState(OverrideAttributeStateEnum::Undefined), overridenBy(nullptr), fullName(name), flag1(0){}
        virtual ~FunctionInstruction() {}
        virtual ObjectInstructionBase* CloneBasicData() {
            FunctionInstruction* obj = new FunctionInstruction(ParsedObjectData(kind, opCode, resultId, typeId, bytecodeStartPosition, bytecodeEndPosition), name, nullptr);
            obj->isStatic = isStatic;
            obj->overrideAttributeState = overrideAttributeState;
            obj->fullName = fullName;
            return obj;
        }

        const std::string& GetMangledName() const { return GetName(); }
        const std::string& GetFullName() const { return fullName; }
        void SetOverridingFunction(FunctionInstruction* function) { overridenBy = function; }
        FunctionInstruction* GetOverridingFunction() const { return overridenBy; }
        void SetFullName(const std::string& str) { fullName = str; }

        void ParsedStaticAttribute(){isStatic = true;}
        bool IsStatic(){return isStatic;}

        void ParsedOverrideAttribute(){if (overrideAttributeState == OverrideAttributeStateEnum::Undefined) overrideAttributeState = OverrideAttributeStateEnum::Defined; }
        OverrideAttributeStateEnum GetOverrideAttributeState() const { return overrideAttributeState; }
        void SetOverrideAttributeState(OverrideAttributeStateEnum state) { overrideAttributeState = state; }

    private:
        bool isStatic;
        OverrideAttributeStateEnum overrideAttributeState;
        FunctionInstruction* overridenBy;  //the function is being overriden by another function
        std::string fullName;  //name only use for debug purpose
        int flag1;  //to simplify some algo

        friend class SpxStreamRemapper;
    };

    //This is a type declared by a shader: we store the type definition, plus the variable and pointer to access it
    class ShaderTypeData
    {
    public:
        TypeInstruction* type;
        TypeInstruction* pointerToType;
        VariableInstruction* variable;

        ShaderTypeData(TypeInstruction* type, TypeInstruction* pointerToType, VariableInstruction* variable) : type(type), pointerToType(pointerToType), variable(variable){}
        virtual ~ShaderTypeData(){}
    };

    class ShaderComposition
    {
    public:
        int compositionShaderId;
        ShaderClassData* compositionShaderOwner;
        ShaderClassData* shaderType;
        std::string variableName;
        bool isArray;
        int countInstances;

        const std::string& GetVariableName() const {return variableName;}

        //ShaderClassData* instantiatedShader;  //resulting shader instance from the composition

        ShaderComposition(int compositionShaderId, ShaderClassData* compositionShaderOwner, ShaderClassData* shaderType,
            const std::string& variableName, bool isArray, int countInstances)
                :compositionShaderId(compositionShaderId), compositionShaderOwner(compositionShaderOwner), shaderType(shaderType),
                variableName(variableName), isArray(isArray), countInstances(countInstances){}

    private:
        friend class SpxStreamRemapper;
    };

    class CompositionInstanceData
    {
    public:
        ShaderComposition* composition;
        unsigned int posStart;
        unsigned int posEnd;

        CompositionInstanceData(ShaderComposition* composition, unsigned int posStart, unsigned int end) :
            composition(composition), posStart(posStart), posEnd(posEnd) {}
    };

    class CompositionForEachLoopData
    {
    public:
        ShaderComposition* composition;
        unsigned int nestedLevel;
        unsigned int foreachLoopStart;
        unsigned int foreachLoopEnd;
        unsigned int firstLoopInstuctionStart;
        unsigned int lastLoopInstuctionEnd;

        //duplicated bytecode after we clone (unroll) each forlopp compositions
        std::vector<spirword_t> foreachDuplicatedBytecode;

        CompositionForEachLoopData(ShaderComposition* composition, unsigned int nestedLevel, unsigned int foreachLoopStart, unsigned int foreachLoopEnd, unsigned int firstLoopInstuctionStart, unsigned int lastLoopInstuctionEnd) :
            composition(composition), nestedLevel(nestedLevel), foreachLoopStart(foreachLoopStart), foreachLoopEnd(foreachLoopEnd), firstLoopInstuctionStart(firstLoopInstuctionStart), lastLoopInstuctionEnd(lastLoopInstuctionEnd) {}
    };

    class ShaderClassData : public ObjectInstructionBase
    {
    public:
        enum class ShaderDependencyTypeEnum
        {
            Undefined = 0,
            StaticFunctionCall = 1 << 0,
            Other = 1 << 1,
        };

    public:
        ShaderClassData(const ParsedObjectData& parsedData, std::string name, SpxStreamRemapper* source)
            : ObjectInstructionBase(parsedData, name, source), level(-1), flag(0), flag1(0), tmpClonedShader(nullptr){
        }
        virtual ~ShaderClassData() {
            for (auto it = shaderTypesList.begin(); it != shaderTypesList.end(); it++) delete (*it);
        }
        virtual ObjectInstructionBase* CloneBasicData() {
            ShaderClassData* obj = new ShaderClassData(ParsedObjectData(kind, opCode, resultId, typeId, bytecodeStartPosition, bytecodeEndPosition), name, nullptr);
            obj->level = level;
            obj->flag = flag;
            return obj;
        }

        void AddParent(ShaderClassData* parent) { parentsList.push_back(parent); }
        bool HasParent(ShaderClassData* parent) {
            for (unsigned int i = 0; i<parentsList.size(); ++i) if (parentsList[i] == parent) return true;
            return false;
        }

        void AddFunction(FunctionInstruction* function) { functionsList.push_back(function); }
        bool HasFunction(FunctionInstruction* function) {
            for (unsigned int i = 0; i<functionsList.size(); ++i) if (functionsList[i] == function) return true;
            return false;
        }
        FunctionInstruction* GetFunctionByName(const std::string& name) {
            for (auto it = functionsList.begin(); it != functionsList.end(); ++it){
                FunctionInstruction* function = *it;
                if (function->GetName() == name) return function;
            }
            return nullptr;
        }
        unsigned int GetCountFunctions() { return functionsList.size(); }

        void AddShaderType(ShaderTypeData* type) { shaderTypesList.push_back(type); }
        bool HasType(TypeInstruction* type) {
            for (unsigned int i = 0; i<shaderTypesList.size(); ++i) if (shaderTypesList[i]->type == type) return true;
            return false;
        }

        unsigned int GetCountShaderComposition() { return compositionsList.size(); }
        void AddComposition(const ShaderComposition& composition) { compositionsList.push_back(composition); }
        ShaderComposition* GetShaderCompositionById(int compositionId) {
            if (compositionId<0 || compositionId>= (int)compositionsList.size()) return nullptr;
            return &(compositionsList[compositionId]);
        }
        ShaderComposition* GetShaderCompositionByName(const std::string variableName) {
            for (unsigned int i=0; i<compositionsList.size(); ++i)
                if (compositionsList[i].variableName == variableName) return &(compositionsList[i]);
            return nullptr;
        }

    public:
        int level;
        std::vector<ShaderClassData*> parentsList;
        std::vector<ShaderTypeData*> shaderTypesList;
        std::vector<FunctionInstruction*> functionsList;

        std::vector<ShaderComposition> compositionsList;

    private:
        //When merging/duplicating a shader into a bytecode, this field will hold a temporary reference to its resulting, cloned shader 
        ShaderClassData* tmpClonedShader;
        int flag, flag1;  //to simplify some algo
        ShaderDependencyTypeEnum dependencyType;  //dependency type, set by GetShadersFullDependencies algorithm.

    friend class SpxStreamRemapper;
    };

    class ShaderToMergeData
    {
    public:
        ShaderClassData* shader;
        bool instantiateShader;

        ShaderToMergeData(ShaderClassData* shaderToMerge): shader(shaderToMerge), instantiateShader(false) {}
        ShaderToMergeData(ShaderClassData* shader, bool instantiateShader) : shader(shader), instantiateShader(instantiateShader) {}
    };
    //============================================================================================================================================
    //============================================================================================================================================

public:
    SpxStreamRemapper(int verbose = 0);
    virtual ~SpxStreamRemapper();

    SpxStreamRemapper* Clone();

    //bool MixWithBytecode(const SpxBytecode& bytecode);
    bool MixWithShadersFromBytecode(const SpxBytecode& sourceBytecode, const std::vector<std::string>& nameOfShadersToMix);

    bool AddComposition(const std::string& shaderName, const std::string& variableName, SpxStreamRemapper* source, std::vector<std::string>& messages);
    void GetMixinBytecode(std::vector<uint32_t>& bytecodeStream);

    virtual void error(const std::string& txt) const;
    bool error(const std::string& txt);
    void copyMessagesTo(std::vector<std::string>& list);

    spv::ExecutionModel GetShadingStageExecutionMode(ShadingStageEnum stage);

private:
    bool SetBytecode(const SpxBytecode& bytecode);
    //bool MergeAllNewShadersFromBytecode(const SpxBytecode& bytecode, std::vector<ShaderClassData*>& listShadersMerged);
    bool MergeShadersIntoBytecode(SpxStreamRemapper& bytecodeToMerge, const std::vector<ShaderToMergeData>& listShadersToMerge, std::string allInstancesPrefixToAdd);
    bool ValidateSpxBytecode();
    bool ValidateHeader();

    unsigned int GetUniqueMergeOperationId();
    static void ResetMergeOperationId();

    bool ProcessOverrideAfterMixingNewShaders(std::vector<ShaderClassData*>& listNewShaders);

    bool RemoveAllUnusedShaders(std::vector<XkslMixerOutputStage>& outputStages);
    bool ApplyCompositionInstancesToBytecode();
    bool CompileMixinForStages(std::vector<XkslMixerOutputStage>& outputStages);
    bool GenerateSpvStageBytecode(ShadingStageEnum stage, std::string entryPointName, FunctionInstruction* entryPoint, SpvBytecode& output);

    FunctionInstruction* GetFunctionForEntryPoint(std::string entryPointName);
    bool RemoveShaderAndAllData(ShaderClassData* shader, std::vector<range_t>& vecStripRanges);

    void ReleaseAllMaps();
    bool BuildAllMaps();
    bool UpdateAllMaps();
    bool UpdateAllObjectsPositionInTheBytecode();
    bool BuildTypesAndConstsHashmap(std::unordered_map<uint32_t, pairIdPos>& mapHashPos);
    bool BuildDeclarationNameMapsAndObjectsDataList(std::vector<ParsedObjectData>& listParsedObjectsData);
    ObjectInstructionBase* CreateAndAddNewObjectFor(ParsedObjectData& parsedData);
    bool DecorateObjects(std::vector<bool>& vectorIdsToDecorate);

    bool UpdateOverridenFunctionMap(std::vector<ShaderClassData*>& listShadersMerged);
    bool UpdateOpFunctionCallTargetsInstructionsToOverridingFunctions();
    bool UpdateFunctionCallsHavingUnresolvedBaseAccessor();

    bool InitDefaultHeader();
    bool BuildAndSetShaderStageHeader(ShadingStageEnum stage, FunctionInstruction* entryFunction, std::string unmangledFunctionName);
    
    bool ComputeShadersLevel();

    void GetShaderFamilyTree(ShaderClassData* shaderFromFamily, std::vector<ShaderClassData*>& shaderFamilyTree);
    void GetShaderChildrenList(ShaderClassData* shader, std::vector<ShaderClassData*>& children);
    static bool GetShadersFullDependencies(SpxStreamRemapper* bytecodeSource, const std::vector<ShaderClassData*> listShaders, std::vector<ShaderClassData*>& fullDependencies);

private:
    //static variable share between all SpxStreamRemapper instances
    static unsigned int currentMergeOperationId;

    SpxRemapperStatusEnum status;

    std::vector<std::string> errorMessages;

    std::unordered_map<spv::Id, std::string> mapDeclarationName;            // delaration name (user defined name) for methods, shaders and variables

    std::vector<ObjectInstructionBase*> listAllObjects;
    std::vector<ShaderClassData*> vecAllShaders;
    std::vector<FunctionInstruction*> vecAllShaderFunctions;  //vec of all functions declared by a shader

private:
    ObjectInstructionBase* GetObjectById(spv::Id id);
    std::string GetDeclarationNameForId(spv::Id id);
    bool GetDeclarationNameForId(spv::Id id, std::string& name);
    ShaderClassData* GetShaderByName(const std::string& name);
    ShaderClassData* GetShaderById(spv::Id id);
    FunctionInstruction* GetFunctionById(spv::Id id);
    TypeInstruction* GetTypeById(spv::Id id);
    VariableInstruction* GetVariableById(spv::Id id);
    VariableInstruction* GetVariableByName(const std::string& name);
    TypeInstruction* GetTypePointingTo(TypeInstruction* targetType);
    VariableInstruction* GetVariablePointingTo(TypeInstruction* targetType);
    HeaderPropertyInstruction* GetHeaderPropertyInstructionByOpCodeAndName(const spv::Op opCode, const std::string& name);
    ShaderComposition* GetCompositionById(spv::Id shaderId, int compositionId);
    FunctionInstruction* GetTargetedFunctionByNameWithinShaderAndItsFamily(ShaderClassData* shader, const std::string& name);
    bool GetAllShaderInstancesForComposition(const ShaderComposition* composition, std::vector<ShaderClassData*>& instances);
    bool GetAllCompositionForEachLoops(std::vector<CompositionForEachLoopData>& vecForEachLoops, int& maxForEachLoopsNestedLevel);

    void stripBytecode(std::vector<range_t>& ranges);

    void CopyInstructionToVector(std::vector<spirword_t>& vec, uint32_t opStart){
        auto start = spv.begin() + opStart;
        auto end = start + asWordCount(opStart);
        vec.insert(vec.end(), start, end);
    }
    void CopyInstructionToVector(std::vector<spirword_t>& vec, uint32_t opStart, uint32_t opEnd) {
        auto start = spv.begin() + opStart;
        auto end = spv.begin() + opEnd;
        vec.insert(vec.end(), start, end);
    }

    friend class XkslMixer;
};

//Contains output stage info (stage + entrypoint), bytecode, plus additionnal data processed by the mixer during compilation
class XkslMixerOutputStage
{
public:
    OutputStageBytecode* outputStage;
    SpxStreamRemapper::FunctionInstruction* entryFunction;

    XkslMixerOutputStage(OutputStageBytecode* outputStage) : outputStage(outputStage) {}
};

}  // namespace xkslang

#endif  // XKSLANG_XKSL_SPX_STREAM_REMAPPER_H__

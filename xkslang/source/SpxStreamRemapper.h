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

static const spv::Id spvUndefinedId = spv::Id(-10001);
static const unsigned int MagicNumber = 0x07230203;
static const unsigned int Version = 0x00010000;
static const unsigned int Revision = 8;
static const unsigned int builderNumber = 0x00080001;

enum class SpxRemapperStatusEnum
{
    WaitingForMixin,
    MixinInProgress,

    //define the order of compilation processes
    MixinBeingCompiled_Initialized,
    MixinBeingCompiled_CompositionInstancesProcessed,
    MixinBeingCompiled_StreamsAndCBuffersAnalysed,
    MixinBeingCompiled_StreamReadyForReschuffling,
    MixinBeingCompiled_StreamReschuffled,
    MixinBeingCompiled_UnusedShaderRemoved,
    MixinBeingCompiled_CBuffersValidated,
    MixinBeingCompiled_ConvertedToSPV,
    MixinBeingCompiled_SPXBytecodeRemoved,
    MixinFinalized
};

class BytecodeChunk
{
public:
    BytecodeChunk() {}
    BytecodeChunk(int insertionPos) : insertionPos(insertionPos), countInstructionsToOverlap(0){}
    BytecodeChunk(int insertionPos, unsigned int countInstructionsToOverlap) : insertionPos(insertionPos), countInstructionsToOverlap(countInstructionsToOverlap) {}

    unsigned int insertionPos;
    std::vector<std::uint32_t> bytecode;
    unsigned int countInstructionsToOverlap;
};

class BytecodeValueToReplace
{
public:
    unsigned int pos;
    uint32_t value;

    BytecodeValueToReplace(unsigned int pos, uint32_t value) : pos(pos), value(value) {}
};

class BytecodePortionToReplace
{
public:
    unsigned int pos;
    std::vector<std::uint32_t> values;

    BytecodePortionToReplace(unsigned int pos) : pos(pos) {}
    void SetNewValues(const std::vector<std::uint32_t>& newValues) {values = newValues;}
};

//In order to update the bytecode, we first store all new codes we want to insert, plus all values we want to change
//Then we'll update the bytecode at once, after all updates have been set
class SpxStreamRemapper;
class BytecodeUpdateController
{
public:

    enum class InsertionConflictBehaviourEnum
    {
        InsertFirst,
        InsertLast,
        ReturnNull,
    };

    std::vector<BytecodeValueToReplace> listAtomicUpdates;
    std::vector<BytecodePortionToReplace> listPortionsToUpdates;
    std::list<BytecodeChunk> listSortedChunksToInsert;

    void SetNewAtomicValueUpdate(unsigned int pos, uint32_t value) { listAtomicUpdates.push_back(BytecodeValueToReplace(pos, value)); }
    BytecodePortionToReplace& SetNewPortionToReplace(unsigned int pos) { listPortionsToUpdates.push_back(BytecodePortionToReplace(pos)); return listPortionsToUpdates.back(); }
    BytecodeChunk* InsertNewBytecodeChunckAt(unsigned int position, InsertionConflictBehaviourEnum conflictBehaviour, unsigned int countBytesToOverlap = 0);
    BytecodeChunk* GetBytecodeChunkAt(unsigned int position);
    unsigned int GetCountBytecodeChuncksToInsert() { return listSortedChunksToInsert.size(); }
};

//==============================================================================================================//
//===========================================  SpxStreamRemapper  ==============================================//
//==============================================================================================================//

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
        unsigned int bytecodeStartPosition;
        unsigned int bytecodeEndPosition;

        ParsedObjectData(){}
        ParsedObjectData(ObjectInstructionTypeEnum kind, spv::Op op, spv::Id resultId, spv::Id typeId, unsigned int startPos, unsigned int endPos)
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
        spv::Id GetTypeId() const { return typeId; }
        
        unsigned int GetBytecodeStartPosition() const {return bytecodeStartPosition;}
        unsigned int GetBytecodeEndPosition() const { return bytecodeEndPosition; }
        void SetBytecodeStartPosition(unsigned int pos) {bytecodeStartPosition = pos;}
        void SetBytecodeRangePositions(unsigned int start, unsigned int end) {bytecodeStartPosition = start; bytecodeEndPosition = end;}

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
        unsigned int bytecodeStartPosition;
        unsigned int bytecodeEndPosition;

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
        ConstInstruction(const ParsedObjectData& parsedData, std::string name, SpxStreamRemapper* source, bool isS32, int valueS32)
            : ObjectInstructionBase(parsedData, name, source), isS32(isS32), valueS32(valueS32) {}
        virtual ~ConstInstruction() {}
        virtual ObjectInstructionBase* CloneBasicData() {
            ConstInstruction* obj = new ConstInstruction(ParsedObjectData(kind, opCode, resultId, typeId, bytecodeStartPosition, bytecodeEndPosition), name, nullptr, isS32, valueS32);
            return obj;
        }

        //if the const is a literalValue signed with 32 bits, we pre-fetch its value
        bool isS32;
        int valueS32;
    };

    class TypeStructMemberArray;
    class ShaderTypeData;
    class TypeInstruction : public ObjectInstructionBase
    {
    public:
        TypeInstruction(const ParsedObjectData& parsedData, std::string name, SpxStreamRemapper* source)
            : ObjectInstructionBase(parsedData, name, source), pointerTo(nullptr), streamStructData(nullptr), connectedShaderTypeData(nullptr),
            isCbuffer(false), cbufferCountMembers(0), cbufferTotalOffset(0), isCbufferUsed(false) {}
        virtual ~TypeInstruction() {}
        virtual ObjectInstructionBase* CloneBasicData() {
            TypeInstruction* obj = new TypeInstruction(ParsedObjectData(kind, opCode, resultId, typeId, bytecodeStartPosition, bytecodeEndPosition), name, nullptr);
            obj->isCbuffer = isCbuffer;
            obj->isCbufferUsed = isCbufferUsed;
            obj->cbufferCountMembers = cbufferCountMembers;
            obj->cbufferTotalOffset = cbufferTotalOffset;
            return obj;
        }

        void SetTypePointed(TypeInstruction* type) { pointerTo = type; }
        TypeInstruction* GetTypePointed() const { return pointerTo; }

    private:
        TypeInstruction* pointerTo;

        //used by some algo to fill the type buffer
        TypeStructMemberArray* streamStructData;
        ShaderTypeData* connectedShaderTypeData;

        //used if the type is a cbuffer struct
        bool isCbuffer;
        bool isCbufferUsed;
        int cbufferCountMembers;
        int cbufferTotalOffset;

        friend class SpxStreamRemapper;
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

        friend class SpxStreamRemapper;
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
            : ObjectInstructionBase(parsedData, name, source), isStatic(false), overrideAttributeState(OverrideAttributeStateEnum::Undefined), overridenBy(nullptr), fullName(name),
            flag1(0), currentPosInBytecode(0), functionProcessingStreamForStage(ShadingStageEnum::Undefined), streamIOStructVariableResultId(0), functionVariablesStartingPosition(0){}
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

        //some variables used to help some algos
        int flag1;
        int currentPosInBytecode;
        unsigned int functionVariablesStartingPosition; //position directly after the function OpLabel instruction (set by some algo needing it)

        //those variables are used when reshuffling stream members
        ShadingStageEnum functionProcessingStreamForStage;  //when a stage calls a function using stream, the stage will reserves the function (another stage calling the function will return an error)
        spv::Id streamIOStructVariableResultId;   //the id of the IO stream struct function parameter

        friend class SpxStreamRemapper;
    };

    class FunctionCallInstructionData
    {
    public:
        FunctionInstruction* functionCalling;
        FunctionInstruction* functionCalled;
        spv::Op opCode;
        unsigned int bytecodePos;

        FunctionCallInstructionData(FunctionInstruction* functionCalling, FunctionInstruction* functionCalled, spv::Op opCode, unsigned int bytecodePos) :
            functionCalling(functionCalling), functionCalled(functionCalled), opCode(opCode), bytecodePos(bytecodePos) {}
    };

    class MemberDecorateData
    {
    public:
        spv::Id typeId;
        unsigned int memberId;
        spv::Decoration decoration;
        unsigned int value;

        MemberDecorateData(spv::Id typeId, unsigned int memberId, spv::Decoration decoration, unsigned int value):
            typeId(typeId), memberId(memberId), decoration(decoration), value(value) {}
    };

    class TypeStructMember
    {
    public:
        TypeStructMember() : structMemberIndex(-1), isStream(false), isStage(false), memberTypeId(spvUndefinedId), tmpRemapToIOIndex(-1), memberPointerFunctionTypeId(-1), offset(-1) {}

        spv::Id structTypeId;             //Id of the struct type containing the member
        int structMemberIndex;            //Id of the member within the struct
        spv::Id memberTypeId;             //Type Id of the member

        bool isStream;
        bool isStage;
        std::string declarationName;
        std::string semantic;

        int memberPointerFunctionTypeId;  //id of the member's pointer type (with Function storage class)
        
        std::vector<unsigned int> listBuiltInSemantics;  //list of builtin semantics set to the member
        int offset;

        //new type and member id used when merging variables
        spv::Id newStructTypeId;
        int newStructMemberIndex;
        int tmpRemapToIOIndex;   //used by some algo

        bool HasSemantic() const { return semantic.size() > 0; }
        bool HasDeclarationName() const { return declarationName.size() > 0; }

        const std::string& GetSemanticOrDeclarationName() const { return HasSemantic()? semantic: declarationName; }
        const std::string& GetDeclarationNameOrSemantic() const { return HasDeclarationName() ? declarationName : semantic; }
        std::string GetNameWithSemantic() const {
            if (HasSemantic()) return declarationName + std::string(": ") + semantic;
            return declarationName;
        }
    };

    class TypeStructMemberArray
    {
    public:
        std::vector<TypeStructMember> members;
        std::vector<spv::Id> mapIndexesWithConstValueId;

        //Id of the type pointing to the list
        spv::Id structTypeId;
        spv::Id structPointerTypeId;
        spv::Id structVariableTypeId;

        unsigned int tmpTargetedBytecodePosition;
        unsigned int cbufferTotalOffset;

        TypeStructMemberArray() : structTypeId(spvUndefinedId), structPointerTypeId(spvUndefinedId), structVariableTypeId(spvUndefinedId), tmpTargetedBytecodePosition(0), cbufferTotalOffset(0){}

        unsigned int countMembers() { return members.size(); }
    };

    //This is a type declared by a shader: we store the type definition, plus the variable and pointer to access it
    class ShaderTypeData
    {
    public:
        TypeInstruction* type;
        TypeInstruction* pointerToType;
        VariableInstruction* variable;

        TypeStructMemberArray* cbufferMembersData;  //data used temporarly when processing cbuffer
        int tmpFlag;

        ShaderTypeData(TypeInstruction* type, TypeInstruction* pointerToType, VariableInstruction* variable) : type(type), pointerToType(pointerToType), variable(variable),
            cbufferMembersData(nullptr), tmpFlag(0){}
        virtual ~ShaderTypeData(){}

        bool isCBufferType() { return type->isCbuffer; }
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
        std::vector<std::uint32_t> foreachDuplicatedBytecode;

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
        ShaderTypeData* GetShaderTypeDataForType(TypeInstruction* type) {
            for (unsigned int i = 0; i<shaderTypesList.size(); ++i) if (shaderTypesList[i]->type == type) return shaderTypesList[i];
            return nullptr;
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
    void GetMixinBytecode(std::vector<std::uint32_t>& bytecodeStream);

    static void GetStagesPipeline(std::vector<ShadingStageEnum>& pipeline);

    bool error(const std::string& txt);
    void copyMessagesTo(std::vector<std::string>& list);

    spv::ExecutionModel GetShadingStageExecutionMode(ShadingStageEnum stage);

private:
    bool SetBytecode(const SpxBytecode& bytecode);
    bool MergeShadersIntoBytecode(SpxStreamRemapper& bytecodeToMerge, const std::vector<ShaderToMergeData>& listShadersToMerge, std::string allInstancesPrefixToAdd);

    bool ValidateSpxBytecodeAndData();
    bool ValidateHeader();
    bool ProcessBytecodeAndDataSanityCheck();

    unsigned int GetUniqueMergeOperationId();
    static void ResetMergeOperationId();
    bool ApplyBytecodeUpdateController(const BytecodeUpdateController& bytecodeUpdateController);
    bool ProcessOverrideAfterMixingNewShaders(std::vector<ShaderClassData*>& listNewShaders);

    bool ApplyCompositionInstancesToBytecode();
    bool InitializeCompilationProcess(std::vector<XkslMixerOutputStage>& outputStages);
    bool MergeStreamMembers(TypeStructMemberArray& globalListOfMergedStreamVariables);
    bool AnalyseStreamsAndCBuffersAccessesForOutputStages(std::vector<XkslMixerOutputStage>& outputStages, TypeStructMemberArray& globalListOfMergedStreamVariables);
    bool ValidateStagesStreamMembersFlow(std::vector<XkslMixerOutputStage>& outputStages, TypeStructMemberArray& globalListOfMergedStreamVariables);
    bool ReshuffleStreamVariables(std::vector<XkslMixerOutputStage>& outputStages, TypeStructMemberArray& globalListOfMergedStreamVariables);
    bool RemoveAllUnusedShaders(std::vector<XkslMixerOutputStage>& outputStages);
    bool RemoveAndConvertSPXExtensions();
    bool GenerateBytecodeForAllStages(std::vector<XkslMixerOutputStage>& outputStages);
    bool ProcessCBuffers(std::vector<XkslMixerOutputStage>& outputStages);
    
    bool GetStructTypeMembersTypeIdList(TypeInstruction* structType, std::vector<spv::Id>& membersTypeList);
    bool GetFunctionLabelAndReturnInstructionsPosition(FunctionInstruction* function, unsigned int& labelPos, unsigned int& returnPos);
    bool GetFunctionLabelInstructionPosition(FunctionInstruction* function, unsigned int& labelPos);
    FunctionInstruction* GetShaderFunctionForEntryPoint(std::string entryPointName);
    bool RemoveShaderFromBytecodeAndData(ShaderClassData* shader, std::vector<range_t>& vecStripRanges);
    bool RemoveShaderTypeFromBytecodeAndData(ShaderTypeData* shaderType, std::vector<range_t>& vecStripRanges);

    void ReleaseAllMaps();
    bool BuildAllMaps();
    bool UpdateAllMaps();
    bool UpdateAllObjectsPositionInTheBytecode();
    bool BuildTypesAndConstsHashmap(std::unordered_map<std::uint32_t, pairIdPos>& mapHashPos);
    bool BuildDeclarationNameMapsAndObjectsDataList(std::vector<ParsedObjectData>& listParsedObjectsData);
    ObjectInstructionBase* CreateAndAddNewObjectFor(ParsedObjectData& parsedData);
    bool DecorateObjects(std::vector<bool>& vectorIdsToDecorate);

    bool UpdateOverridenFunctionMap(std::vector<ShaderClassData*>& listShadersMerged);
    bool UpdateOpFunctionCallTargetsInstructionsToOverridingFunctions();
    bool UpdateFunctionCallsHavingUnresolvedBaseAccessor();

    bool InitDefaultHeader();
    bool ComputeShadersLevel();

    void GetShaderFamilyTree(ShaderClassData* shaderFromFamily, std::vector<ShaderClassData*>& shaderFamilyTree);
    void GetShaderChildrenList(ShaderClassData* shader, std::vector<ShaderClassData*>& children);
    static bool GetShadersFullDependencies(SpxStreamRemapper* bytecodeSource, const std::vector<ShaderClassData*>& listShaders, std::vector<ShaderClassData*>& fullDependencies);

    static bool parseInstruction(const std::vector<std::uint32_t>& bytecode, unsigned int word, spv::Op& opCode, unsigned int& wordCount, spv::Id& type, spv::Id& result, std::vector<spv::Id>& listIds, std::string& errorMsg);
    bool parseInstruction(unsigned int word, spv::Op& opCode, unsigned int& wordCount, spv::Id& type, spv::Id& result, std::vector<spv::Id>& listIds);
    bool flagAllIdsFromInstruction(unsigned int word, spv::Op& opCode, unsigned int& wordCount, std::vector<bool>& listIdsUsed);
    static bool remapAllInstructionIds(std::vector<std::uint32_t>& bytecode, unsigned int word, unsigned int& wordCount, const std::vector<spv::Id>& remapTable, std::string& errorMsg);
    static bool remapAllIds(std::vector<std::uint32_t>& bytecode, unsigned int begin, unsigned int end, const std::vector<spv::Id>& remapTable, std::string& errorMsg);
    bool remapAllIds(std::vector<std::uint32_t>& bytecode, unsigned int begin, unsigned int end, const std::vector<spv::Id>& remapTable);

    bool CleanAndSetStageBytecode(XkslMixerOutputStage& stage, std::vector<spv::Id>& listCBufferIds);

private:
    //static variable share between all SpxStreamRemapper instances
    static unsigned int currentMergeOperationId;

    SpxRemapperStatusEnum status;

    std::vector<std::string> errorMessages;

    std::unordered_map<spv::Id, std::string> mapDeclarationName;            // delaration name (user defined name) for methods, shaders and variables

    std::vector<ObjectInstructionBase*> listAllObjects;
    std::vector<ShaderClassData*> vecAllShaders;
    std::vector<FunctionInstruction*> vecAllFunctions;  //vec of all functions

private:
    ObjectInstructionBase* GetObjectById(spv::Id id);
    std::string GetDeclarationNameForId(spv::Id id);
    bool GetDeclarationNameForId(spv::Id id, std::string& name);
    ShaderClassData* GetShaderByName(const std::string& name);
    ShaderClassData* GetShaderById(spv::Id id);
    FunctionInstruction* GetFunctionById(spv::Id id);
    TypeInstruction* GetTypeById(spv::Id id);
    ConstInstruction* GetConstById(spv::Id id);
    VariableInstruction* GetVariableById(spv::Id id);
    VariableInstruction* GetVariableByName(const std::string& name);
    TypeInstruction* GetTypePointingTo(TypeInstruction* targetType);
    VariableInstruction* GetVariablePointingTo(TypeInstruction* targetType);
    HeaderPropertyInstruction* GetHeaderPropertyInstructionByOpCodeAndName(const spv::Op opCode, const std::string& name);
    ShaderComposition* GetCompositionById(spv::Id shaderId, int compositionId);
    FunctionInstruction* GetTargetedFunctionByNameWithinShaderAndItsFamily(ShaderClassData* shader, const std::string& name);
    bool GetListAllFunctionCallInstructions(std::vector<FunctionCallInstructionData>& listFunctionCallInstructions);
    bool GetAllShaderInstancesForComposition(const ShaderComposition* composition, std::vector<ShaderClassData*>& instances);
    bool GetAllCompositionForEachLoops(std::vector<CompositionForEachLoopData>& vecForEachLoops, int& maxForEachLoopsNestedLevel);
    
    void stripBytecode(std::vector<range_t>& ranges);

    void CopyInstructionToVector(std::vector<std::uint32_t>& vec, int opStart){
        auto start = spv.begin() + opStart;
        auto end = start + asWordCount(opStart);
        vec.insert(vec.end(), start, end);
    }
    void CopyInstructionToVector(std::vector<std::uint32_t>& vec, int opStart, int opEnd) {
        auto start = spv.begin() + opStart;
        auto end = spv.begin() + opEnd;
        vec.insert(vec.end(), start, end);
    }

    friend class XkslMixer;
};

class MemberAccessDetails
{
public:
    enum class MemberFirstAccessEnum
    {
        Undefined = 0,
        ReadFirst = 1 << 0,
        WriteFirst = 1 << 1,
    };

    enum class MemberIOEnum
    {
        Undefined = 0,
        Input = 1 << 0,
        Output = 1 << 1,
        PassThrough = 1 << 2,
    };

public:
    MemberFirstAccessEnum firstAccess;
    int stageIONeeded;

    MemberAccessDetails() : firstAccess(MemberFirstAccessEnum::Undefined), stageIONeeded(0){}
    
    void SetFirstAccessRead() {
        if (firstAccess == MemberFirstAccessEnum::Undefined) firstAccess = MemberFirstAccessEnum::ReadFirst;
    }
    void SetFirstAccessWrite() {
        if (firstAccess == MemberFirstAccessEnum::Undefined) firstAccess = MemberFirstAccessEnum::WriteFirst;
    }

    void SetAsInput() {
        stageIONeeded |= ((int)MemberIOEnum::Input);
    }
    void SetAsOutput() {
        stageIONeeded |= ((int)MemberIOEnum::Output);
    }
    void SetAsPassThrough() {
        stageIONeeded |= (((int)MemberIOEnum::Input) + ((int)MemberIOEnum::Output) + ((int)MemberIOEnum::PassThrough));
    }

    bool IsNeededAsInput() { return (stageIONeeded & ((int)MemberIOEnum::Input)) != 0; }
    bool IsNeededAsOutput() { return (stageIONeeded & ((int)MemberIOEnum::Output)) != 0; }
    bool IsInputOnly() { return (stageIONeeded == (int)MemberIOEnum::Input); }

    //bool HasWriteAccess() { return (accessesNeeded & ((int)MemberAccessDetailsEnum::Write)); }
    bool IsWriteFirstStream() { return firstAccess == MemberFirstAccessEnum::WriteFirst; }
    bool IsReadFirstStream() { return firstAccess == MemberFirstAccessEnum::ReadFirst; }
    bool IsBeingAccessed() { return firstAccess != MemberFirstAccessEnum::Undefined; }
};

//Contains output stage info (stage + entrypoint), bytecode, plus additionnal data processed by the mixer during compilation
class XkslMixerOutputStage
{
public:
    OutputStageBytecode* outputStage; //set by the user

    SpxStreamRemapper::FunctionInstruction* entryFunction;  //set when initializing the compilation process

    std::vector<MemberAccessDetails> listStreamVariablesAccessed;  //list of stream variables accessed by the stage, set by AnalyseStreams method
    std::vector<SpxStreamRemapper::FunctionInstruction*> listFunctionsCalledAndAccessingStreamMembers;  //list of functions called by the stage, accessing some stream members
    std::vector<SpxStreamRemapper::ShaderTypeData*> listCBuffersAccessed;

    XkslMixerOutputStage(OutputStageBytecode* outputStage) : outputStage(outputStage) {}
};

}  // namespace xkslang

#endif  // XKSLANG_XKSL_SPX_STREAM_REMAPPER_H__

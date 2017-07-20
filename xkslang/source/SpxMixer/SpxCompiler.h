//
// Copyright (C) 

#ifndef XKSLANG_XKSL_SPX_STREAM_REMAPPER_H__
#define XKSLANG_XKSL_SPX_STREAM_REMAPPER_H__

#include <string>
#include <vector>
#include <stack>
#include <sstream>

#include "SPIRV/spvIR.h"
#include "SPIRV/SPVRemapper.h"

#include "../Common/xkslangDefine.h"
#include "../Common/SpxBytecode.h"
#include "OutputStageBytecode.h"
#include "EffectReflection.h"

namespace xkslang
{

static const spv::Id spvUndefinedId = 0;
static const unsigned int MagicNumber = 0x07230203;
static const unsigned int Version = 0x00010000;
static const unsigned int Revision = 8;
static const unsigned int builderNumber = 0x00080001;

//Todo: To be clean and parameterized:
static const bool compilerSettings_useStd140Rules = true;  //set as true for now (to be consistent with glslang)
static const int compilerSettings_baseAlignmentVec4Std140 = 16;
static const int compilerSettings_maxMatrixSize = 4;
static const int compilerSettings_maxArraySize = 2048;  //safety marge
static const int compilerSettings_maxStructMembers = 512;  //safety marge

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
    MixinBeingCompiled_CBuffersValidated,
    MixinBeingCompiled_ResourcesProcessed,
    MixinBeingCompiled_UnusedStuffRemoved,
    MixinBeingCompiled_Finalized,
    MixinFinalized
};

enum class BytecodeChunkInsertionTypeEnum
{
    InsertBeforeInstruction = 0,
    InsertWithinInstruction = 1,
    InsertAfterInstruction = 2,
};

class BytecodeChunk
{
public:
    unsigned int instructionPos;
    BytecodeChunkInsertionTypeEnum insertionType;
    unsigned int insertionPos;
    
    std::vector<std::uint32_t> bytecode;

    BytecodeChunk() {}
    BytecodeChunk(int instructionPos, BytecodeChunkInsertionTypeEnum insertionType, int insertionPos) : instructionPos(instructionPos), insertionType(insertionType), insertionPos(insertionPos) {}
};

class BytecodeValueToReplace
{
public:
    unsigned int pos;
    uint32_t value;

    BytecodeValueToReplace(unsigned int pos, uint32_t value) : pos(pos), value(value) {}
};

class BytecodePortionToRemove
{
public:
    unsigned int position;
    unsigned int count;

    BytecodePortionToRemove(unsigned int position, unsigned int count) : position(position), count(count) {}
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
class SpxCompiler;
class BytecodeUpdateController
{
private:
    std::vector<BytecodeValueToReplace> listAtomicUpdates;
    std::vector<BytecodePortionToReplace> listPortionsToUpdates;
    std::vector<BytecodePortionToRemove> listSortedPortionsToRemove;
    std::list<BytecodeChunk> listSortedChunksToInsert;

    friend class SpxCompiler;
};

//==============================================================================================================//
//===============================================  SpxCompiler  ================================================//
//==============================================================================================================//

class XkslMixerOutputStage;
class SpxCompiler : public spv::spirvbin_t
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
        ObjectInstructionBase(const ParsedObjectData& parsedData, std::string name, SpxCompiler* source)
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
        SpxCompiler* bytecodeSource;

        //those fields can change when we mix bytecodes
        unsigned int bytecodeStartPosition;
        unsigned int bytecodeEndPosition;

        friend class SpxCompiler;
    };

    class HeaderPropertyInstruction : public ObjectInstructionBase
    {
    public:
        HeaderPropertyInstruction(const ParsedObjectData& parsedData, std::string name, SpxCompiler* source)
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
        ConstInstruction(const ParsedObjectData& parsedData, std::string name, SpxCompiler* source, bool isS32, int valueS32)
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

    //extra data recorded when a shaderType defines a cbuffer
    class TypeStructMemberArray;
    class ShaderTypeData;
    class TypeInstruction;
    class VariableInstruction;

    class VariableData
    {
    public:
        std::string variableRawName;
        std::string variableKeyName;
		std::string semanticName;
		int semanticIndex;
        TypeReflectionDescription variableTypeReflection;

        VariableData() : semanticIndex(0) {}

        bool hasKeyName() { return variableKeyName.size() > 0; }
        bool hasRawName() { return variableRawName.size() > 0; }
        void SetVariableRawName(const std::string& rawName) { variableRawName = rawName;}
        void SetVariableKeyName(const std::string& keyName) { variableKeyName = keyName; }
    };

    class CBufferTypeData
    {
    public:
        //used if the type is a cbuffer struct
        spv::Id cbufferTypeId;
        bool isDefine;
        bool isStage;
        int cbufferCountMembers;
        std::string cbufferName;
        std::string shaderOwnerName;  //name of the shader owning the cbuffer (for stage cbuffer, instantiated cbuffers will keep the name of the original shader class)

        bool isUsed;
        
        //temporary data used when processing the cbuffers
        TypeStructMemberArray* cbufferMembersData;
        ShaderTypeData* correspondingShaderType;
        int posOpNameType;
        int posOpNameVariable;
        TypeInstruction* cbufferTypeObject;
        TypeInstruction* cbufferPointerTypeObject;
        VariableInstruction* cbufferVariableTypeObject;
        int tmpFlag;

        CBufferTypeData(spv::Id cbufferTypeId, std::string shaderOwnerName, std::string cbufferName, bool isDefine, bool isStage, int cbufferCountMembers) :
            cbufferTypeId(cbufferTypeId), shaderOwnerName(shaderOwnerName), cbufferName(cbufferName), isDefine(isDefine), isStage(isStage),
            cbufferCountMembers(cbufferCountMembers), isUsed(false), cbufferMembersData(nullptr), correspondingShaderType(nullptr), posOpNameType(0), posOpNameVariable(0),
            cbufferTypeObject(nullptr), cbufferPointerTypeObject(nullptr), cbufferVariableTypeObject(nullptr), tmpFlag(0){}
        virtual ~CBufferTypeData() { if (cbufferMembersData != nullptr) delete cbufferMembersData; }

        virtual CBufferTypeData* Clone() {
            CBufferTypeData* cbufferData = new CBufferTypeData(cbufferTypeId, shaderOwnerName, cbufferName, isDefine, isStage, cbufferCountMembers);
            cbufferData->isUsed = isUsed;
            return cbufferData;
        }
    };

    class TypeStructMemberArray;
    class ShaderTypeData;
    class TypeInstruction : public ObjectInstructionBase
    {
    public:
        TypeInstruction(const ParsedObjectData& parsedData, std::string name, SpxCompiler* source)
            : ObjectInstructionBase(parsedData, name, source),
            arrayStride(0), pointerTo(nullptr), streamStructData(nullptr), connectedShaderTypeData(nullptr), isCBuffer(false), cbufferData(nullptr) {}
        virtual ~TypeInstruction() {
            if (cbufferData != nullptr) delete cbufferData;
        }
        virtual ObjectInstructionBase* CloneBasicData() {
            TypeInstruction* obj = new TypeInstruction(ParsedObjectData(kind, opCode, resultId, typeId, bytecodeStartPosition, bytecodeEndPosition), name, nullptr);
            obj->arrayStride = arrayStride;
            obj->isCBuffer = isCBuffer;
            if (cbufferData != nullptr) obj->cbufferData = cbufferData->Clone();
            return obj;
        }

        void SetTypePointed(TypeInstruction* type) { pointerTo = type; }
        TypeInstruction* GetTypePointed() const { return pointerTo; }

        void SetAsCBuffer() {isCBuffer = true;}
        void SetCBufferData(CBufferTypeData* data) {
            if (cbufferData != nullptr) delete cbufferData;
            cbufferData = data;
        }
        CBufferTypeData* GetCBufferData() { return cbufferData; }

    private:
        int arrayStride; //set if the type is an array
        TypeInstruction* pointerTo;  //set if the type is a pointer

        //used by some algo to fill the type buffer
        TypeStructMemberArray* streamStructData;
        ShaderTypeData* connectedShaderTypeData;

        bool IsCBuffer(){ return isCBuffer; }

        //used if the type is a cbuffer struct
        bool isCBuffer;
        CBufferTypeData* cbufferData;

        friend class SpxCompiler;
    };

    class VariableInstruction : public ObjectInstructionBase
    {
    public:
        VariableInstruction(const ParsedObjectData& parsedData, std::string name, SpxCompiler* source)
            : ObjectInstructionBase(parsedData, name, source), variableTo(nullptr), variableData(nullptr){}

        virtual ~VariableInstruction();
        virtual ObjectInstructionBase* CloneBasicData() {
            VariableInstruction* obj = new VariableInstruction(ParsedObjectData(kind, opCode, resultId, typeId, bytecodeStartPosition, bytecodeEndPosition), name, nullptr);
            return obj;
        }

        void SetTypePointed(TypeInstruction* type) { variableTo = type; }
        TypeInstruction* GetTypePointed() const { return variableTo; }

    private:
        TypeInstruction* variableTo;
        int tmpFlag;

        VariableData* variableData; //temporarily used by some algo

        friend class SpxCompiler;
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

        FunctionInstruction(const ParsedObjectData& parsedData, std::string name, SpxCompiler* source)
            : ObjectInstructionBase(parsedData, name, source), isStatic(false), overrideAttributeState(OverrideAttributeStateEnum::Undefined), overridenBy(nullptr), fullName(name),
            flag1(0), currentPosInBytecode(0), functionProcessingStreamForStage(ShadingStageEnum::Undefined),
            streamIOStructVariableResultId(0), streamIOStructConstantCompositeId(0), streamOutputStructVariableResultId(0), streamOutputStructConstantCompositeId(0) {}
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

        //those variables are used when reshuffling stream members
        ShadingStageEnum functionProcessingStreamForStage;  //when a stage calls a function using stream, the stage will reserves the function (another stage calling the function will return an error)
        spv::Id streamIOStructVariableResultId;        //the id of the IO stream struct function parameter
        spv::Id streamIOStructConstantCompositeId;     //the id of the IO stream struct constant composite
        spv::Id streamOutputStructVariableResultId;    //the id of the Output stream struct function parameter
        spv::Id streamOutputStructConstantCompositeId; //the id of the output stream struct constant composite

        friend class SpxCompiler;
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

    //This class is a bit messy as it contains data for different member type (stream, cbuffer) and some parameters used for some specific algorithm or some specific cases only
    class TypeStructMember
    {
    public:
        TypeStructMember() : structTypeId(spvUndefinedId), structMemberIndex(-1),
            isStream(false), isStage(false), memberTypeId(spvUndefinedId), memberType(nullptr), memberDefaultConstantTypeId(spvUndefinedId),
            memberSize(-1), memberAlignment(-1), memberOffset(-1), matrixLayoutDecoration(-1), matrixStride(0), arrayStride(0),
            newStructTypeId(0), newStructVariableAccessTypeId(0), newStructMemberIndex(-1), tmpRemapToIOIndex(-1), memberPointerFunctionTypeId(-1),
            variableAccessTypeId(0), memberTypePointerInputId(0), memberTypePointerOutputId(0), memberStageInputVariableId(0), memberStageOutputVariableId(0),
            isUsed(false) {}

        spv::Id structTypeId;             //Id of the struct type containing the member
        int structMemberIndex;            //Id of the member within the struct
        spv::Id memberTypeId;             //Type Id of the member
        spv::Id memberDefaultConstantTypeId;       
        TypeInstruction* memberType;      //member type

        bool isStream;
        bool isStage;
        std::string declarationName;
        std::string semantic;
        std::string attribute;
        std::string linkName;  //the user specified a link value for the member

        //some stream member properties
        int memberPointerFunctionTypeId;  //id of the member's pointer type (TypePointer with Function storage class)
        //std::vector<unsigned int> listBuiltInSemantics; //list of builtin semantics set to the member
        spv::Id memberTypePointerInputId;      //if the member is a stage input, and if we use an input variable for the member
        spv::Id memberTypePointerOutputId;     //if the member is a stage output, and if we use an output variable for the member
        spv::Id memberStageInputVariableId;
        spv::Id memberStageOutputVariableId;

        //some cbuffer members properties
        int memberSize;
        int memberAlignment;
        int memberOffset;
        int matrixLayoutDecoration; //a matrix layout is either RowMajor (DecorationRowMajor) or ColMajor (DecorationColMajor), or -1 if undefined
        int matrixStride;           //set the stride for matrix types (or array of matrices)
        int arrayStride;            //set the stride for array types

        //std::vector<unsigned int> listMemberDecoration;  //extra decorate properties set with the member (for example: RowMajor, MatrixStride, ... set for cbuffer members)

        bool isUsed; //in some case we need to know which members are actually used or not
        bool isResourceType;

        std::string shaderOwnerName;  //name of the shader owning the cbuffer member (for stage cbuffer, instantiated cbuffers will keep the name of the original shader class)

        //temporary variables used to remap members to others
        spv::Id newStructTypeId;
        spv::Id newStructVariableAccessTypeId;
        int newStructMemberIndex;
        int tmpRemapToIOIndex;   //used by some algo
        spv::Id variableAccessTypeId; //in some case (resources) the member is not within a struct...

        //====================================================
        bool HasSemantic() const { return semantic.size() > 0; }
        bool HasDeclarationName() const { return declarationName.size() > 0; }
        bool HasAttribute() const { return attribute.size() > 0; }
        bool HasLinkName() const { return linkName.size() > 0; }

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
        std::string declarationName;

        TypeStructMemberArray() : structTypeId(spvUndefinedId), structPointerTypeId(spvUndefinedId), structVariableTypeId(spvUndefinedId), tmpTargetedBytecodePosition(0){}

        unsigned int countMembers() { return (unsigned int)members.size(); }
    };

    //This is a type declared by a shader: we store the type definition, plus the variable and pointer to access it
    class ShaderTypeData
    {
    public:
        TypeInstruction* type;
        TypeInstruction* pointerToType;
        VariableInstruction* variable;
        ShaderClassData* shaderOwner;

        //data used temporarly when processing cbuffers
        int tmpFlag;

        //===========================================================
        ShaderTypeData(ShaderClassData* shaderOwner, TypeInstruction* type, TypeInstruction* pointerToType, VariableInstruction* variable) :
            shaderOwner(shaderOwner), type(type), pointerToType(pointerToType), variable(variable), tmpFlag(0){}
        virtual ~ShaderTypeData(){}

        bool isCBufferType() { return type->IsCBuffer(); }
        CBufferTypeData* GetCBufferData() { return type->GetCBufferData(); }
    };

    class ShaderComposition
    {
    public:
        int compositionShaderId;
        ShaderClassData* compositionShaderOwner;
        ShaderClassData* shaderType;
        std::string variableName;
        bool isStage;
        bool isArray;
        int countInstances;
        ShaderComposition* tmpClonedComposition;

        //If we merge a composition into a bytecode, this composition can be overriden by another one
        //A composition gets overriden if:
        // - it is set as stage
        // - it has the same shaderOwner name (original base name) and same shaderType (original base name)
        // - it has the same variable name
        //When a composition is overriden: everytime we instantiate it, we instantiate the overridding composition instead
        //plus, when solving the composition function call, we will call the overriding composition instead
        ShaderComposition* overridenBy;

        const std::string& GetVariableName() const {return variableName;}
        std::string GetShaderOwnerAndVariableName() const { return std::string((compositionShaderOwner == nullptr? "---": compositionShaderOwner->name)) + "." + variableName; }

        //ShaderClassData* instantiatedShader;  //resulting shader instance from the composition

        ShaderComposition(int compositionShaderId, ShaderClassData* compositionShaderOwner, ShaderClassData* shaderType,
            const std::string& variableName, bool isStage, bool isArray, int countInstances)
                :compositionShaderId(compositionShaderId), compositionShaderOwner(compositionShaderOwner), shaderType(shaderType),
                variableName(variableName), isStage(isStage), isArray(isArray), countInstances(countInstances), tmpClonedComposition(nullptr), overridenBy(nullptr){}

    private:
        friend class SpxCompiler;
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
            StaticAccess = 1 << 0,
            Other = 1 << 1,
        };

    public:
        ShaderClassData(const ParsedObjectData& parsedData, std::string name, SpxCompiler* source)
            : ObjectInstructionBase(parsedData, name, source), level(-1), countGenerics(0), flag(0), flag1(0), tmpClonedShader(nullptr) {
        }
        virtual ~ShaderClassData() {
            for (auto it = shaderTypesList.begin(); it != shaderTypesList.end(); it++) delete (*it);
        }
        virtual ObjectInstructionBase* CloneBasicData() {
            ShaderClassData* obj = new ShaderClassData(ParsedObjectData(kind, opCode, resultId, typeId, bytecodeStartPosition, bytecodeEndPosition), name, nullptr);
            obj->level = level;
            obj->countGenerics = countGenerics;
            obj->shaderOriginalTypeName = shaderOriginalTypeName;
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
        unsigned int GetCountFunctions() { return (unsigned int)functionsList.size(); }

        void AddShaderType(ShaderTypeData* type) { shaderTypesList.push_back(type); }
        bool HasType(TypeInstruction* type) {
            for (unsigned int i = 0; i<shaderTypesList.size(); ++i) if (shaderTypesList[i]->type == type) return true;
            return false;
        }
        ShaderTypeData* GetShaderTypeDataForType(TypeInstruction* type) {
            for (unsigned int i = 0; i<shaderTypesList.size(); ++i) if (shaderTypesList[i]->type == type) return shaderTypesList[i];
            return nullptr;
        }
        ShaderTypeData* GetShaderTypeDataForVariable(VariableInstruction* variable) {
            for (unsigned int i = 0; i<shaderTypesList.size(); ++i) if (shaderTypesList[i]->variable == variable) return shaderTypesList[i];
            return nullptr;
        }

        unsigned int GetCountShaderComposition() { return (unsigned int)compositionsList.size(); }
        void AddComposition(const ShaderComposition& composition) { compositionsList.push_back(composition); }
        ShaderComposition* GetShaderCompositionById(int compositionId) {
            if (compositionId<0 || compositionId>= (int)compositionsList.size()) return nullptr;
            return &(compositionsList[compositionId]);
        }
        ShaderComposition* GetShaderCompositionByName(const std::string& variableName) {
            for (unsigned int i=0; i<compositionsList.size(); ++i)
                if (compositionsList[i].variableName == variableName) return &(compositionsList[i]);
            return nullptr;
        }

        std::string GetShaderFullName() { return GetName(); }
        std::string GetShaderOriginalTypeName() { return shaderOriginalTypeName; }

    public:
        int level;
        int countGenerics;
        std::string shaderOriginalTypeName;   //when we instantiate a shader (a composition for example), we keep the original shaderName in this field
        std::vector<ShaderClassData*> parentsList;
        std::vector<ShaderTypeData*> shaderTypesList;
        std::vector<FunctionInstruction*> functionsList;

        std::vector<ShaderComposition> compositionsList;

        //std::string instanceOriginalShaderName;  //when a shader is instantiated (for composition), we store its original shader name

    private:
        //When merging/duplicating a shader into a bytecode, this field will hold a temporary reference to its resulting, cloned shader 
        ShaderClassData* tmpClonedShader;

        //some params to simplify some algos
        int flag, flag1;
        ShaderDependencyTypeEnum dependencyType;  //dependency type (static or not), set by GetShadersFullDependencies algorithm.

    friend class SpxCompiler;
    };

    class ShaderToMergeData
    {
    public:
        ShaderClassData* shader;
        bool instantiateShader;

        ShaderToMergeData(ShaderClassData* shaderToMerge): shader(shaderToMerge), instantiateShader(false) {}
        ShaderToMergeData(ShaderClassData* shader, bool instantiateShader) : shader(shader), instantiateShader(instantiateShader) {}
    };

    class OutputStageEntryPoint
    {
    public:
        ShadingStageEnum stage;
        FunctionInstruction* entryFunction;

        OutputStageEntryPoint() : stage(ShadingStageEnum::Undefined), entryFunction(nullptr) {}
        OutputStageEntryPoint(ShadingStageEnum stage, FunctionInstruction* entryFunction) : stage(stage), entryFunction(entryFunction) {}
    };
    //============================================================================================================================================
    //============================================================================================================================================

public:
    SpxCompiler(int verbose = 0);
    virtual ~SpxCompiler();

    SpxCompiler* Clone();

    void CopyMixinBytecode(std::vector<std::uint32_t>& bytecodeStream);
    const std::vector<std::uint32_t>& GetMixinBytecode();
    static void GetStagesPipeline(std::vector<ShadingStageEnum>& pipeline);

    //bool MixWithBytecode(const SpxBytecode& bytecode);
    bool MixWithShadersFromBytecode(const SpxBytecode& sourceBytecode, const std::vector<std::string>& nameOfShadersToMix);

    bool GetListAllCompositions(std::vector<ShaderComposition*>& vecCompositions);
    bool GetListAllCompositionsInfo(std::vector<ShaderCompositionInfo>& vecCompositionsInfo);
    bool AddComposition(const std::string& shaderName, const std::string& variableName, SpxCompiler* source);
    bool InsertNewCompositionInstanceForComposition(ShaderComposition* compositionTarget, spv::Id instanceShaderId);
    ShaderComposition* GetShaderCompositionForVariableName(ShaderClassData* shader, const std::string& variableName, bool lookInParentShaders);
    bool GetAllCompositionsForVariableName(ShaderClassData* shader, const std::string& variableName, bool lookInParentShaders, std::vector<ShaderComposition*>& listCompositions);
    bool CheckIfAnyNewCompositionGetOverridenByExistingOnes(std::vector<ShaderClassData*>& listMergedShaders);
    bool CheckIfTheCompositionGetOverridenByAnExistingStageComposition(ShaderComposition* newStagedComposition, std::vector<ShaderComposition*>& listStagedCompositionsPotentiallyOverriding);
    bool GetAllShaderInstancesForComposition(const ShaderComposition* composition, std::vector<ShaderClassData*>& instances);
    bool GetAllCompositionForEachLoops(std::vector<CompositionForEachLoopData>& vecForEachLoops, int& maxForEachLoopsNestedLevel);

    bool error(const std::string& txt);
    static bool error(std::vector<std::string>& errorMsgs, const std::string& txt);
    void copyMessagesTo(std::vector<std::string>& list);

    spv::ExecutionModel GetExecutionModeForShadingStage(ShadingStageEnum stage);
    ShadingStageEnum GetShadingStageForExecutionMode(spv::ExecutionModel model);

    static bool ProcessBytecodeSanityCheck(const std::vector<uint32_t>& bytecode, std::vector<std::string>& errorMsgs);

private:
    bool SetBytecode(const SpvBytecode& bytecode);
    bool SetBytecode(const std::vector<std::uint32_t>& bytecode);
    bool MergeShadersIntoBytecode(SpxCompiler& bytecodeToMerge, const std::vector<ShaderToMergeData>& listShadersToMerge, std::string allInstancesPrefixToAdd);

    //validate a member name (for example Shader<8>_var will return Shader_8__var)
    std::string getRawNameFromKeyName(const std::string& keyName);
    unsigned int GetUniqueMergeOperationId();
    static void ResetMergeOperationId();

    //bool GetReflectionTypeForMember(TypeStructMember& member, TypeReflectionDescription& typeReflection);
    //bool GetReflectionTypeFor(TypeInstruction* memberType, TypeReflectionDescription& typeReflection, const std::string& attribute, int iterationNum);

    bool ValidateSpxBytecodeAndData();
    bool ValidateHeader();
    bool ProcessBytecodeAndDataSanityCheck();

    bool GetBytecodeReflectionData(EffectReflection& effectReflection);
	bool GetAllCBufferAndResourcesBindingsReflectionDataFromBytecode(EffectReflection& effectReflection, std::vector<OutputStageEntryPoint>& listEntryPoints);
	bool GetInputAttributesFromBytecode(EffectReflection& effectReflection, std::vector<OutputStageEntryPoint>& listEntryPoints);

    bool ProcessOverrideAfterMixingNewShaders(std::vector<ShaderClassData*>& listNewShaders);

    bool ApplyCompositionInstancesToBytecode();
    bool InitializeCompilationProcess(std::vector<XkslMixerOutputStage>& outputStages);
    bool MergeStreamMembers(TypeStructMemberArray& globalListOfMergedStreamVariables);
    bool AnalyseStreamsAndCBuffersAccessesForOutputStages(std::vector<XkslMixerOutputStage>& outputStages, TypeStructMemberArray& globalListOfMergedStreamVariables);
    bool ValidateStagesStreamMembersFlow(std::vector<XkslMixerOutputStage>& outputStages, TypeStructMemberArray& globalListOfMergedStreamVariables);
    bool ReshuffleStreamVariables(std::vector<XkslMixerOutputStage>& outputStages, TypeStructMemberArray& globalListOfMergedStreamVariables);
    bool RemoveAllUnusedShaders(std::vector<XkslMixerOutputStage>& outputStages);
	bool RemoveAllUnusedFunctionsAndMembers(std::vector<XkslMixerOutputStage>& outputStages);
    bool FinalizeCompilation(std::vector<XkslMixerOutputStage>& outputStages);
    bool GenerateBytecodeForAllStages(std::vector<XkslMixerOutputStage>& outputStages);
    bool ProcessCBuffers(std::vector<XkslMixerOutputStage>& outputStages);

    static bool IsResourceType(const spv::Op& opCode);
    static bool IsScalarType(const spv::Op& opCode);
    static bool IsVectorType(const spv::Op& opCode) { return opCode == spv::OpTypeVector; }
    static bool IsMatrixType(const spv::Op& opCode) { return opCode == spv::OpTypeMatrix; }
    static bool IsArrayType(const spv::Op& opCode) { return opCode == spv::OpTypeArray; }
    bool IsArrayType(TypeInstruction* type) { return IsArrayType(type->GetOpCode()); }
    bool IsMatrixType(TypeInstruction* type) { return IsMatrixType(type->GetOpCode()); }
    bool IsMatrixArrayType(TypeInstruction* type);
    int GetVectorTypeCountElements(TypeInstruction* vectorType);

    bool AreTypeInstructionsIdentical(spv::Id typeId1, spv::Id typeId2);
    bool GetTypeReflectionDescription(TypeInstruction* type, bool isRowMajor, std::string* memberAttribute, TypeReflectionDescription& typeReflection,
        const std::vector<unsigned int>* listStartPositionOfAllMemberDecorateInstructions, int iterationCounter = 0);
    bool GetIntegerConstTypeExpressionValue(ConstInstruction* constObject, int& constValue);
    spv::Id GetOrCreateTypeDefaultConstValue(spv::Id& newId, TypeInstruction* type, const std::vector<ConstInstruction*>& listAllConsts,
        std::vector<spv::Instruction>& listNewConstInstructionsToAdd, int iterationCounter = 0);
    ConstInstruction* FindConstFromList(const std::vector<ConstInstruction*>& listConsts, spv::Op opCode, spv::Id typeId, const std::vector<unsigned int>& values);
    //std::uint32_t getBasicConstTypeHashmapValue(spv::Op opCode, TypeInstruction* constType, unsigned int wordCount, unsigned int* values);

    bool GetStructTypeMembersTypeIdList(TypeInstruction* structType, std::vector<spv::Id>& membersTypeList);
    bool GetFunctionLabelAndReturnInstructionsPosition(FunctionInstruction* function, unsigned int& labelPos, unsigned int& latestReturnPos, unsigned int& countReturnInstructions);
    bool GetFunctionLabelInstructionPosition(FunctionInstruction* function, unsigned int& labelPos);
    FunctionInstruction* GetShaderFunctionForEntryPoint(std::string entryPointName);
    bool RemoveShaderFromBytecodeAndData(ShaderClassData* shader, std::vector<range_t>& vecStripRanges);
    bool RemoveShaderTypeFromBytecodeAndData(ShaderTypeData* shaderType, std::vector<range_t>& vecStripRanges);
	//bool RemoveFunctionInstructionFromBytecodeAndData(FunctionInstruction* functionToRemove, std::vector<range_t>& vecStripRanges);

    void ReleaseAllMaps();
    bool BuildAllMaps();
    bool UpdateAllMaps();
    bool UpdateAllObjectsPositionInTheBytecode();
    bool BuildConstsHashmap(std::unordered_map<std::uint32_t, pairIdPos>& mapHashPos);
    bool BuildTypesAndConstsHashmap(std::unordered_map<std::uint32_t, pairIdPos>& mapHashPos);
    bool BuildDeclarationNameMapsAndObjectsDataList(std::vector<ParsedObjectData>& listParsedObjectsData);
    ObjectInstructionBase* CreateAndAddNewObjectFor(ParsedObjectData& parsedData);
    bool DecorateObjects(std::vector<bool>& vectorIdsToDecorate);

    bool UpdateOverridenFunctionMap(std::vector<ShaderClassData*>& listShadersMerged);
    bool UpdateOpFunctionCallTargetsInstructionsToOverridingFunctions();
    bool UpdateFunctionCallsHavingUnresolvedBaseAccessor();

    //bytecode Update controller
    BytecodeChunk* CreateNewBytecodeChunckToInsert(BytecodeUpdateController& bytecodeUpdateController, unsigned int instructionPos, BytecodeChunkInsertionTypeEnum insertionType, unsigned int offset = 0);
    bool SetNewAtomicValueUpdate(BytecodeUpdateController& bytecodeUpdateController, unsigned int pos, uint32_t value);
    BytecodePortionToReplace* SetNewPortionToReplace(BytecodeUpdateController& bytecodeUpdateController, unsigned int pos);
    BytecodePortionToRemove* AddPortionToRemove(BytecodeUpdateController& bytecodeUpdateController, unsigned int position, unsigned int count);
    bool ApplyBytecodeUpdateController(BytecodeUpdateController& bytecodeUpdateController);

    bool InitDefaultHeader();
    bool ComputeShadersLevel();
    bool HasAnyError() { return errorMessages.size() > 0; }

    void GetShaderFamilyTree(ShaderClassData* shaderFromFamily, std::vector<ShaderClassData*>& shaderFamilyTree);
    void GetShaderChildrenList(ShaderClassData* shader, std::vector<ShaderClassData*>& children);
    static bool GetShadersFullDependencies(SpxCompiler* bytecodeSource, const std::vector<ShaderClassData*>& listShaders, std::vector<ShaderClassData*>& fullDependencies);

    static bool parseInstruction(const std::vector<std::uint32_t>& bytecode, unsigned int word, spv::Op& opCode, unsigned int& wordCount, spv::Id& type, spv::Id& result, std::vector<spv::Id>& listIds, std::string& errorMsg);
    bool parseInstruction(unsigned int word, spv::Op& opCode, unsigned int& wordCount, spv::Id& type, spv::Id& result, std::vector<spv::Id>& listIds);
    bool flagAllIdsFromInstruction(unsigned int word, spv::Op& opCode, unsigned int& wordCount, std::vector<bool>& listIdsUsed);
    static bool remapAllInstructionIds(std::vector<std::uint32_t>& bytecode, unsigned int word, unsigned int& wordCount, const std::vector<spv::Id>& remapTable, std::string& errorMsg);
    static bool remapAllIds(std::vector<std::uint32_t>& bytecode, unsigned int begin, unsigned int end, const std::vector<spv::Id>& remapTable, std::string& errorMsg);
    bool remapAllIds(std::vector<std::uint32_t>& bytecode, unsigned int begin, unsigned int end, const std::vector<spv::Id>& remapTable);

    bool GenerateBytecodeForStage(XkslMixerOutputStage& stage, std::vector<spv::Id>& listObjectIdsToKeep);
    
private:
    //static variable share between all SpxCompiler instances
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
    int GetCountShaders();
    std::string GetShaderUniqueId();
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
    bool GetStartPositionOfAllMemberDecorateInstructions(std::vector<unsigned int>& listStartPositionOfAllMemberDecorateInstructions);
    
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

    friend class SpxMixer;
};

class SPVHeaderStageExecutionMode
{
public:
    spv::ExecutionMode mode;
    int value1, value2, value3;

    SPVHeaderStageExecutionMode(spv::ExecutionMode mode, int value1 = -1, int value2 = -1, int value3 = -1) : mode(mode), value1(value1), value2(value2), value3(value3){}
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

    bool IsNeededAsInput() const { return (stageIONeeded & ((int)MemberIOEnum::Input)) != 0; }
    bool IsNeededAsOutput() const { return (stageIONeeded & ((int)MemberIOEnum::Output)) != 0; }
    bool IsNeededAsInputOrOutput() const { return stageIONeeded != 0; }
    bool IsInputOnly() const { return (stageIONeeded == (int)MemberIOEnum::Input); }

    //bool HasWriteAccess() { return (accessesNeeded & ((int)MemberAccessDetailsEnum::Write)); }
    bool IsWriteFirstStream() const { return firstAccess == MemberFirstAccessEnum::WriteFirst; }
    bool IsReadFirstStream() const { return firstAccess == MemberFirstAccessEnum::ReadFirst; }
    bool IsBeingAccessed() const { return firstAccess != MemberFirstAccessEnum::Undefined; }
};

//Contains output stage info (stage + entrypoint), bytecode, plus additionnal data processed by the mixer during compilation
class XkslMixerOutputStage
{
public:
    OutputStageBytecode* outputStage; //set by the user

    SpxCompiler::FunctionInstruction* entryFunction;  //set when initializing the compilation process

    std::vector<MemberAccessDetails> listStreamVariablesAccessed;  //list of stream variables accessed by the stage, set by AnalyseStreams method
    std::vector<SpxCompiler::FunctionInstruction*> listFunctionsCalledAndAccessingStreamMembers;  //list of functions called by the stage, accessing some stream members
    std::vector<SpxCompiler::ShaderTypeData*> listCBuffersAccessed;

    class OutputStageIOVariable
    {
    public:
        spv::Id spvVariableId;
        int globalStreamMemberIndex;
        int locationNum;
        std::string semanticName;

        OutputStageIOVariable(spv::Id spvVariableId, int globalStreamMemberIndex, int locationNum, std::string semanticName) :
            spvVariableId(spvVariableId), globalStreamMemberIndex(globalStreamMemberIndex) , locationNum(locationNum), semanticName(semanticName){}
    };

    std::vector<OutputStageIOVariable> listStageInputVariableInfo;
    std::vector<OutputStageIOVariable> listStageOutputVariableInfo;

    XkslMixerOutputStage(OutputStageBytecode* outputStage) : outputStage(outputStage) {}
};

}  // namespace xkslang

#endif  // XKSLANG_XKSL_SPX_STREAM_REMAPPER_H__

//
// Copyright (C) 

#ifndef XKSLANG_XKSL_SPX_STREAM_PARSER_H__
#define XKSLANG_XKSL_SPX_STREAM_PARSER_H__

#include <string>
#include <vector>
#include <stack>
#include <sstream>

#include "SPIRV/spirv.hpp"

#include "define.h"

#define SPVInstruction_MAX_COUNT_IDS 3

namespace xkslang
{

class SPXStreamParser
{
public:

    enum class SPVObjectTypeEnum
    {
        SPVTypeUndefined,
        SPVTypeFunction,
        SPVTypeShader
    };

    class SPVFunction;
    class SPVShader;
    class SPVObject
    {
    public:
        SPVObject(SPVObjectTypeEnum objectType): objectType(objectType){}
        virtual ~SPVObject(){}

        SPVFunction* GetAsSPVFunction() {return dynamic_cast<SPVFunction*>(this);}
        SPVShader* GetAsSPVShader() { return dynamic_cast<SPVShader*>(this); }

    public:
        const SPVObjectTypeEnum objectType;
        std::string declarationName;
    };

    class SPVShader : public SPVObject
    {
    public:
        uint32_t opDeclaration;       //index of type declaration (OpTypeXlslShaderClass)
        std::vector<SPVShader*> parents;

        SPVShader() : SPVObject(SPVObjectTypeEnum::SPVTypeShader), opDeclaration(0) {}
        void AddParent(SPVShader* p)
        {
            parents.push_back(p);
        }
    };

    class SPVFunction : public SPVObject
    {
    public:
        uint32_t opStart;       //index of OpFunction
        uint32_t opEnd;         //index of OpFunctionEnd
        bool isAbstract;
        bool isOverride;
        bool isStage;
        bool isClone;

        SPVFunction() : SPVObject(SPVObjectTypeEnum::SPVTypeFunction), opStart(0), opEnd(0), isAbstract(false), isOverride(false), isStage(false), isClone(false) {}
    };

    class SPVInstruction
    {
    public:
        uint32_t instructionNum;
        uint32_t streamIndex;

        spv::Op opCode;
        spv::Id resultId;
        spv::Id typeId;

        const char* str;
        int strLen;

        const uint32_t* ids[SPVInstruction_MAX_COUNT_IDS];
        int nbIds[SPVInstruction_MAX_COUNT_IDS];

        const uint32_t* immediates[SPVInstruction_MAX_COUNT_IDS];
        int nbImmediates[SPVInstruction_MAX_COUNT_IDS];

        uint32_t decorationType;

    public:
        SPVInstruction(uint32_t instructionNum, uint32_t streamIndex, spv::Op opCode, spv::Id resultId, spv::Id typeId);
        virtual ~SPVInstruction(){}

        std::string toString()
        {
            std::ostringstream sstream;
            sstream << "Num:" << instructionNum << " Pos:" << streamIndex << " opCode:" << opCode;
            return sstream.str();
        }
    };

public:
    SPXStreamParser(const std::vector<unsigned int>& stream) : stream(stream), size(0), word(0), idBound(0), nextNestedControl(0){ }
    virtual ~SPXStreamParser();

    bool ValidateHeader(uint32_t& magicNumber, uint32_t& moduleVersion, uint32_t& generatorMagicNumber);
    bool DisassembleSpirXStream();
    bool ProcessSpriXBytecode();

    SPVObject* GetSpvObjectById(uint32_t id);
    SPVFunction* GetSpvFunctionById(uint32_t id);
    SPVShader* GetSpvShaderById(uint32_t id);
    SPVShader* GetSpvShaderByName(const std::string& name);

    void copyMessagesTo(std::vector<std::string>& list);

private:
    std::vector<std::string> messages;
    const std::vector<unsigned int>& stream;
    int size;
    int word;

    // map each <id> to the instruction that created it
    spv::Id idBound;

    // the word offset into the stream where the instruction for result [id] starts; 0 if not yet seen (forward reference or function parameter)
    //std::vector<unsigned int> idInstruction;
    //std::vector<std::string> idDescriptor; // the best text string known for explaining the <id>

    std::vector<SPVObject*> listSpvObjectsById;

    // schema
    unsigned int schema;

    // stack of structured-merge points
    std::stack<spv::Id> nestedControl;
    spv::Id nextNestedControl;

    std::vector<SPVInstruction*> listInstructions;
    std::vector<SPVFunction*> listFunctions;
    std::vector<SPVShader*> listShaders;

private:
    void disassembleInstruction(SPVInstruction* instr, spv::Op opCode, spv::Id resultId, spv::Id typeId, int numOperands);
    void disassembleImmediates(int numOperands, SPVInstruction* instr);
    void disassembleIds(int numOperands, SPVInstruction* instr);
    int disassembleString(SPVInstruction* instr);
    void setDecorationType(SPVInstruction* instr, uint32_t decorationType);

    bool AddSPVFunction(SPVFunction*, uint32_t resultId);
    bool AddSPVShader(SPVShader*, uint32_t resultId);
};

}  // namespace xkslang

#endif  // XKSLANG_XKSL_SPX_STREAM_PARSER_H__

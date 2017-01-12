//
// Copyright (C) 

#ifndef XKSLANG_XKSLSPIRXPARSER_H
#define XKSLANG_XKSLSPIRXPARSER_H

#include <string>
#include <vector>
#include <stack>
#include <sstream>

#include "SPIRV/spirv.hpp"

#include "define.h"

#define SPVInstruction_MAX_COUNT_IDS 3

namespace xkslang
{

class SpirxStreamParser
{
public:

    class SPVFunction;
    class SPVObject
    {
    public:
        SPVObject() {}

        virtual SPVFunction* GetAsSPVFunction(){return nullptr;}
    };

    class SPVFunction : public SPVObject
    {
    public:
        std::string declarationName;
        uint32_t opStart;       //index of OpFunction
        uint32_t opEnd;         //index of OpFunctionEnd
        bool isAbstract;
        bool isOverride;
        bool isStage;
        bool isClone;

        SPVFunction() : SPVObject(), opStart(0), opEnd(0), isAbstract(false), isOverride(false), isStage(false), isClone(false) {}

        virtual SPVFunction* GetAsSPVFunction() { return this; }
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
    SpirxStreamParser(const std::vector<unsigned int>& stream) : stream(stream), size(0), word(0), idBound(0), nextNestedControl(0){ }
    virtual ~SpirxStreamParser();

    bool ValidateHeader(uint32_t& magicNumber, uint32_t& moduleVersion, uint32_t& generatorMagicNumber);
    bool DisassembleSpirXStream();
    bool ProcessSpriXBytecode();

    SPVFunction* GetSpvFunctionById(uint32_t id);

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

private:
    void disassembleInstruction(SPVInstruction* instr, spv::Op opCode, spv::Id resultId, spv::Id typeId, int numOperands);
    void disassembleImmediates(int numOperands, SPVInstruction* instr);
    void disassembleIds(int numOperands, SPVInstruction* instr);
    int disassembleString(SPVInstruction* instr);
    void setDecorationType(SPVInstruction* instr, uint32_t decorationType);
};

}  // namespace xkslang

#endif  // XKSLANG_XKSLSPIRXPARSER_H

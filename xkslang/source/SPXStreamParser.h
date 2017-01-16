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

//============================================================================================================//
//===========================================  SPVInstruction  ===============================================//
//============================================================================================================//
class SPVFunction;
class SPVObject;
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
    virtual ~SPVInstruction() {}

    std::string toString()
    {
        std::ostringstream sstream;
        sstream << "Num:" << instructionNum << " Pos:" << streamIndex << " opCode:" << opCode;
        return sstream.str();
    }
};

//============================================================================================================//
//===========================================  SpxStreamParser  ==============================================//
//============================================================================================================//
class XkslMixer;
class SPVShader;
class SPVFunction;
class SpxStreamParser
{
public:
    SpxStreamParser(XkslMixer* mixer, const std::vector<unsigned int>& stream) : mixer(mixer), stream(stream), size(0), word(0), idBound(0), nextNestedControl(0){ }
    virtual ~SpxStreamParser();

    bool ValidateHeader(uint32_t& magicNumber, uint32_t& moduleVersion, uint32_t& generatorMagicNumber);
    bool DisassembleSpirXStream(int streamMixinNum);
    bool DecorateAllObjects();

    SPVObject* GetSpvObjectByResultId(uint32_t id);

    void copyMessagesTo(std::vector<std::string>& list);

private:
    XkslMixer* mixer;
    const std::vector<unsigned int>& stream;
    uint32_t size;
    uint32_t word;
    uint32_t schema;
    spv::Id idBound;

    std::vector<std::string> messages;
    
    // the word offset into the stream where the instruction for result [id] starts; 0 if not yet seen (forward reference or function parameter)
    //std::vector<unsigned int> idInstruction;
    //std::vector<std::string> idDescriptor; // the best text string known for explaining the <id>

    // stack of structured-merge points
    std::stack<spv::Id> nestedControl;
    spv::Id nextNestedControl;

    std::vector<SPVInstruction*> listInstructions;
    std::vector<SPVObject*> listSpvObjectsByResultId;

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

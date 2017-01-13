//
// Copyright (C)

#include <cassert>
#include <iostream>
#include <memory>
#include <string>

#include "glslang/Public/ShaderLang.h"
#include "StandAlone/ResourceLimits.h"

#include "SPIRV/doc.h"
//#include "SPIRV/disassemble.h"
//#include "SPIRV/SPVRemapper.h"

#include "SPXStreamParser.h"

using namespace std;
using namespace spv;
using namespace xkslang;

//=====================================================================================================================
//=====================================================================================================================
SPXStreamParser::SPVInstruction::SPVInstruction(uint32_t instructionNum, uint32_t streamIndex, spv::Op opCode, spv::Id resultId, spv::Id typeId)
{
    this->instructionNum = instructionNum;
    this->streamIndex = streamIndex;
    this->opCode = opCode;
    this->resultId = resultId;
    this->typeId = typeId;

    this->str = nullptr;
    this->strLen = 0;

    for (int i = 0; i < SPVInstruction_MAX_COUNT_IDS; ++i)
    {
        this->ids[i] = nullptr;
        this->nbIds[i] = 0;

        this->immediates[i] = nullptr;
        this->nbImmediates[i] = 0;
    }

    this->decorationType = 0;
}

//=====================================================================================================================
//=====================================================================================================================

static bool error(vector<string>& msgs, string msg)
{
    msgs.push_back(string("Error: ") + msg);
    return false;
}

void SPXStreamParser::copyMessagesTo(std::vector<std::string>& list)
{
    list.insert(list.end(), messages.begin(), messages.end());
}

SPXStreamParser::~SPXStreamParser()
{
    for (int i = 0; i<listInstructions.size(); ++i)
    {
        delete listInstructions[i];
    }
    listInstructions.clear();

    for (int i=0; i<listSpvObjectsById.size(); ++i)
    {
        if (listSpvObjectsById[i] != nullptr)
        {
            delete listSpvObjectsById[i];
            listSpvObjectsById[i] = nullptr;
        }
    }
    listFunctions.clear();
}

bool SPXStreamParser::ProcessSpriXBytecode()
{
    //Process all decorate instructions
    int countInstructions = listInstructions.size();
    for (int i = 0; i < countInstructions; ++i)
    {
        SPVInstruction* instr = listInstructions[i];
        if (instr->opCode == spv::OpDecorate)
        {
            if (instr->nbIds[0] == 0) return error(messages, "The decorate operand defines no id");
            uint32_t objectDecoratedId = *(instr->ids[0]);

            SPVObject* spvObj = GetSpvObjectById(objectDecoratedId);
            if (spvObj != nullptr)
            {
                switch (spvObj->objectType)
                {
                    case SPVObjectTypeEnum::SPVTypeFunction:
                    {
                        SPVFunction* spvFunction = spvObj->GetAsSPVFunction();
                        if (spvFunction == nullptr) return error(messages, "Cannot get SPVFunction from SPVObject");

                        //Update the function attributes with decorate attributes
                        switch (instr->decorationType)
                        {
                            case DecorationBelongsToShader:
                                //gfdgsdfgsdfg;
                                break; //spvFunction->declarationName = instr->str; break;
                            case DecorationDeclarationName:
                                spvFunction->declarationName = instr->str; break;
                            case DecorationAttributeStage:
                                spvFunction->isStage = true; break;
                            case DecorationMethodAbstract:
                                spvFunction->isAbstract = true; break;
                            case DecorationMethodOverride:
                                spvFunction->isOverride = true; break;
                            case DecorationMethodClone:
                                spvFunction->isClone = true; break;
                        }

                        break;
                    }

                    case SPVObjectTypeEnum::SPVTypeShader:
                    {
                        SPVShader* spvShader = spvObj->GetAsSPVShader();
                        if (spvShader == nullptr) return error(messages, "Cannot get SPVShader from SPVObject");

                        //Update the shader attributes with decorate attributes
                        switch (instr->decorationType)
                        {
                            case DecorationShaderInheritFromParent:
                            {
                                SPVShader* spvParent = GetSpvShaderByName(instr->str);
                                if (spvParent == nullptr) return error(messages, string("Cannot find shader: ") + instr->str);
                                spvShader->AddParent(spvParent);
                                break;
                            }
                            case DecorationDeclarationName:
                                spvShader->declarationName = instr->str; break;
                                break;
                        }

                        break;
                    }
                } //end switch (spvObj->objectType)
            }
        }
    }

    return true;
}

SPXStreamParser::SPVObject* SPXStreamParser::GetSpvObjectById(uint32_t id)
{
    if (id < 0 || id >= listSpvObjectsById.size()) return nullptr;
    return listSpvObjectsById[id];
}

SPXStreamParser::SPVFunction* SPXStreamParser::GetSpvFunctionById(uint32_t id)
{
    if (id < 0 || id >= listSpvObjectsById.size()) return nullptr;

    if (listSpvObjectsById[id] == nullptr) return nullptr;
    return listSpvObjectsById[id]->GetAsSPVFunction();
}

SPXStreamParser::SPVShader* SPXStreamParser::GetSpvShaderById(uint32_t id)
{
    if (id < 0 || id >= listShaders.size()) return nullptr;

    if (listShaders[id] == nullptr) return nullptr;
    return listShaders[id]->GetAsSPVShader();
}

SPXStreamParser::SPVShader* SPXStreamParser::GetSpvShaderByName(const std::string& name)
{
    int count = listShaders.size();
    for (int i = 0; i < count; ++i)
    {
        if (listShaders[i]->declarationName == name) return listShaders[i];
    }
    return nullptr;
}

bool SPXStreamParser::AddSPVFunction(SPVFunction* func, uint32_t resultId)
{
    if (resultId == 0 || resultId >= listSpvObjectsById.size()) return error(messages, "the function has an invalid result Id");
    if (this->listSpvObjectsById[resultId] != nullptr) return error(messages, "an object already exists with the same resultId");
    this->listSpvObjectsById[resultId] = func;
    this->listFunctions.push_back(func);
}

bool SPXStreamParser::AddSPVShader(SPVShader* shader, uint32_t resultId)
{
    if (resultId == 0 || resultId >= listSpvObjectsById.size()) return error(messages, "the shader has an invalid result Id");
    if (this->listSpvObjectsById[resultId] != nullptr) return error(messages, "an object already exists with the same resultId");
    this->listSpvObjectsById[resultId] = shader;
    this->listShaders.push_back(shader);
}

bool SPXStreamParser::ValidateHeader(uint32_t& magicNumber, uint32_t& moduleVersion, uint32_t& generatorMagicNumber)
{
    spv::Parameterize();   //get opcode details (if not already done)

    messages.clear();

    if (size != 0 || word != 0)
    {
        //to be processed one time only
        return error(messages, "stream has already been validated");
    }

    size = (int)stream.size();
    if (size < 4)
    {
        return error(messages, "stream is too short");
    }

    magicNumber = stream[word++];
    moduleVersion = stream[word++];
    generatorMagicNumber = stream[word++];

    // Result <id> bound
    idBound = stream[word++];
    listSpvObjectsById.resize(idBound, nullptr);
    //idDescriptor.resize(bound);

    return true;
}

bool SPXStreamParser::DisassembleSpirXStream()
{
    if (word != 4 || word > size)
    {
        return error(messages, "invalid call to processSpirXStream: call validate function first");
    }

    // Reserved schema, must be 0 for now
    schema = stream[word++];
    if (schema != 0)
    {
        return error(messages, "bad schema, must be 0");
    }

    SPVFunction* parsedFunction = nullptr;

    // Process Instructions
    uint32_t instructionNum = -1;
    while (word < size)
    {
        instructionNum++;
        int instructionStart = word;

        // Instruction wordCount and opcode
        uint32_t firstWord = stream[word];
        uint32_t wordCount = firstWord >> WordCountShift;
        Op opCode = (Op)(firstWord & OpCodeMask);
        uint32_t nextInst = word + wordCount;
        ++word;

        if (nextInst > size){
            return error(messages, "stream instruction terminated too early");
        }

        // Base for computing number of operands; will be updated as more is learned
        unsigned numOperands = wordCount - 1;

        // Type <id>
        Id typeId = 0;
        if (InstructionDesc[opCode].hasType()) {
            typeId = stream[word++];
            --numOperands;
        }

        // Result <id>
        Id resultId = 0;
        if (InstructionDesc[opCode].hasResult()) {
            resultId = stream[word++];
            --numOperands;

            // save instruction for future reference
            //idInstruction[resultId] = instructionStart;
        }

        /*if (opCode == OpDecorate)
        {
            int lkgjflsdj = 343434;
        }*/

        // Hand off the Op and all its operands
        SPXStreamParser::SPVInstruction* instruction = new SPXStreamParser::SPVInstruction(instructionNum, instructionStart, opCode, resultId, typeId);
        listInstructions.push_back(instruction);
        disassembleInstruction(instruction, opCode, resultId, typeId, numOperands);

        if (messages.size() > 0)
        {
            return error(messages, string("Failed to parse the instruction: ") + instruction->toString());
        }

        if (word != nextInst) {
            return error(messages, "incorrect number of operands consumed.");
            //word = nextInst;
        }

        switch (opCode)
        {
            case spv::OpDecorate:
                //if (!DisassembleOpDecorateInstruction(stream, word, opCode, numOperands, typeId, resultId))
                //return error(msgs, "Failed to disassemble the instruction");
                break;

            case spv::OpFunction:
            {
                if (parsedFunction != nullptr) return error(messages, "A function is aready being parsed");
                parsedFunction = new SPVFunction();
                AddSPVFunction(parsedFunction, resultId);
                parsedFunction->opStart = instructionStart;
                break;
            }

            case spv::OpFunctionEnd:
            {
                if (parsedFunction == nullptr) return error(messages, "No function is being parsed");
                parsedFunction->opEnd = instructionStart;
                parsedFunction = nullptr;
                break;
            }

            case spv::OpTypeXlslShaderClass:
            {
                SPVShader* shader = new SPVShader();
                AddSPVShader(shader, resultId);
                break;
            }

            default:
                break;
        }
    }

    return true;
}

void SPXStreamParser::disassembleInstruction(SPVInstruction* instr, Op opCode, Id resultId, Id typeId, int numOperands)
{
    if (opCode == OpLoopMerge || opCode == OpSelectionMerge)
        nextNestedControl = stream[word];
    else if (opCode == OpBranchConditional || opCode == OpSwitch) {
        if (nextNestedControl) {
            nestedControl.push(nextNestedControl);
            nextNestedControl = 0;
        }
    }
    /// else if (opCode == OpExtInstImport) {
    ///     idDescriptor[resultId] = (const char*)(&stream[word]);
    /// }
    /// else {
    ///     if (resultId != 0 && idDescriptor[resultId].size() == 0) {
    ///         switch (opCode) {
    ///         case OpTypeInt:
    ///             idDescriptor[resultId] = "int";
    ///             break;
    ///         case OpTypeFloat:
    ///             idDescriptor[resultId] = "float";
    ///             break;
    ///         case OpTypeBool:
    ///             idDescriptor[resultId] = "bool";
    ///             break;
    ///         case OpTypeStruct:
    ///             idDescriptor[resultId] = "struct";
    ///             break;
    ///         case OpTypePointer:
    ///             idDescriptor[resultId] = "ptr";
    ///             break;
    ///         case OpTypeVector:
    ///             if (idDescriptor[stream[word]].size() > 0)
    ///                 idDescriptor[resultId].append(idDescriptor[stream[word]].begin(), idDescriptor[stream[word]].begin() + 1);
    ///             idDescriptor[resultId].append("vec");
    ///             switch (stream[word + 1]) {
    ///             case 2:   idDescriptor[resultId].append("2");   break;
    ///             case 3:   idDescriptor[resultId].append("3");   break;
    ///             case 4:   idDescriptor[resultId].append("4");   break;
    ///             case 8:   idDescriptor[resultId].append("8");   break;
    ///             case 16:  idDescriptor[resultId].append("16");  break;
    ///             case 32:  idDescriptor[resultId].append("32");  break;
    ///             default: break;
    ///             }
    ///             break;
    ///         default:
    ///             break;
    ///         }
    ///     }
    /// }

    // Handle images specially, so can put out helpful strings.
    if (opCode == OpTypeImage) {
        /// disassembleIds(1);
        /// out << " " << DimensionString((Dim)stream[word++]);
        /// out << (stream[word++] != 0 ? " depth" : "");
        /// out << (stream[word++] != 0 ? " array" : "");
        /// out << (stream[word++] != 0 ? " multi-sampled" : "");
        /// switch (stream[word++]) {
        /// case 0: out << " runtime";    break;
        /// case 1: out << " sampled";    break;
        /// case 2: out << " nonsampled"; break;
        /// }
        /// out << " format:" << ImageFormatString((ImageFormat)stream[word++]);
        /// 
        /// if (numOperands == 8) {
        ///     out << " " << AccessQualifierString(stream[word++]);
        /// }

        word += numOperands;
        return;
    }

    // Handle all the parameterized operands
    for (int op = 0; op < InstructionDesc[opCode].operands.getNum() && numOperands > 0; ++op) {
        OperandClass operandClass = InstructionDesc[opCode].operands.getClass(op);
        switch (operandClass) {
        case OperandId:
        case OperandScope:
        case OperandMemorySemantics:
            disassembleIds(1, instr);
            --numOperands;
            /// // Get names for printing "(XXX)" for readability, *after* this id
            /// if (opCode == OpName)
            ///     idDescriptor[stream[word - 1]] = (const char*)(&stream[word]);
            break;
        case OperandVariableIds:
            disassembleIds(numOperands, instr);
            return;
        case OperandImageOperands:
            word++; /// outputMask(OperandImageOperands, stream[word++]);
            --numOperands;
            disassembleIds(numOperands, instr);
            return;
        case OperandOptionalLiteral:
        case OperandVariableLiterals:
            if (opCode == OpDecorate &&
                (stream[word - 1] == DecorationDeclarationName
                || stream[word - 1] == DecorationShaderInheritFromParent
                || stream[word - 1] == DecorationBelongsToShader))
            {
                //XKSL extensions. Did not find how to define a different operand class per decorationId (their system don't feature this)
                numOperands -= disassembleString(instr);
                break;
            }

            if ((opCode == OpDecorate && stream[word - 1] == DecorationBuiltIn) ||
                (opCode == OpMemberDecorate && stream[word - 1] == DecorationBuiltIn)) {
                word++; /// out << BuiltInString(stream[word++]);
                --numOperands;
                ++op;
            }
            disassembleImmediates(numOperands, instr);
            return;
        case OperandVariableIdLiteral:
            while (numOperands > 0) {
                /// out << std::endl;
                /// outputResultId(0);
                /// outputTypeId(0);
                /// outputIndent();
                /// out << "     Type ";
                disassembleIds(1, instr);
                /// out << ", member ";
                disassembleImmediates(1, instr);
                numOperands -= 2;
            }
            return;
        case OperandVariableLiteralId:
            while (numOperands > 0) {
                /// out << std::endl;
                /// outputResultId(0);
                /// outputTypeId(0);
                /// outputIndent();
                /// out << "     case ";
                disassembleImmediates(1, instr);
                /// out << ": ";
                disassembleIds(1, instr);
                numOperands -= 2;
            }
            return;
        case OperandLiteralNumber:
            disassembleImmediates(1, instr);
            --numOperands;
///             if (opCode == OpExtInst) {
///                 ExtInstSet extInstSet = GLSL450Inst;
///                 const char* name = idDescriptor[stream[word - 2]].c_str();
///                 if (0 == memcmp("OpenCL", name, 6)) {
///                     extInstSet = OpenCLExtInst;
/// #ifdef AMD_EXTENSIONS
///                 }
///                 else if (strcmp(spv::E_SPV_AMD_shader_ballot, name) == 0 ||
///                     strcmp(spv::E_SPV_AMD_shader_trinary_minmax, name) == 0 ||
///                     strcmp(spv::E_SPV_AMD_shader_explicit_vertex_parameter, name) == 0 ||
///                     strcmp(spv::E_SPV_AMD_gcn_shader, name) == 0) {
///                     extInstSet = GLSLextAMDInst;
/// #endif
/// #ifdef NV_EXTENSIONS
///                 }
///                 else if (strcmp(spv::E_SPV_NV_sample_mask_override_coverage, name) == 0 ||
///                     strcmp(spv::E_SPV_NV_geometry_shader_passthrough, name) == 0) {
///                     extInstSet = GLSLextNVInst;
/// #endif
///                 }
///                 unsigned entrypoint = stream[word - 1];
///                 if (extInstSet == GLSL450Inst) {
///                     if (entrypoint < GLSLstd450Count) {
///                         out << "(" << GlslStd450DebugNames[entrypoint] << ")";
///                     }
/// #ifdef AMD_EXTENSIONS
///                 }
///                 else if (extInstSet == GLSLextAMDInst) {
///                     out << "(" << GLSLextAMDGetDebugNames(name, entrypoint) << ")";
/// #endif
/// #ifdef NV_EXTENSIONS
///                 }
///                 else if (extInstSet == GLSLextNVInst) {
///                     out << "(" << GLSLextNVGetDebugNames(name, entrypoint) << ")";
/// #endif
///                 }
///             }
            break;
        case OperandOptionalLiteralString:
        case OperandLiteralString:
            numOperands -= disassembleString(instr);
            break;
        default:
            /// assert(operandClass >= OperandSource && operandClass < OperandOpcode);
            if (!(operandClass >= OperandSource && operandClass < OperandOpcode))
            {
                error(messages, "Invalid operandClass");
                return;
            }

            if (OperandClassParams[operandClass].bitmask)
                word++; /// outputMask(operandClass, stream[word++]);
            else
            {
                //OperandClassParams[operandClass].getName(stream[word]);
                setDecorationType(instr, stream[word]);
                word++;
             }
            --numOperands;

            break;
        }
    }

    return;
}

void SPXStreamParser::setDecorationType(SPVInstruction* instr, uint32_t decorationType)
{
    if (instr->decorationType != 0)
    {
        error(messages, "The decorationType has already been set");
        return;
    }

    instr->decorationType = decorationType;
}

void SPXStreamParser::disassembleImmediates(int numOperands, SPXStreamParser::SPVInstruction* instr)
{
    int ind;
    for (ind=0; ind < SPVInstruction_MAX_COUNT_IDS; ++ind)
        if (instr->immediates[ind] == nullptr) break;

    if (ind == SPVInstruction_MAX_COUNT_IDS)
    {
        error(messages, "The instruction immediates are full");
        return;
    }

    instr->immediates[ind] = &(stream[word]);
    instr->nbImmediates[ind] = numOperands;
    word += numOperands;

    /*for (int i = 0; i < numOperands; ++i) {
        vecImmediates.push_back(stream[word++]);
    }*/
}

void SPXStreamParser::disassembleIds(int numOperands, SPXStreamParser::SPVInstruction* instr)
{
    int ind;
    for (ind = 0; ind < SPVInstruction_MAX_COUNT_IDS; ++ind)
        if (instr->ids[ind] == nullptr) break;

    if (ind == SPVInstruction_MAX_COUNT_IDS)
    {
        error(messages, "The instruction ids are full");
        return;
    }

    instr->ids[ind] = &(stream[word]);
    instr->nbIds[ind] = numOperands;
    word += numOperands;

    /*for (int i = 0; i < numOperands; ++i) {
        vecIds.push_back(stream[word++]);
    }*/
}

// return the number of operands consumed by the string
int SPXStreamParser::disassembleString(SPXStreamParser::SPVInstruction* instr)
{
    if (instr->str != nullptr)
    {
        error(messages, "The instruction already has a string");
        return 0;
    }
    instr->str = (const char*) (&stream[word]);
    instr->strLen = 0;

    int startWord = word;
    const char* wordString;
    bool done = false;
    do {
        unsigned int content = stream[word];
        wordString = (const char*)&content;
        for (int charCount = 0; charCount < 4; ++charCount) {
            if (*wordString == 0) {
                done = true;
                break;
            }
            //string += *(wordString++);
            wordString++;
            instr->strLen++;
        }
        ++word;
    } while (!done);

    return word - startWord;
}
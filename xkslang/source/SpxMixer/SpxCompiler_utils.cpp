//
// Copyright (C)

#include <cassert>
#include <iostream>
#include <memory>
#include <string>

#include "SPIRV/doc.h"

#include "SpxCompiler.h"

using namespace std;
using namespace xkslang;

bool SpxCompiler::ShaderClassData::SetShaderName(const string& originalBaseName, const string& fullName, int countGenerics, string& errorMsg)
{
    this->countGenerics = countGenerics;
    this->name = fullName;
    this->shaderOriginalBaseName = originalBaseName;

    const size_t pos = fullName.find_last_of('<');
    if (this->countGenerics > 0)
    {
        //remove the "<...>" from the shaderName
        if (pos == string::npos) {
            errorMsg = "Invalid fullName format: \"<\" expected when a shader sets some generics";
            return false;
        }
        this->shaderFullNameWithoutGenerics = fullName.substr(0, pos);
    }
    else
    {
        if (pos != string::npos) {
            errorMsg = "Invalid fullName format: \"<\" found while the shader sets no generics";
            return false;
        }
        this->shaderFullNameWithoutGenerics = fullName;
    }

    return true;
}

bool SpxCompiler::SetBytecode(const vector<uint32_t>& bytecode)
{
    spv.clear();
    spv.insert(spv.end(), bytecode.begin(), bytecode.end());

    if (!ValidateHeader()) return error(string("Invalid bytecode header"));
    return true;
}

bool SpxCompiler::SetBytecode(const SpvBytecode& bytecode)
{
    const vector<uint32_t>& vecBytecode = bytecode.getBytecodeStream();
    return SetBytecode(vecBytecode);
}

bool SpxCompiler::ValidateHeader()
{
    if (spv.size() < header_size) {
        return error("file too short");
    }

    if (magic() != spv::MagicNumber) {
        error("bad magic number");
    }

    // field 1 = version
    // field 2 = generator magic
    // field 3 = result <id> bound

    if (schemaNum() != 0) {
        error("bad schema, must be 0");
    }

    if (errorMessages.size() > 0) return false;
    return true;
}

bool SpxCompiler::ProcessBytecodeSanityCheck(const std::vector<uint32_t>& bytecode, std::vector<std::string>& errorMsgs)
{
    unsigned int bytecodeSize = (unsigned int)(bytecode.size());
    if (bytecodeSize < header_size) return error(errorMsgs, "invalid bytecode size");
    unsigned int maxId = bound(bytecode);
    if (maxId == 0) return error(errorMsgs, "invalid bound value: " + to_string(maxId));

    //===============================================================================================================================================
    //===============================================================================================================================================
    //1st step: build the bytecode data
    SpxCompiler spxBytecode;
    if (!spxBytecode.SetBytecode(bytecode)) return error(errorMsgs, "Failed to set the bytecode");

    if (!spxBytecode.BuildAllMaps()) {
        spxBytecode.copyMessagesTo(errorMsgs);
        return error(errorMsgs, "Failed to parse the bytecode");
    }

    //===============================================================================================================================================
    //===============================================================================================================================================
    //2nd step: we parse the bytecode and check that all types, consts and variables are using IDs which have been previously defined in the bytecode
    vector<unsigned int> listAllTypesConstsAndVariablesPosition;
    vector<unsigned int> listAllResultIdsPosition;
    listAllTypesConstsAndVariablesPosition.resize(maxId, 0);
    listAllResultIdsPosition.resize(maxId, 0);
    string errorStr;

    vector<spv::Id> listIds;
    unsigned int start = header_size;
    const unsigned int end = bytecodeSize;
    while (start < end)
    {
        unsigned int wordCount = asWordCount(bytecode, start);
        spv::Op opCode = asOpCode(bytecode, start);

        if (wordCount == 0) return error(errorMsgs, "wordCount is 0 for instruction at pos: " + to_string(start));

        bool hasAType = false;
        spv::Id instructionTypeId = 0;
        bool hasAResultId = false;
        spv::Id instructionResultId = 0;

        //if the instruction has a type, check that this type is already defined
        if (spv::InstructionDesc[opCode].hasType())
        {
            hasAType = true;
            unsigned int posTypeId = start + 1;
            if (posTypeId >= bytecodeSize) return error(errorMsgs, "TypeId position is out of bound for instruction at pos: " + to_string(start));

            instructionTypeId = asId(bytecode, start + 1);
            if (instructionTypeId == 0 || instructionTypeId >= listAllTypesConstsAndVariablesPosition.size()) return error(errorMsgs, "instruction TypeId is out of bound: " + to_string(instructionTypeId));
            if (listAllTypesConstsAndVariablesPosition[instructionTypeId] == 0) return error(errorMsgs, "an instruction is using an undefined type: " + to_string(instructionTypeId));
        }

        //if the instruction has a resultId, check this resultId
        if (spv::InstructionDesc[opCode].hasResult())
        {
            hasAResultId = true;
            unsigned int posResultId = hasAType ? start + 2 : start + 1;
            if (posResultId >= bytecodeSize) return error(errorMsgs, "ResultId position is out of bound for instruction at pos: " + to_string(start));

            instructionResultId = asId(bytecode, posResultId);
            if (instructionResultId == 0 || instructionResultId >= listAllResultIdsPosition.size()) return error(errorMsgs, "instruction ResultId is out of bound: " + to_string(instructionResultId));
            if (listAllResultIdsPosition[instructionResultId] != 0) return error(errorMsgs, "the instruction result Id is already defined: " + to_string(instructionResultId));
            listAllResultIdsPosition[instructionResultId] = start;
        }

        //Check more details for instruction defining types/consts/variables
        bool isOpTypeConstOrVariableInstruction = false;
        spv::Id typeConstVariableResultId = 0;
        if (isConstOp(opCode)) {
            if (start + 2 >= bytecodeSize) return error(errorMsgs, "resultId position is out of bound for instruction at pos: " + to_string(start));
            isOpTypeConstOrVariableInstruction = true;
            typeConstVariableResultId = asId(bytecode, start + 2);
        }
        else if (isTypeOp(opCode)) {
            if (start + 1 >= bytecodeSize) return error(errorMsgs, "resultId position is out of bound for instruction at pos: " + to_string(start));
            isOpTypeConstOrVariableInstruction = true;
            typeConstVariableResultId = asId(bytecode, start + 1);
        }
        else if (isVariableOp(opCode)) {
            if (start + 2 >= bytecodeSize) return error(errorMsgs, "resultId position is out of bound for instruction at pos: " + to_string(start));
            isOpTypeConstOrVariableInstruction = true;
            typeConstVariableResultId = asId(bytecode, start + 2);
        }

        //parse the instruction data
        {
            unsigned int wordCountBis;
            spv::Op opCodeBis;
            spv::Id typeId;
            spv::Id resultId;
            listIds.clear();
            if (!parseInstruction(bytecode, start, opCodeBis, wordCountBis, typeId, resultId, listIds, errorStr)) return error(errorMsgs, "Error parsing the instruction: " + errorStr);
            if (opCodeBis != opCode) return error(errorMsgs, "Inconsistency in the opCode parsed");
            if (wordCountBis != wordCount) return error(errorMsgs, "Inconsistency in the wordCount parsed");

            if (typeId != spvUndefinedId) listIds.push_back(typeId);

            //Check that all IDs are valid
            for (unsigned k = 0; k < listIds.size(); ++k)
            {
                spv::Id anId = listIds[k];
                if (anId >= maxId) return error(errorMsgs, "anId is out of bound: " + to_string(anId));
            }
        }

        //for every types/consts/variables: if the instruction is refering to other IDs, check that those IDs are already defined.
        if (isOpTypeConstOrVariableInstruction)
        {
            if (typeConstVariableResultId == 0 || typeConstVariableResultId != instructionResultId)
                return error(errorMsgs, "typeConstVariableResultId is inconsistent with the instruction resultId: " + to_string(typeConstVariableResultId));

            if (typeConstVariableResultId >= listAllTypesConstsAndVariablesPosition.size()) return error(errorMsgs, "resultId is out of bound: " + to_string(typeConstVariableResultId));

            if (listAllTypesConstsAndVariablesPosition[typeConstVariableResultId] != 0) return error(errorMsgs, "the Id is already used by another type: " + to_string(typeConstVariableResultId));
            listAllTypesConstsAndVariablesPosition[typeConstVariableResultId] = start;

            if (spxBytecode.listAllObjects[typeConstVariableResultId] == nullptr) return error(errorMsgs, "The type, const or variable has no object data defined. Id: " + to_string(typeConstVariableResultId));

            //Check that all IDs refering to by the type/const/variable are already defined
            for (unsigned k = 0; k < listIds.size(); ++k)
            {
                spv::Id anId = listIds[k];

                if (anId >= listAllTypesConstsAndVariablesPosition.size()) return error(errorMsgs, "anId is out of bound: " + to_string(anId));
                if (listAllTypesConstsAndVariablesPosition[anId] == 0) return error(errorMsgs, "a type refers to an Id not defined yet: " + to_string(anId));
            }
        }

        start += wordCount;
    }

    //===============================================================================================================================================
    //===============================================================================================================================================
    //3rd step: check the shader compositions and their instances
    {
        vector<ShaderCompositionDeclaration*> vecCompositions;
        if (!spxBytecode.GetListAllCompositions(vecCompositions))
        {
            spxBytecode.copyMessagesTo(errorMsgs);
            return error(errorMsgs, "Failed to get the list of all compositions");
        }

        unsigned int countCompositions = (unsigned int)(vecCompositions.size());
        for (unsigned int ic = 0; ic < countCompositions; ic++)
        {
            ShaderCompositionDeclaration* aComposition = vecCompositions[ic];
            vector<ShaderClassData*> vecCompositionInstances;
            if (!spxBytecode.GetAllShaderInstancesForComposition(aComposition, vecCompositionInstances))
            {
                spxBytecode.copyMessagesTo(errorMsgs);
                return error(errorMsgs, "Failed to get the list of instances for the composition: " + aComposition->GetShaderOwnerAndVariableName());
            }

            unsigned int countInstances = (unsigned int)(vecCompositionInstances.size());
            if (countInstances != aComposition->countInstances)
                return error(errorMsgs, "Invalid number of instances for the composition: " + aComposition->GetShaderOwnerAndVariableName());
        }
    }

    return true;
}

bool SpxCompiler::ProcessFullBytecodeAndDataSanityCheck()
{
    bool success = ProcessBytecodeSanityCheck(spv, errorMessages);
    if (!success) return error("Failed to process the bytecode sanity check");

    return true;
}

bool SpxCompiler::ValidateSpxBytecodeAndData()
{
    if (!ValidateHeader()) return error("Failed to validate the header");

#ifdef XKSLANG_DEBUG_MODE
    //Debug sanity check: make sure that 2 shaders or 2 variables does not share the same name
    unordered_map<string, int> shaderVariablesDeclarationName;
    unordered_map<string, int> shadersFunctionsDeclarationName;
    unordered_map<string, int> shadersDeclarationName;

    for (auto itsh = vecAllShaders.begin(); itsh != vecAllShaders.end(); itsh++)
    {
        ShaderClassData* aShader = *itsh;
        if (shadersDeclarationName.find(aShader->GetName()) != shadersDeclarationName.end())
            return error(string("A shader already exists with the name: ") + aShader->GetName());
        shadersDeclarationName[aShader->GetName()] = 1;

        //2 different shaders cannot share an identical variable name
        for (auto itt = aShader->shaderTypesList.begin(); itt != aShader->shaderTypesList.end(); itt++)
        {
            ShaderTypeData* type = *itt;
            if (shaderVariablesDeclarationName.find(type->variable->GetName()) != shaderVariablesDeclarationName.end())
                return error(string("A variable already exists with the name: ") + type->variable->GetName());
            shaderVariablesDeclarationName[type->variable->GetName()] = 1;
        }
    }
#endif

    return true;
}

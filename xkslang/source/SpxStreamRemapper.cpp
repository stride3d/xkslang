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

#include "SpxStreamRemapper.h"

using namespace std;
using namespace xkslang;

//===================================================================================================//
static vector<string> staticErrorMessages;
static void copyStaticErrorMessagesTo(std::vector<std::string>& list)
{
    list.insert(list.end(), staticErrorMessages.begin(), staticErrorMessages.end());
}

void SpxStreamRemapper::copyMessagesTo(std::vector<std::string>& list)
{
    list.insert(list.end(), errorMessages.begin(), errorMessages.end());
}

void SpxStreamRemapper::error(const string& txt) const
{
    //we use a static vector first because we override a function defined as const, and parent class is calling this function >_<
    staticErrorMessages.push_back(txt);
}

bool SpxStreamRemapper::error(const std::string& txt)
{
    errorMessages.push_back(txt);
    return false;
}

static const auto spx_inst_fn_nop = [](spv::Op, unsigned) { return false; };
static const auto spx_op_fn_nop = [](spv::Id&) {};
//===================================================================================================//

SpxStreamRemapper::SpxStreamRemapper(int verbose) : spirvbin_t(verbose)
{
    staticErrorMessages.clear(); //clear the list of error messages that we will receive from the parent class
    status = SpxRemapperStatusEnum::WaitingForMixin;
}

SpxStreamRemapper::~SpxStreamRemapper()
{
    ReleaseAllMaps();
}

void SpxStreamRemapper::ReleaseAllMaps()
{
    mapDeclarationName.clear();
    int size = listAllObjects.size();
    for (int i = 0; i < size; ++i)
    {
        if (listAllObjects[i] != nullptr) delete listAllObjects[i];
    }
    listAllObjects.clear();
    vecAllShaders.clear();
    vecAllShaderFunctions.clear();
}

bool SpxStreamRemapper::MixWithSpxBytecode(const SpxBytecode& bytecode)
{
    if (status != SpxRemapperStatusEnum::WaitingForMixin) {
        return error("Invalid remapper status");
    }
    status = SpxRemapperStatusEnum::MixinInProgress;

    if (spv.size() == 0)
    {
        //just copy the full code into the remapper
        if (!SetBytecode(bytecode)) return false;

        if (!BuildAllMaps())
        {
            return error("Error building bytecode data map");
        }

        //Build the list of all overriding methods
        if (!BuildOverridenFunctionMap())
        {
            return error("Processing overriding functions failed");
        }

        //target function to the overriing functions
        if (!UpdateOpFunctionCallTargetsInstructionsToOverridingFunctions())
        {
            return error("Remapping overriding functions failed");
        }
    }
    else
    {
        //merge the new bytecode
        if (!MergeWithBytecode(bytecode)) return false;
    }

    if (errorMessages.size() > 0) return false;

    status = SpxRemapperStatusEnum::WaitingForMixin;
    return true;
}

bool SpxStreamRemapper::MergeWithBytecode(const SpxBytecode& bytecode)
{
    int maxCountIds = bound();

    //=============================================================================================================
    //Init
    unordered_map<uint32_t, pairIdPos> destinationBytecodeTypeHashMap;
    if (!this->BuildTypesAndConstsHashmap(destinationBytecodeTypeHashMap)) {
        return error("Error building type and const hashmap");
    }

    SpxStreamRemapper bytecodeToMerge;
    if (!bytecodeToMerge.SetBytecode(bytecode)) return false;
    bool res = bytecodeToMerge.BuildAllMaps();

    if (!res) {
        return error(string("Error building XKSL shaders data map from bytecode:") + bytecode.GetName());
    }

    //=============================================================================================================
    //=============================================================================================================
    //Get the list of shaders we want to merge (shaders not already in the destination bytecode)
    vector<ShaderClassData*> listShadersToMerge;
    for (auto itsh = bytecodeToMerge.vecAllShaders.begin(); itsh != bytecodeToMerge.vecAllShaders.end(); itsh++)
    {
        ShaderClassData* shaderToMerge = *itsh;
        if (!GetShaderByName(shaderToMerge->GetName()))
        {
            listShadersToMerge.push_back(shaderToMerge);
        }
    }

    if (errorMessages.size() > 0) return false;
    if (listShadersToMerge.size() == 0) return true;  //no new shaders to merge

    //=============================================================================================================
    //=============================================================================================================
    //Merge all the selected shaders
    int newId = bound();

    spirvbin_t vecNamesToMerge;
    spirvbin_t vecDecoratesToMerge;
    spirvbin_t vecTypesConstsAndVariablesToMerge;
    spirvbin_t vecFunctionsToMerge;

    vector<spv::Id> finalRemapTable;
    finalRemapTable.resize(bytecodeToMerge.bound(), unused);
    //keep the list of merged Ids so that we can look for their name or decorate
    vector<bool> newIdMerged;  
    newIdMerged.resize(bytecodeToMerge.bound(), false);

    for (int is=0; is<listShadersToMerge.size(); ++is)
    {
        ShaderClassData* shaderToMerge = listShadersToMerge[is];

        //=============================================================================================================
        //=============================================================================================================
        //merge all types and variables declared by the shader
        for (int t = 0; t < shaderToMerge->shaderTypesList.size(); ++t)
        {
            ShaderTypeData* shaderTypeToMerge = shaderToMerge->shaderTypesList[t];

            TypeInstruction* type = shaderTypeToMerge->type;
            TypeInstruction* pointerToType = shaderTypeToMerge->pointerToType;
            VariableInstruction* variable = shaderTypeToMerge->variable;

#ifdef XKSLANG_DEBUG_MODE
            if (finalRemapTable[type->GetId()] != unused) error(string("id:") + to_string(type->GetId()) + string(" has already been remapped"));
            if (finalRemapTable[pointerToType->GetId()] != unused) error(string("id:") + to_string(pointerToType->GetId()) + string(" has already been remapped"));
            if (finalRemapTable[variable->GetId()] != unused) error(string("id:") + to_string(variable->GetId()) + string(" has already been remapped"));
#endif

            finalRemapTable[type->GetId()] = newId++;
            bytecodeToMerge.CopyInstructionToVector(vecTypesConstsAndVariablesToMerge.spv, type->GetBytecodeStartPosition());
            finalRemapTable[pointerToType->GetId()] = newId++;
            bytecodeToMerge.CopyInstructionToVector(vecTypesConstsAndVariablesToMerge.spv, pointerToType->GetBytecodeStartPosition());
            finalRemapTable[variable->GetId()] = newId++;
            bytecodeToMerge.CopyInstructionToVector(vecTypesConstsAndVariablesToMerge.spv, variable->GetBytecodeStartPosition());
        }

        //Add the shader type declaration
        {
            const spv::Id resultId = shaderToMerge->GetId();

#ifdef XKSLANG_DEBUG_MODE
            if (finalRemapTable[resultId] != unused) error(string("id:") + to_string(resultId) + string(" has already been remapped"));
#endif

            finalRemapTable[resultId] = newId++;
            bytecodeToMerge.CopyInstructionToVector(vecTypesConstsAndVariablesToMerge.spv, shaderToMerge->GetBytecodeStartPosition());
        }

        //=============================================================================================================
        //=============================================================================================================
        //add all functions' instructions declared by the shader
        for (int t = 0; t < shaderToMerge->functionsList.size(); ++t)
        {
            FunctionInstruction* functionToMerge = shaderToMerge->functionsList[t];
            //finalRemapTable[functionToMerge->id] = newId++; //done below

            //For each instructions within the functions bytecode: Remap their results IDs
            bytecodeToMerge.process(
                [&](spv::Op opCode, unsigned start)
                {
                    unsigned word = start + 1;

                    // Read type and result ID from instruction desc table
                    if (spv::InstructionDesc[opCode].hasType()) {
                        word++;  //spv::Id typeId = bytecodeToMerge.asId(word++);
                    }

                    if (spv::InstructionDesc[opCode].hasResult()) {
                        spv::Id resultId = bytecodeToMerge.asId(word++);
#ifdef XKSLANG_DEBUG_MODE
                        if (finalRemapTable[resultId] != unused) error(string("id:") + to_string(resultId) + string(" has already been remapped"));
#endif
                        finalRemapTable[resultId] = newId++;
                    }

                    return true;
                },
                spx_op_fn_nop,
                functionToMerge->GetBytecodeStartPosition(), functionToMerge->GetBytecodeEndPosition()
            );

            //Copy all bytecode instructions from the function
            bytecodeToMerge.CopyInstructionToVector(vecFunctionsToMerge.spv, functionToMerge->GetBytecodeStartPosition(), functionToMerge->GetBytecodeEndPosition());
        }
    } //end shaderToMerge loop

    //update newIdMerged table (this table will define the name and decorate to fetch and merge)
    {
        int len = finalRemapTable.size();
        for (int i = 0; i < len; ++i)
        {
            if (finalRemapTable[i] != unused)
                newIdMerged[i] = true;
        }
    }

    //=============================================================================================================
    //=============================================================================================================
    //Check for all unmapped Ids called within the shader types/consts instructions that we have to merge
    spirvbin_t bytecodeWithExtraTypesToMerge;
    {
        //Init by checking all types instructions for unmapped IDs
        spirvbin_t bytecodeToCheckForUnmappedIds;
        bytecodeToCheckForUnmappedIds.spv.insert(bytecodeToCheckForUnmappedIds.spv.end(), vecTypesConstsAndVariablesToMerge.spv.begin(), vecTypesConstsAndVariablesToMerge.spv.end());
        bytecodeToCheckForUnmappedIds.spv.insert(bytecodeToCheckForUnmappedIds.spv.end(), vecFunctionsToMerge.spv.begin(), vecFunctionsToMerge.spv.end());
        vector<pairIdPos> listUnmappedIdsToProcess;  //unmapped ids and their pos in the bytecode to merge

        spv::Op opCode;
        unsigned wordCount;
        std::vector<spv::Id> listIds;
        spv::Id typeId, resultId;
        unsigned listIdsLen;
        unsigned pos = 0;
        const unsigned end = bytecodeToCheckForUnmappedIds.spv.size();
        while (pos < end)
        {
            listIds.clear();
            if (!bytecodeToCheckForUnmappedIds.parseInstruction(pos, opCode, wordCount, typeId, resultId, listIds))
                return error("Error parsing bytecodeToCheckForUnmappedIds");

            if (typeId != unused) listIds.push_back(typeId);
            listIdsLen = listIds.size();
            for (int i = 0; i < listIdsLen; ++i)
            {
                const spv::Id id = listIds[i];
                if (finalRemapTable[id] == unused)
                {
                    listUnmappedIdsToProcess.push_back(pairIdPos(id, -1));
                }
            }

            pos += wordCount;
        }

        //TypeData* typeToMerge;
        //ConstData* constToMerge;
        //VariableData* variableToAccess;
        while (listUnmappedIdsToProcess.size() > 0)
        {
            pairIdPos& unmappedIdPos = listUnmappedIdsToProcess.back();
            const spv::Id unmappedId = unmappedIdPos.first;
            if (finalRemapTable[unmappedId] != unused)
            {
                listUnmappedIdsToProcess.pop_back();
                continue;
            }

            //Find the position in the code to merge where the unmapped IDs is defined
            ObjectInstructionBase* objectFromUnmappedId = bytecodeToMerge.GetObjectForId(unmappedId);
            if (objectFromUnmappedId == nullptr) return error(string("No object is defined for the unmapped id:") + to_string(unmappedId));

            bool mappingResolved = false;
            bool mergeTypeOrConst = false;
            //uint32_t instructionPos = object->GetBytecodeStartPosition();
            switch (objectFromUnmappedId->GetKind())
            {
                case ObjectInstructionTypeEnum::Const:
                case ObjectInstructionTypeEnum::Type:
                {
                    mergeTypeOrConst = true;
                    uint32_t typeHash = bytecodeToMerge.hashType(objectFromUnmappedId->GetBytecodeStartPosition());
                    auto hashTypePosIt = destinationBytecodeTypeHashMap.find(typeHash);
                    if (hashTypePosIt != destinationBytecodeTypeHashMap.end())
                    {
                        //The type already exists in the destination bytecode, we can simply remap to it
                        mappingResolved = true;
                        finalRemapTable[unmappedId] = hashTypePosIt->second.first;
                    }
                    break;
                }

                default:
                    return error(string("Invalid object. unable to remap unmapped id:") + to_string(unmappedId));
            }

            if (mappingResolved)
            {
                listUnmappedIdsToProcess.pop_back();
            }
            else
            {
                if (mergeTypeOrConst)
                {
                    //The type doesn't exist yet, we will copy the full instruction, but only after checking that all depending IDs are mapped as well
                    bytecodeToCheckForUnmappedIds.spv.clear();
                    bytecodeToMerge.CopyInstructionToVector(bytecodeToCheckForUnmappedIds.spv, objectFromUnmappedId->GetBytecodeStartPosition());
                    listIds.clear();
                    if (!bytecodeToCheckForUnmappedIds.parseInstruction(0, opCode, wordCount, typeId, resultId, listIds))
                        return error("Error parsing bytecodeToCheckForUnmappedIds");

                    if (typeId != unused) listIds.push_back(typeId);
                    listIdsLen = listIds.size();
                    bool canAddTheInstruction = true;
                    for (int i = 0; i < listIdsLen; ++i)
                    {
                        const spv::Id anotherId = listIds[i];
#ifdef XKSLANG_DEBUG_MODE
                        if (anotherId == unmappedId) return error(string("anotherId == unmappedId:") + to_string(anotherId) + string(". This should be impossible (bytecode is invalid)"));
#endif
                        if (finalRemapTable[anotherId] == unused)
                        {
                            //we add anotherId to the list of Ids to process (we're depending on it)
                            listUnmappedIdsToProcess.push_back(pairIdPos(anotherId, -1));
                            canAddTheInstruction = false;
                        }
                    }

                    if (canAddTheInstruction)
                    {
                        //the instruction is not depending on another unmapped IDs anymore, we can copy it
                        finalRemapTable[unmappedId] = newId++;
                        listUnmappedIdsToProcess.pop_back();
                        bytecodeToMerge.CopyInstructionToVector(bytecodeWithExtraTypesToMerge.spv, objectFromUnmappedId->GetBytecodeStartPosition());

                        newIdMerged[unmappedId] = true;
                    }
                }
            }
        }  //end while (listUnmappedIdsToProcess.size() > 0)
    }  //end block

    //Add the extra types we merged at the beginning of our vec of type/const/variable
    if (bytecodeWithExtraTypesToMerge.spv.size() > 0)
    {
        vecTypesConstsAndVariablesToMerge.spv.insert(vecTypesConstsAndVariablesToMerge.spv.begin(), bytecodeWithExtraTypesToMerge.spv.begin(), bytecodeWithExtraTypesToMerge.spv.end());       
    }

    //=============================================================================================================
    //=============================================================================================================
    // get all name and decoration for the new IDs we need to merge
    bytecodeToMerge.process(
        [&](spv::Op opCode, unsigned start)
        {
            switch (opCode)
            {
                case spv::OpName:
                case spv::OpMemberName:
                {
                    const spv::Id id = bytecodeToMerge.asId(start + 1);
                    if (newIdMerged[id])
                    {
                        bytecodeToMerge.CopyInstructionToVector(vecNamesToMerge.spv, start);
                    }
                    break;
                }

                case spv::OpDecorate:
                case spv::OpMemberDecorate:
                {
                    const spv::Id id = bytecodeToMerge.asId(start + 1);
                    if (newIdMerged[id])
                    {
                        bytecodeToMerge.CopyInstructionToVector(vecDecoratesToMerge.spv, start);
                    }
                    break;
                }
            }
            return true;
        },
        spx_op_fn_nop
    );

    //=============================================================================================================
    //=============================================================================================================
    //remap IDs for all mergable types / variables / consts / functions
    vecTypesConstsAndVariablesToMerge.processOnFullBytecode(
        spx_inst_fn_nop,
        [&](spv::Id& id)
        {
            spv::Id newId = finalRemapTable[id];
            if (newId == unused) error(string("Invalid remapper Id:") + to_string(id));
            else id = newId;
        }
    );
    vecDecoratesToMerge.processOnFullBytecode(
        spx_inst_fn_nop,
        [&](spv::Id& id)
        {
            spv::Id newId = finalRemapTable[id];
            if (newId == unused) error(string("Invalid remapper Id:") + to_string(id));
            else id = newId;
        }
    );
    vecNamesToMerge.processOnFullBytecode(
        spx_inst_fn_nop,
        [&](spv::Id& id)
        {
            spv::Id newId = finalRemapTable[id];
            if (newId == unused) error(string("Invalid remapper Id:") + to_string(id));
            else id = newId;
        }
    );
    vecFunctionsToMerge.processOnFullBytecode(
        spx_inst_fn_nop,
        [&](spv::Id& id)
        {
            spv::Id newId = finalRemapTable[id];
            if (newId == unused) error(string("Invalid remapper Id:") + to_string(id));
            else id = newId;
        }
    );

    //=============================================================================================================
    //=============================================================================================================
    //merge all types / variables / consts in the current bytecode
    bound(newId);

    //get the best positions where we can merge
    unsigned int firstTypeOrConstPos = 0;
    unsigned int firstFunctionPos = 0;
    process(
        [&](spv::Op opCode, unsigned start)
        {
            if (firstTypeOrConstPos == 0)
            {
                if (isConstOp(opCode) || isTypeOp(opCode))
                {
                    firstTypeOrConstPos = start;
                }
            }
            if (opCode == spv::OpFunction)
            {
                if (firstFunctionPos == 0) firstFunctionPos = start;
            }
            return true;
        },
        spx_op_fn_nop
    );
    if (firstTypeOrConstPos == 0) firstTypeOrConstPos = header_size;
    if (firstFunctionPos == 0) spv.size();

    //=============================================================================================================
    //merge functions
    spv.insert(spv.end(), vecFunctionsToMerge.spv.begin(), vecFunctionsToMerge.spv.end());
    //Merge types and variables
    spv.insert(spv.begin() + firstFunctionPos, vecTypesConstsAndVariablesToMerge.spv.begin(), vecTypesConstsAndVariablesToMerge.spv.end());
    //Merge names and decorates
    spv.insert(spv.begin() + firstTypeOrConstPos, vecDecoratesToMerge.spv.begin(), vecDecoratesToMerge.spv.end());
    spv.insert(spv.begin() + firstTypeOrConstPos, vecNamesToMerge.spv.begin(), vecNamesToMerge.spv.end());

    //TOTO: reupdate all type, check for overrides!
    asdadadas;
    UpdateAllMaps();
    int klgjfdslkgjdlfkjgljdflgjs = 3454;

    if (errorMessages.size() > 0) return false;
    return true;
}

bool SpxStreamRemapper::SetBytecode(const SpxBytecode& bytecode)
{
    const std::vector<uint32_t>& spx = bytecode.getBytecodeStream();
    spv.clear();
    spv.insert(spv.end(), spx.begin(), spx.end());

    validate();  //validate the header
    if (staticErrorMessages.size() > 0) {
        copyStaticErrorMessagesTo(errorMessages);
        return false;
    }

    return true;
}

//Mixin is finalized: no more updates will be brought to the mixin bytecode after
bool SpxStreamRemapper::FinalizeMixin()
{
    if (status != SpxRemapperStatusEnum::WaitingForMixin) {
        return error("Invalid remapper status");
    }
    status = SpxRemapperStatusEnum::MixinBeingFinalized;

    validate();  //validate the header
    if (staticErrorMessages.size() > 0) {
        copyStaticErrorMessagesTo(errorMessages);
        return false;
    }

    //TOTO: TMP
    //BuildAllMaps();

    //Convert SPIRX extensions to SPIRV
    if (!ConvertSpirxToSpirVBytecode()) {
        return error("Failed to convert SPIRX to SPIRV");
    }

    status = SpxRemapperStatusEnum::MixinFinalized;
    return true;
}

bool SpxStreamRemapper::ConvertSpirxToSpirVBytecode()
{
    //Convert OpIntructions
    // - OpFunctionCallBase --> OpFunctionCall (remapping of override functions has been completed)

    std::vector<range_t> vecStripRanges;
    process(
        [&](spv::Op opCode, unsigned start)
        {
            switch (opCode) {
                case spv::OpFunctionCallBase:
                {
                    // change OpFunctionCallBase to OpFunctionCall
                    setOpCode(start, spv::OpFunctionCall);
                    break;
                }
                case spv::OpTypeXlslShaderClass:
                {
                    //remove OpTypeXlslShaderClass type
                    stripInst(start, vecStripRanges);
                    break;
                }
                case spv::OpName:
                case spv::OpDecorate:
                {
                    //id = asId(start + 1);
                    //if (id != spv::NoResult && isXkslShaderClassType[id] == 1)
                    //    stripInst(start);

                    //remove all XKSL decoration
                    const spv::Decoration dec = asDecoration(start + 2);
                    if (dec >= spv::DecorationDeclarationName && dec <= spv::DecorationMethodClone)
                        stripInst(start, vecStripRanges);
                    break;
                }
            }
            return true;
        },
        spx_op_fn_nop
    );

    stripBytecode(vecStripRanges);

    return true;
}

bool SpxStreamRemapper::UpdateOpFunctionCallTargetsInstructionsToOverridingFunctions()
{
    std::vector<FunctionInstruction*> vecFunctionIdBeingOverriden;
    vecFunctionIdBeingOverriden.resize(bound(), nullptr);
    bool anyOverridingFunction = false;
    for (auto itfn = vecAllShaderFunctions.begin(); itfn != vecAllShaderFunctions.end(); itfn++)
    {
        FunctionInstruction* function = *itfn;
        if (function->GetOverridingFunction() != nullptr)
        {
            vecFunctionIdBeingOverriden[function->GetResultId()] = function->GetOverridingFunction();
            anyOverridingFunction = true;
        }
    }

    if (!anyOverridingFunction) return true; //nothing to override

    process(
        [&](spv::Op opCode, unsigned start)
        {
            switch (opCode) {
                case spv::OpFunctionCall:
                {
                    spv::Id functionCalledId = asId(start + 3);

                    FunctionInstruction* overridingFunction = vecFunctionIdBeingOverriden[functionCalledId];
                    if (overridingFunction != nullptr)
                    {
                        spv::Id overridingFunctionId = overridingFunction->GetResultId();
                        spv[start + 3] = overridingFunctionId;
                    }

                    break;
                }
            }
            return true;
        },
        spx_op_fn_nop
    );

    return true;
}

bool SpxStreamRemapper::BuildOverridenFunctionMap()
{
    //========================================================================================================================//
    //========================================================================================================================//
    // check overrides for each functions declared with an override attribute
    // temporary implementation for now: simply override (replace) similar (same mangled name) functions from the function shader's parents classes
    for (auto itfn = vecAllShaderFunctions.begin(); itfn != vecAllShaderFunctions.end(); itfn++)
    {
        FunctionInstruction* overridingFunction = *itfn;
        if (!overridingFunction->HasAttributeOverride()) continue;

        const string& overringFunctionName = overridingFunction->GetMangledName();
        ShaderClassData* functionShaderOwner = overridingFunction->GetShaderOwner();
        if (functionShaderOwner == nullptr){
            return error(string("Overriding function does not belong to a known shader class:") + overringFunctionName);
        }

        //Check all parents classes for functions with same mangled name
        vector<ShaderClassData*> listShadersToCheckForOverrideWithinParents;
        listShadersToCheckForOverrideWithinParents.push_back(functionShaderOwner);

        while (listShadersToCheckForOverrideWithinParents.size() > 0)
        {
            ShaderClassData* shaderToCheckForOverride = listShadersToCheckForOverrideWithinParents.back();
            listShadersToCheckForOverrideWithinParents.pop_back();

            //Check all parents
            for (int p=0; p<shaderToCheckForOverride->parentsList.size(); ++p)
            {
                ShaderClassData* parent = shaderToCheckForOverride->parentsList[p];
                listShadersToCheckForOverrideWithinParents.push_back(parent);

                //check all functions belonging to the parent shader
                for (int f = 0; f<parent->functionsList.size(); ++f)
                {
                    //compare the shader's functions name with the overring function name
                    FunctionInstruction* aFunctionFromParent = parent->functionsList[f];
                    if (overringFunctionName == aFunctionFromParent->GetMangledName())
                    {
                        //Got a function to be overriden !
                        if (aFunctionFromParent->GetOverridingFunction() == nullptr ||
                            aFunctionFromParent->GetOverridingFunction()->GetShaderOwner()->level <= overridingFunction->GetShaderOwner()->level)
                        {
                            aFunctionFromParent->SetOverridingFunction(overridingFunction);
                        }
                    }
                }
            }
        }
    }

    if (errorMessages.size() > 0) return false;

    return true;
}

bool SpxStreamRemapper::GetMappedSpxBytecode(SpxBytecode& bytecode)
{
    if (spv.size() == 0)
    {
        return error("No code mapped");
    }

    std::vector<uint32_t>& bytecodeStream = bytecode.getWritableBytecodeStream();
    bytecodeStream.clear();
    bytecodeStream.insert(bytecodeStream.end(), spv.begin(), spv.end());

    return true;
}

bool SpxStreamRemapper::GenerateSpvStageBytecode(ShadingStage stage, std::string entryPointName, SpvBytecode& output)
{
    if (status != SpxRemapperStatusEnum::MixinFinalized)
    {
        return error("Invalid remapper status");
    }
    
    //==========================================================================================
    //==========================================================================================
    // Search for the shader entry point
    FunctionInstruction* entryPointFunction = nullptr;
    for (auto it = vecAllShaderFunctions.begin(); it != vecAllShaderFunctions.end(); it++)
    {
        FunctionInstruction* func = *it;
        string mangledFunctionName = func->GetMangledName();
        string unmangledFunctionName = mangledFunctionName.substr(0, mangledFunctionName.find('('));
        if (unmangledFunctionName == entryPointName)
        {
            entryPointFunction = func;
            //has the function been overriden?
            if (entryPointFunction->GetOverridingFunction() != nullptr) entryPointFunction = entryPointFunction->GetOverridingFunction();
            break;
        }
    }
    if (entryPointFunction == nullptr)
    {
        return error(string("Entry point not found: ") + entryPointName);
    }

    //==========================================================================================
    //==========================================================================================
    //save the current bytecode
    std::vector<uint32_t> bytecodeBackup;// = output.getWritableBytecodeStream();
    bytecodeBackup.insert(bytecodeBackup.end(), spv.begin(), spv.end());

    //==========================================================================================
    //==========================================================================================
    // Update the shader stage header
    if (!BuildAndSetShaderStageHeader(stage, entryPointFunction, entryPointName))
    {
        error("Error buiding the shader stage header");
        spv.clear(); spv.insert(spv.end(), bytecodeBackup.begin(), bytecodeBackup.end());
        return false;
    }

    //==========================================================================================
    //==========================================================================================
    //Clean and generate SPIRV bytecode

    buildLocalMaps();
    if (staticErrorMessages.size() > 0) {
        copyStaticErrorMessagesTo(errorMessages);
        spv.clear(); spv.insert(spv.end(), bytecodeBackup.begin(), bytecodeBackup.end());
        return false;
    }

    dceFuncs(); //dce uncalled functions
    dceVars();  //dce unused function variables + decorations / name
    dceTypes(); //dce unused types

    strip();         //remove all strip bytecode
    stripDeadRefs(); //remove references to things we DCEed

    //change the range of all remaining IDs
    mapRemainder(); // map any unmapped IDs
    applyMap();     // Now remap each shader to the new IDs we've come up with

    //copy the spv bytecode into the output
    std::vector<uint32_t>& outputSpv = output.getWritableBytecodeStream();
    outputSpv.clear();
    outputSpv.insert(outputSpv.end(), spv.begin(), spv.end());

    //reset the initial spx bytecode
    spv.clear();
    spv.insert(spv.end(), bytecodeBackup.begin(), bytecodeBackup.end());

    return true;
}

bool SpxStreamRemapper::BuildAndSetShaderStageHeader(ShadingStage stage, FunctionInstruction* entryFunction, string unmangledFunctionName)
{
    /*
    //capabilities
    //check which capabilities are required, check to merge capabilities from SPX bytecode
    spv::Instruction capInst(0, 0, spv::OpCapability);
    capInst.addImmediateOperand(spv::CapabilityShader);
    capInst.dump(stageHeader);

    //extensions?
    //spv::Instruction extInst(0, 0, spv::OpExtension);
    //extInst.addStringOperand(*it);
    //extInst.dump(stageHeader);

    //import
    spv::Instruction import(getUniqueId(), NoType, OpExtInstImport);
    import->addStringOperand(name);
    */

    if (entryFunction == nullptr)
    {
        return error("Unknown entry function");
    }

    spv::ExecutionModel model = GetShadingStageExecutionMode(stage);
    if (model == spv::ExecutionModelMax)
    {
        return error("Unknown stage");
    }

    //opEntryPoint: set the stage model, and entry point
    vector<unsigned int> stageHeader;
    spv::Instruction entryPointInstr(spv::OpEntryPoint);
    entryPointInstr.addImmediateOperand(model);
    entryPointInstr.addIdOperand(entryFunction->GetResultId());
    entryPointInstr.addStringOperand(unmangledFunctionName.c_str());
    entryPointInstr.dump(stageHeader);

    //remove all current entry points
    {
        std::vector<range_t> vecStripRanges;
        process(
            [&](spv::Op opCode, unsigned start)
            {
                if (opCode == spv::OpEntryPoint)
                    stripInst(start, vecStripRanges);
                return true;
            },
            spx_op_fn_nop
        );
        stripBytecode(vecStripRanges);
    }

    //insert the stage header in the bytecode, after the header
    vector<unsigned int>::iterator it = spv.begin() + header_size;
    spv.insert(it, stageHeader.begin(), stageHeader.end());

    return true;
}

spv::ExecutionModel SpxStreamRemapper::GetShadingStageExecutionMode(ShadingStage stage)
{
    switch (stage) {
    case ShadingStage::Vertex:           return spv::ExecutionModelVertex;
    case ShadingStage::Pixel:            return spv::ExecutionModelFragment;
    case ShadingStage::TessControl:      return spv::ExecutionModelTessellationControl;
    case ShadingStage::TessEvaluation:   return spv::ExecutionModelTessellationEvaluation;
    case ShadingStage::Geometry:         return spv::ExecutionModelGeometry;
    case ShadingStage::Compute:          return spv::ExecutionModelGLCompute;
    default:
        return spv::ExecutionModelMax;
    }
}

bool SpxStreamRemapper::BuildTypesAndConstsHashmap(unordered_map<uint32_t, pairIdPos>& mapHashPos)
{
    mapHashPos.clear();

    if (idPosR.size() == 0)
    {
        return error("Fail to execute BuildTypesAndConstsHashmap: call BuildAllMaps first");
    }

    process(
        [&](spv::Op opCode, unsigned start)
        {
            spv::Id id = unused;
            if (isConstOp(opCode))
            {
                id = asId(start + 2);
            }
            else if (isTypeOp(opCode))
            {
                id = asId(start + 1);
            }

            if (id != unused)
            {
                const uint32_t hashval = hashType(start);
                mapHashPos[hashval] = pairIdPos(id, start);
            }
            return true;
        },
        spx_op_fn_nop
    );

    return true;
}

/*
bool SpxStreamRemapper::UpdateAllMaps()
{
    unordered_map<spv::Id, range_t> functionPos;
}
*/

bool SpxStreamRemapper::BuildAllMaps()
{
    //cout << "BuildAllMaps!!" << endl;

    ReleaseAllMaps();
    
    std::vector<ParsedObjectData> listParsedObjectsData;
    bool res = BuildDeclarationNameMapsAndObjectsDataList(listParsedObjectsData);
    if (!res) {
        return error("Failed to build maps");
    }

    //======================================================================================================
    //create all objects
    int maxResultId = bound();
    listAllObjects.resize(maxResultId, nullptr);
    int countParsedObjects = listParsedObjectsData.size();
    for (int i = 0; i < countParsedObjects; ++i)
    {
        ParsedObjectData& parsedData = listParsedObjectsData[i];
        
        spv::Id resultId = parsedData.resultId;
        if (resultId <= 0 || resultId == spv::NoResult || resultId >= maxResultId)
            return error(string("The object has an invalid resultId:") + to_string(resultId));
        if (listAllObjects[resultId] != nullptr)
            return error(string("An object with the same resultId already exists. resultId:") + to_string(resultId));

        string name;
        bool declarationNameRequired = true;
        ObjectInstructionBase* newObject = nullptr;

        bool hasDeclarationName = GetDeclarationNameForId(parsedData.resultId, name);

        switch (parsedData.kind)
        {
            case ObjectInstructionTypeEnum::Const:
            {
                declarationNameRequired = false;
                newObject = new ConstInstruction(parsedData, name);
                break;
            }
            case ObjectInstructionTypeEnum::Shader:
            {
                declarationNameRequired = true;
                ShaderClassData* shader = new ShaderClassData(parsedData, name);
                vecAllShaders.push_back(shader);
                newObject = shader;
                break;
            }
            case ObjectInstructionTypeEnum::Type:
            {
                declarationNameRequired = false;
                TypeInstruction* type = new TypeInstruction(parsedData, name);
                if (isPointerTypeOp(parsedData.opCode))
                {
                    //create the link to the type pointed by the pointer (already created at this stage)
                    TypeInstruction* pointedType = GetTypeById(parsedData.targetId);
                    if (pointedType == nullptr)
                        return error(string("Cannot find the typeId:") + to_string(parsedData.targetId) + string(", pointed by pointer Id:") + to_string(resultId));
                    type->SetTypePointed(pointedType);
                }
                newObject = type;

                break;
            }
            case ObjectInstructionTypeEnum::Variable:
            {
                declarationNameRequired = true;
                VariableInstruction* variable = new VariableInstruction(parsedData, name);

                //create the link to the type pointed by the variable (already created at this stage)
                TypeInstruction* pointedType = GetTypeById(parsedData.typeId);
                if (pointedType == nullptr)
                    return error(string("Cannot find the typeId:") + to_string(parsedData.typeId) + string(", pointed by variable Id:") + to_string(resultId));

                variable->SetTypePointed(pointedType);
                newObject = variable;
                break;
            }
            case ObjectInstructionTypeEnum::Function:
            {
                declarationNameRequired = false;  //some functions can be declared outside a shader definition, they don't belong to a shader then
                FunctionInstruction* function = new FunctionInstruction(parsedData, name);

                if (hasDeclarationName && name.size() > 0)
                {
                    vecAllShaderFunctions.push_back(function);
                }

                newObject = function;
                break;
            }
        }

        listAllObjects[resultId] = newObject;

        if (newObject == nullptr) return error("Unknown parsed data kind");
        if (declarationNameRequired && !hasDeclarationName) return error("Object requires a declaration name");
    }

    if (errorMessages.size() > 0) return false;

    //======================================================================================================
    // Decorate objects
    process(
        [&](spv::Op opCode, unsigned start) {
            unsigned word = start + 1;

            if (opCode == spv::Op::OpDecorate) {
                const spv::Id targetId = asId(start + 1);
                const spv::Decoration dec = asDecoration(start + 2);

                switch (dec)
                {
                    case spv::DecorationMethodOverride:
                    {
                        //a function is defined with an override attribute
                        FunctionInstruction* function = GetFunctionById(targetId);
                        if (function == nullptr) {error(string("undeclared function id:") + to_string(targetId)); break;}
                        function->SetAttributeOverride(true);
                        break;
                    }

                    case spv::DecorationBelongsToShader:
                    {
                        FunctionInstruction* function = GetFunctionById(targetId);
                        const std::string ownerName = literalString(start + 3);
                        ShaderClassData* shaderOwner = GetShaderByName(ownerName);
                        if (shaderOwner == nullptr) { error(string("undeclared shader owner:") + ownerName); break; }

                        if (function != nullptr) {
                            //a function is defined as being owned by a shader
#ifdef XKSLANG_DEBUG_MODE
                            if (function->GetShaderOwner() != nullptr) {error(string("function already has a shader owner:") + function->GetMangledName()); break;}
                            if (shaderOwner->HasFunction(function))
                            { error(string("The shader:") + ownerName + string(" already possesses the function:") + function->GetMangledName()); break; }
#endif
                            function->SetShaderOwner(shaderOwner);
                            shaderOwner->AddFunction(function);
                        }
                        else
                        {
                            //a type is defined as being owned by a shader
                            TypeInstruction* type = GetTypeById(targetId);
                            if (type != nullptr)
                            {
                                //A type belongs to a shader, we fetch the necessary data
                                //A shader type is defined by: the type, a pointer type to it, and a variable to access it
                                TypeInstruction* typePointer = GetTypePointingTo(type);
                                VariableInstruction* variable = GetVariablePointingTo(typePointer);

                                if (typePointer == nullptr) { error(string("cannot find the pointer type to the shader type:") + type->GetName()); break; }
                                if (variable == nullptr) { error(string("cannot find the variable for shader type:") + type->GetName()); break; }
#ifdef XKSLANG_DEBUG_MODE
                                if (type->GetShaderOwner() != nullptr) { error(string("type already has a shader owner:") + type->GetName()); break; }
                                if (shaderOwner->HasType(type)) { error(string("shader:") + ownerName + string(" already possesses the type:") + type->GetName()); break; }
#endif
                                ShaderTypeData* shaderType = new ShaderTypeData(type, typePointer, variable);
                                type->SetShaderOwner(shaderOwner);
                                shaderOwner->AddShaderType(shaderType);
                            }
                            else
                            {
                                error(string("unprocessed DecorationBelongsToShader operand, invalid Id:") + to_string(targetId));
                            }
                        }
                        break;
                    }

                    case spv::DecorationShaderInheritFromParent:
                    {
                        //A shader inherits from a parent
                        ShaderClassData* shader = GetShaderById(targetId);
                        const std::string parentName = literalString(start + 3);
                        ShaderClassData* shaderParent = GetShaderByName(parentName);

                        if (shader == nullptr) { error(string("undeclared shader id:") + to_string(targetId)); break; }
                        if (shaderParent == nullptr) { error(string("undeclared parent shader:") + parentName); break; }
#ifdef XKSLANG_DEBUG_MODE
                        if (shader->HasParent(shaderParent)) {error(string("shader:") + shader->GetName() + string(" already inherits from parent:") + parentName); break;}
#endif

                        shader->AddParent(shaderParent);
                        break;
                    }
                }

            }
            return true;
        },
        spx_op_fn_nop
    );

    if (errorMessages.size() > 0) return false;

    //========================================================================================================================//
    // Set the shader levels (also detects cyclic shader inheritance)
    {
        for (auto itsh = vecAllShaders.begin(); itsh != vecAllShaders.end(); itsh++)
            (*itsh)->level = -1;

        bool allShaderSet = false;
        while (!allShaderSet)
        {
            allShaderSet = true;
            bool anyShaderUpdated = false;

            for (auto itsh = vecAllShaders.begin(); itsh != vecAllShaders.end(); itsh++)
            {
                ShaderClassData* shader = *itsh;
                if (shader->level != -1) continue;  //shader already set

                if (shader->parentsList.size() == 0)
                {
                    shader->level = 0; //shader has no parent
                    anyShaderUpdated = true;
                    continue;
                }

                int maxParentLevel = -1;
                for (int p = 0; p<shader->parentsList.size(); ++p)
                {
                    int parentLevel = shader->parentsList[p]->level;
                    if (parentLevel == -1)
                    {
                        //parent not set yet: got to wait
                        allShaderSet = false;
                        maxParentLevel = -1;
                        break;
                    }
                    if (parentLevel > maxParentLevel) maxParentLevel = parentLevel;
                }

                if (maxParentLevel >= 0)
                {
                    shader->level = maxParentLevel + 1; //shader level
                    anyShaderUpdated = true;
                }
            }

            if (!anyShaderUpdated)
            {
                return error("Cyclic inheritance detected among shaders");
            }
        }
    }

    return true;
}

bool SpxStreamRemapper::BuildDeclarationNameMapsAndObjectsDataList(vector<ParsedObjectData>& listParsedObjectsData)
{
    mapDeclarationName.clear();
    idPosR.clear();
    listParsedObjectsData.clear();

    int fnStart = 0;
    spv::Id fnResId = spv::NoResult;
    spv::Id fnTypeId = spv::NoResult;
    spv::Op fnOpCode;
    process(
        [&](spv::Op opCode, unsigned start)
        {
            unsigned end = start + asWordCount(start);

            //Fill idPosR map (used by hashType)
            unsigned word = start+1;
            spv::Id typeId = spv::NoType;
            spv::Id resultId = spv::NoResult;
            if (spv::InstructionDesc[opCode].hasType())
                typeId = asId(word++);
            if (spv::InstructionDesc[opCode].hasResult()) {
                resultId = asId(word++);
                idPosR[resultId] = start;
            }

            if (opCode == spv::Op::OpName)
            {
            /*
#ifdef XKSLANG_DEBUG_MODE
                const spv::Id target = asId(start + 1);
                const std::string name = literalString(start + 2);

                if (mapDeclarationName.find(target) == mapDeclarationName.end())
                    mapDeclarationName[target] = name;
#endif*/
            }
            else if (opCode == spv::Op::OpDecorate)
            {
                const spv::Id target = asId(start + 1);
                const spv::Decoration dec = asDecoration(start + 2);

                switch (dec)
                {
                    case spv::DecorationDeclarationName:
                    {
                        const std::string  name = literalString(start + 3);
                        mapDeclarationName[target] = name;
                        break;
                    }
                }
            }
            else if (isConstOp(opCode))
            {
                listParsedObjectsData.push_back(ParsedObjectData(ObjectInstructionTypeEnum::Const, opCode, resultId, typeId, start, end));
            }
            else if (isTypeOp(opCode))
            {
                if (opCode == spv::OpTypeXlslShaderClass)
                {
                    listParsedObjectsData.push_back(ParsedObjectData(ObjectInstructionTypeEnum::Shader, opCode, resultId, typeId, start, end));
                }
                else
                {
                    ParsedObjectData data = ParsedObjectData(ObjectInstructionTypeEnum::Type, opCode, resultId, typeId, start, end);
                    if (isPointerTypeOp(opCode))
                    {
                        spv::Id targetId = asId(start + 3);
                        data.SetTargetId(targetId);
                    }
                    listParsedObjectsData.push_back(data);
                }
            }
            else if (isVariableOp(opCode))
            {
                listParsedObjectsData.push_back(ParsedObjectData(ObjectInstructionTypeEnum::Variable, opCode, resultId, typeId, start, end));
            }
            else if (opCode == spv::Op::OpFunction)
            {
                if (fnStart != 0) error("nested function found");
                fnStart = start;
                fnTypeId = typeId;
                fnResId = resultId;
                fnOpCode = opCode;
            }
            else if (opCode == spv::Op::OpFunctionEnd)
            {
                if (fnStart == 0) error("function end without function start");
                if (fnResId == spv::NoResult) error("function has no result iD");
                listParsedObjectsData.push_back(ParsedObjectData(ObjectInstructionTypeEnum::Function, fnOpCode, fnResId, fnTypeId, fnStart, end));
                fnStart = 0;
            }
            return true;
        },
        spx_op_fn_nop
    );

    if (errorMessages.size() > 0) return false;
    return true;
}

//===========================================================================================================================//
//===========================================================================================================================//
string SpxStreamRemapper::GetDeclarationNameForId(spv::Id id)
{
    auto it = mapDeclarationName.find(id);
    if (it == mapDeclarationName.end())
    {
        error(string("Id:") + to_string(id) + string(" has no declaration name"));
        return string("");
    }
    return it->second;
}

bool SpxStreamRemapper::GetDeclarationNameForId(spv::Id id, string& name)
{
    auto it = mapDeclarationName.find(id);
    if (it == mapDeclarationName.end())
        return false;

    name = it->second;
    return true;
}

SpxStreamRemapper::ShaderClassData* SpxStreamRemapper::GetShaderByName(const std::string& name)
{
    int size = vecAllShaders.size();
    for (int i = 0; i < size; ++i)
    {
        ShaderClassData* shader = vecAllShaders[i];
        if (shader->GetName() == name)
        {
            return shader;
        }
    }
    return nullptr;
}

SpxStreamRemapper::ObjectInstructionBase* SpxStreamRemapper::GetObjectForId(spv::Id id)
{
    if (id < 0 || id >= listAllObjects.size()) return nullptr;
    ObjectInstructionBase* obj = listAllObjects[id];
    return obj;
}

SpxStreamRemapper::ShaderClassData* SpxStreamRemapper::GetShaderById(spv::Id id)
{
    if (id < 0 || id >= listAllObjects.size()) return nullptr;
    ObjectInstructionBase* obj = listAllObjects[id];

    if (obj != nullptr && obj->GetKind() == ObjectInstructionTypeEnum::Shader)
    {
        ShaderClassData* shader = dynamic_cast<ShaderClassData*>(obj);
        return shader;
    }
    return nullptr;
}

SpxStreamRemapper::FunctionInstruction* SpxStreamRemapper::GetFunctionById(spv::Id id)
{
    if (id < 0 || id >= listAllObjects.size()) return nullptr;
    ObjectInstructionBase* obj = listAllObjects[id];

    if (obj != nullptr && obj->GetKind() == ObjectInstructionTypeEnum::Function)
    {
        FunctionInstruction* function = dynamic_cast<FunctionInstruction*>(obj);
        return function;
    }
    return nullptr;
}

SpxStreamRemapper::TypeInstruction* SpxStreamRemapper::GetTypeById(spv::Id id)
{
    if (id < 0 || id >= listAllObjects.size()) return nullptr;
    ObjectInstructionBase* obj = listAllObjects[id];

    if (obj != nullptr && obj->GetKind() == ObjectInstructionTypeEnum::Type)
    {
        TypeInstruction* type = dynamic_cast<TypeInstruction*>(obj);
        return type;
    }
    return nullptr;
}

SpxStreamRemapper::TypeInstruction* SpxStreamRemapper::GetTypePointingTo(TypeInstruction* targetType)
{
    TypeInstruction* res = nullptr;
    int size = listAllObjects.size();
    for (int i = 0; i < size; ++i)
    {
        ObjectInstructionBase* obj = listAllObjects[i];
        if (obj != nullptr && obj->GetKind() == ObjectInstructionTypeEnum::Type)
        {
            TypeInstruction* aType = dynamic_cast<TypeInstruction*>(obj);
            if (aType->GetTypePointed() == targetType)
            {
                if (res != nullptr) error("found 2 types pointing to the same type");
                res = aType;
            }
        }
    }
    return res;
}

SpxStreamRemapper::VariableInstruction* SpxStreamRemapper::GetVariablePointingTo(TypeInstruction* targetType)
{
    VariableInstruction* res = nullptr;
    int size = listAllObjects.size();
    for (int i = 0; i < size; ++i)
    {
        ObjectInstructionBase* obj = listAllObjects[i];
        if (obj != nullptr && obj->GetKind() == ObjectInstructionTypeEnum::Variable)
        {
            VariableInstruction* variable = dynamic_cast<VariableInstruction*>(obj);
            if (variable->GetTypePointed() == targetType)
            {
                if (res != nullptr) error("found 2 variables pointing to the same type");
                res = variable;
            }
        }
    }
    return res;
}

void SpxStreamRemapper::stripBytecode(vector<range_t>& ranges)
{
    if (ranges.empty()) // nothing to do
        return;

    // Sort strip ranges in order of traversal
    std::sort(ranges.begin(), ranges.end());

    // Allocate a new binary big enough to hold old binary
    // We'll step this iterator through the strip ranges as we go through the binary
    auto strip_it = ranges.begin();

    int strippedPos = 0;
    for (unsigned word = 0; word < unsigned(spv.size()); ++word) {
        if (strip_it != ranges.end() && word >= strip_it->second)
            ++strip_it;

        if (strip_it == ranges.end() || word < strip_it->first || word >= strip_it->second)
            spv[strippedPos++] = spv[word];
    }

    spv.resize(strippedPos);
}
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

bool SpxCompiler::GetBytecodeReflectionData(EffectReflection& effectReflection)
{
    effectReflection.Clear();

    //Find all output stages entry point
    vector<OutputStageEntryPoint> listEntryPoints;
    {
        unsigned int start = header_size;
        const unsigned int end = (unsigned int)spv.size();
        while (start < end)
        {
            unsigned int wordCount = asWordCount(start);
            spv::Op opCode = asOpCode(start);

            switch (opCode)
            {
                case spv::OpEntryPoint:
                {
                    spv::ExecutionModel model = (spv::ExecutionModel)asLiteralValue(start + 1);
                    ShadingStageEnum stage = GetShadingStageForExecutionMode(model);

                    spv::Id entryFunctionId = asId(start + 2);
                    FunctionInstruction* entryFunction = GetFunctionById(entryFunctionId);
                    
                    if (stage == ShadingStageEnum::Undefined) return error("Failed to read the stage from the entry point");
                    if (entryFunction == nullptr) return error("Failed to find the entry function for id: " + to_string(entryFunctionId));

                    listEntryPoints.push_back(OutputStageEntryPoint(stage, entryFunction));
                    break;
                }

                case spv::OpName:
                case spv::OpMemberName:
                case spv::OpFunction:
                case spv::OpConstant:
                {
                    start = end;
                    break;
                }
            }
            start += wordCount;
        }
        if (listEntryPoints.size() == 0) return error("No entry points found in the compiled bytecode");
    }

    bool success;
    success = GetAllCBufferAndResourcesBindingsReflectionDataFromBytecode(effectReflection, listEntryPoints);
    if (!success) return error("Failed to get the CBuffer and ResourcesBinding reflection data from the bytecode");

	success = GetInputAttributesFromBytecode(effectReflection, listEntryPoints);
	if (!success) return error("Failed to get the Input Attributes reflection data from the bytecode");

    return true;
}

bool SpxCompiler::GetInputAttributesFromBytecode(EffectReflection& effectReflection, std::vector<OutputStageEntryPoint>& listEntryPoints)
{
	bool success = true;

	if (effectReflection.InputAttributes != nullptr || effectReflection.CountInputAttributes != 0) { return error("effectReflection.InputAttributes has not been properly released"); }

	// the first stage that exists is the entry point in the pipeline for the streams
	OutputStageEntryPoint& firstOutputStage = listEntryPoints[0];
	for (unsigned int iStage = 1; iStage < listEntryPoints.size(); ++iStage)
	{
		if ((int)(listEntryPoints[iStage].stage) < (int)(firstOutputStage.stage)) firstOutputStage = listEntryPoints[iStage];
	}
	FunctionInstruction* firstStageEntryFunction = firstOutputStage.entryFunction;

	vector<VariableInstruction*> vecInputVariables;
	vector<VariableInstruction*> mapStreamInputVariableById;
	mapStreamInputVariableById.resize(bound(), nullptr);

	//Find the first stage input attributes: all input streams are accessed (loaded) within the stage main entry function
	{
		unsigned int start = firstStageEntryFunction->bytecodeStartPosition;
		const unsigned int end = firstStageEntryFunction->bytecodeEndPosition;
		while (start < end)
		{
			unsigned int wordCount = asWordCount(start);
			spv::Op opCode = asOpCode(start);
			switch (opCode)
			{
				case spv::OpLoad:
				{
					spv::Id idLoaded = asId(start + 3);

					//are we accessing a variable?
					VariableInstruction* variable = GetVariableById(idLoaded);
					if (variable != nullptr)
					{
						//is it an input variable?
						spv::StorageClass storageClass = (spv::StorageClass)asLiteralValue(variable->bytecodeStartPosition + 3);
						if (storageClass == spv::StorageClassInput)
						{
#ifdef XKSLANG_DEBUG_MODE
							if (variable->variableData != nullptr) { error("The input variable already has some variable data allocated"); break; }
							if (variable->GetId() >= mapStreamInputVariableById.size()) { error("Invalid variable id"); break; }
							if (mapStreamInputVariableById[variable->GetId()] != nullptr) { error("a stream input variable is loaded more than once"); break; }
#endif
							variable->variableData = new VariableData();
							vecInputVariables.push_back(variable);
							mapStreamInputVariableById[variable->GetId()] = variable;
						}
					}
					break;
				}
			}

			start += wordCount;
		} 
	}

	//Find the input variable semantic name
    {
        unsigned int start = header_size;
        const unsigned int end = (unsigned int)spv.size();
        while (start < end)
        {
            unsigned int wordCount = asWordCount(start);
            spv::Op opCode = asOpCode(start);

            switch (opCode)
            {
                case spv::OpSemanticName:
                {
                    spv::Id id = asId(start + 1);
#ifdef XKSLANG_DEBUG_MODE
                    if (id >= (unsigned int)mapStreamInputVariableById.size()) { error("Id out of bound: " + to_string(id)); break; }
#endif
                    if (mapStreamInputVariableById[id] != nullptr)
                    {
						VariableInstruction* variable = mapStreamInputVariableById[id];
						variable->variableData->semanticName = literalString(start + 2);
                    }
                    break;
                }

				case spv::OpDecorate:
				{
					spv::Id id = asId(start + 1);
					spv::Decoration decoration = (spv::Decoration)asLiteralValue(start + 2);
					if (decoration == spv::Decoration::DecorationLocation)
					{
#ifdef XKSLANG_DEBUG_MODE
						if (id >= (unsigned int)mapStreamInputVariableById.size()) { error("Id out of bound: " + to_string(id)); break; }
#endif
						if (mapStreamInputVariableById[id] != nullptr)
						{
							VariableInstruction* variable = mapStreamInputVariableById[id];
							variable->variableData->semanticIndex = asLiteralValue(start + 3);
						}
					}
					break;
				}

                case spv::OpFunction:
                case spv::OpTypeFunction:
                {
                    start = end;
                    break;
                }
            }
            start += wordCount;
        }
    }
	if (errorMessages.size() > 0) success = false;

	//Create the inputAttributes
	if (success)
	{
		effectReflection.CountInputAttributes = vecInputVariables.size();
		if (effectReflection.CountInputAttributes > 0) effectReflection.InputAttributes = new ShaderInputAttributeDescription[effectReflection.CountInputAttributes];
		for (int k = 0; k < effectReflection.CountInputAttributes; k++)
		{
			VariableData* variableData = vecInputVariables[k]->variableData;
			effectReflection.InputAttributes[k] = ShaderInputAttributeDescription(variableData->semanticIndex, variableData->semanticName);
		}
	}

	//delete allocated data
	for (auto itrv = vecInputVariables.begin(); itrv != vecInputVariables.end(); itrv++)
	{
		VariableInstruction* variable = *itrv;
		if (variable->variableData != nullptr)
		{
			delete variable->variableData;
			variable->variableData = nullptr;
		}
	}

	return success;
}

bool SpxCompiler::GetAllCBufferAndResourcesBindingsReflectionDataFromBytecode(EffectReflection& effectReflection, vector<OutputStageEntryPoint>& listEntryPoints)
{
    bool success = true;

	if (effectReflection.ConstantBuffers != nullptr || effectReflection.CountConstantBuffers != 0) { return error("effectReflection.ConstantBuffers has not been properly released"); }
	if (effectReflection.ResourceBindings != nullptr || effectReflection.CountResourceBindings != 0) { return error("effectReflection.ResourceBindings has not been properly released"); }

#ifdef XKSLANG_DEBUG_MODE
	if (listEntryPoints.size() == 0) return error("No output stage entry points");
#endif

    //=========================================================================================================================
    //=========================================================================================================================
    //get ALL cbuffers and resources
    // -a CBuffer is defined when a struct type has a Block decorate (those have already been parsed and set when building all maps)
    // - a resources is defined by a UniformConstant variable
    vector<CBufferTypeData*> listAllCBuffers;
    vector<CBufferTypeData*> vectorCBuffersIds;
    vector<VariableInstruction*> listAllResourceVariables;
    vector<VariableInstruction*> vectorResourceVariablesId;
    vectorCBuffersIds.resize(bound(), nullptr);
    vectorResourceVariablesId.resize(bound(), nullptr);

    for (auto it = listAllObjects.begin(); it != listAllObjects.end(); ++it)
    {
        ObjectInstructionBase* obj = *it;
        if (obj == nullptr) continue;

        if (obj->GetKind() == ObjectInstructionTypeEnum::Type)
        {
            TypeInstruction* type = dynamic_cast<TypeInstruction*>(obj);
            if (type->IsCBuffer())
            {
                CBufferTypeData* cbufferData = type->GetCBufferData();
                if (cbufferData == nullptr)
                {
                    spv::Op opCode = asOpCode(type->GetBytecodeStartPosition());
                    int wordCount = asWordCount(type->GetBytecodeStartPosition());
                    int countMembers = wordCount - 2;

#ifdef XKSLANG_DEBUG_MODE
                    if (opCode != spv::OpTypeStruct) { error(string("Invalid OpCode type for the cbuffer instruction (expected OpTypeStruct): ") + OpcodeString(opCode)); break; }
                    if (countMembers <= 0 || countMembers > compilerSettings_maxStructMembers) { error("invalid cbuffer member count"); break; }
#endif

                    //we can have a cbuffer with no cbufferData: the compiled bytecode not necessarily has OpCBufferProperties instruction
                    cbufferData = new CBufferTypeData(nullptr, type->GetId(), type->GetName(), "", false, true, countMembers);
                    type->SetCBufferData(cbufferData);
                }

#ifdef XKSLANG_DEBUG_MODE
                if (cbufferData == nullptr) { error("cbuffer data missing!"); break; }
                if (cbufferData->cbufferMembersData != nullptr) { error("a cbuffer members data has already been initialized"); break; }
                if (cbufferData->cbufferCountMembers <= 0) { error("invalid cbuffer member count"); break; }
#endif

                cbufferData->cbufferTypeObject = type;
                cbufferData->cbufferPointerTypeObject = GetTypePointingTo(type);
                cbufferData->cbufferVariableTypeObject = GetVariablePointingTo(cbufferData->cbufferPointerTypeObject);
                if (cbufferData->cbufferVariableTypeObject == nullptr) { error("Failed to find the variable pointing to the cbuffer: " + to_string(type->GetId())); break; }

#ifdef XKSLANG_DEBUG_MODE
                if (type->GetId() >= vectorCBuffersIds.size()) { error("cbuffer type id is out of bound. Id: " + to_string(type->GetId())); break; }
                if (cbufferData->cbufferVariableTypeObject->GetId() >= vectorCBuffersIds.size()) { error("cbuffer variable type id is out of bound. Id: " + to_string(type->GetId())); break; }
#endif

                cbufferData->cbufferMembersData = new TypeStructMemberArray();
                cbufferData->cbufferMembersData->members.resize(cbufferData->cbufferCountMembers);
                cbufferData->isUsed = true;

                listAllCBuffers.push_back(cbufferData);
                vectorCBuffersIds[type->GetId()] = cbufferData;
                //vectorCBuffersIds[cbufferData->cbufferVariableTypeObject->GetId()] = cbufferData;
            }
        }
        else if (obj->GetKind() == ObjectInstructionTypeEnum::Variable)
        {
            VariableInstruction* variable = dynamic_cast<VariableInstruction*>(obj);
            spv::StorageClass storageClass = (spv::StorageClass)asLiteralValue(variable->GetBytecodeStartPosition()  + 3);
            if (storageClass == spv::StorageClass::StorageClassUniformConstant)
            {
                spv::Id variableId = variable->GetResultId();
#ifdef XKSLANG_DEBUG_MODE
                if (variableId >= vectorResourceVariablesId.size()) { error("the ressource variable id is out of bound. Id: " + to_string(variableId)); break; }
                if (vectorResourceVariablesId[variableId] != nullptr) { error("a ressource variable is defined more than once. Id: " + to_string(variableId)); break; }
#endif
                if (variable->variableData != nullptr) { error("a ressource variable already has some data set. Id: " + to_string(variableId)); break; }

                variable->variableData = new VariableData();
                vectorResourceVariablesId[variableId] = variable;
                listAllResourceVariables.push_back(variable);

                //=======================================================
                //Find the variable type and class
                TypeInstruction* variablePointerType = variable->GetTypePointed();
                if (variablePointerType == nullptr) {
                    error("a variable is missing its pointer type. variable id: " + to_string(variable->GetId()));
                    break;
                }
                TypeInstruction* variableType = variablePointerType->GetTypePointed();
                if (variableType == nullptr) {
                    error("a variable pointer type is missing its pointed type. variable pointer id: " + to_string(variablePointerType->GetId()));
                    break;
                }

                TypeReflectionDescription resourceTypeReflection;
                if (!GetTypeReflectionDescription(variableType, false, nullptr, variable->variableData->variableTypeReflection, nullptr))
                {
                    error("Failed to get the reflexon data for the resource type: " + to_string(variable->GetId()));
                    break;
                }
            }
        }
    }
    if (errorMessages.size() > 0) success = false;
    if (listAllCBuffers.size() == 0 && listAllResourceVariables.size() == 0) return success;

    //=========================================================================================================================
    //=========================================================================================================================
    //Retrieve information from the bytecode for all variables and all cbuffers and their members
    if (success)
    {
        unsigned int start = header_size;
        const unsigned int end = (unsigned int)spv.size();
        while (start < end)
        {
            unsigned int wordCount = asWordCount(start);
            spv::Op opCode = asOpCode(start);

            switch (opCode)
            {
                case spv::OpName:
                {
                    const spv::Id id = asId(start + 1);
                    if (vectorCBuffersIds[id] != nullptr)
                    {
                        CBufferTypeData* cbufferData = vectorCBuffersIds[id];
                        const string name = literalString(start + 2);
                        cbufferData->cbufferName = name;
                    }
                    else if (vectorResourceVariablesId[id] != nullptr)
                    {
                        VariableData* variableData = vectorResourceVariablesId[id]->variableData;
                        const string name = literalString(start + 2);

                        //By default: the name is the keyName, and we deduct the rawName from it, unless this keyName has been specified by an attribute
                        if (!variableData->hasKeyName()) variableData->SetVariableKeyName(name);
                        variableData->SetVariableRawName( getRawNameFromKeyName(name) );
                    }
                    break;
                }

                case spv::OpLinkName:
                {
                    //apply to a resource variable
                    const spv::Id id = asId(start + 1);

                    if (vectorCBuffersIds[id] != nullptr)
                    {
                        error("LinkName attribute cannot apply to a cbuffer");
                        break;
                    }
                    else if (vectorResourceVariablesId[id] != nullptr)
                    {
                        VariableData* variableData = vectorResourceVariablesId[id]->variableData;
                        const string linkName = literalString(start + 2);
                        variableData->SetVariableKeyName(linkName);
                    }
                    break;
                }

                case spv::OpMemberName:
                {
                    spv::Id id = asId(start + 1);
                    if (vectorCBuffersIds[id] != nullptr)
                    {
                        CBufferTypeData* cbufferData = vectorCBuffersIds[id];

                        unsigned int index = asLiteralValue(start + 2);
                        string memberName = literalString(start + 3);
#ifdef XKSLANG_DEBUG_MODE
                        if (index >= cbufferData->cbufferMembersData->countMembers()) { error("Invalid member index"); break; }
#endif
                        cbufferData->cbufferMembersData->members[index].declarationName = memberName;
                    }
                    break;
                }

                case spv::OpMemberLinkName:
                {
                    //apply to a cbuffer member
                    spv::Id id = asId(start + 1);
                    if (vectorCBuffersIds[id] != nullptr)
                    {
                        CBufferTypeData* cbufferData = vectorCBuffersIds[id];

                        unsigned int index = asLiteralValue(start + 2);
                        string linkName = literalString(start + 3);
#ifdef XKSLANG_DEBUG_MODE
                        if (index >= cbufferData->cbufferMembersData->countMembers()) { error("Invalid member index"); break; }
#endif
                        cbufferData->cbufferMembersData->members[index].linkName = linkName;
                    }
                    break;
                }

                case spv::OpMemberAttribute:
                {
                    const spv::Id id = asId(start + 1);
                    if (vectorCBuffersIds[id] != nullptr)
                    {
                        CBufferTypeData* cbufferData = vectorCBuffersIds[id];

                        unsigned int index = asLiteralValue(start + 2);
                        string attribute = literalString(start + 3);
#ifdef XKSLANG_DEBUG_MODE
                        if (index >= cbufferData->cbufferMembersData->countMembers()) { error("Invalid member index"); break; }
#endif
                        cbufferData->cbufferMembersData->members[index].attribute = attribute;
                    }
                    break;
                }

                //we're not getting the size / alignment from this instruction anymore
                /*case spv::OpCBufferProperties:
                {
                    const spv::Id typeId = asId(start + 1);

                    if (vectorCBuffersIds[typeId] != nullptr)
                    {
                        CBufferTypeData* cbufferData = vectorCBuffersIds[typeId];

                        spv::XkslPropertyEnum cbufferType = (spv::XkslPropertyEnum)asLiteralValue(start + 2);
                        spv::XkslPropertyEnum cbufferStage = (spv::XkslPropertyEnum)asLiteralValue(start + 3);
                        unsigned int countMembers = asLiteralValue(start + 4);
                        unsigned int remainingBytes = wordCount - 5;

#ifdef XKSLANG_DEBUG_MODE
                        if (cbufferType != cbufferData->cbufferType) { error("Invalid cbuffer type property"); break; }
                        if (countMembers != cbufferData->cbufferCountMembers) { error("Invalid cbuffer count members property"); break; }
#endif

                        if (countMembers != cbufferData->cbufferMembersData->countMembers()) { error("Inconsistent number of members"); break; }
                        if (remainingBytes != countMembers * 2) { error("OpCBufferProperties instruction has an invalid number of bytes"); break; }

                        for (unsigned int m = 0; m < countMembers; m++)
                        {
                            unsigned int k = start + 5 + (m * 2);
                            unsigned int memberSize = asLiteralValue(k);
                            unsigned int memberAlignment = asLiteralValue(k + 1);

                            cbufferData->cbufferMembersData->members[m].memberSize = memberSize;
                            cbufferData->cbufferMembersData->members[m].memberAlignment = memberAlignment;
                        }
                    }
                    break;
                }*/

                case spv::OpMemberDecorate:
                {
                    spv::Id id = asId(start + 1);
                    if (vectorCBuffersIds[id] != nullptr)
                    {
                        CBufferTypeData* cbufferData = vectorCBuffersIds[id];

                        const unsigned int index = asLiteralValue(start + 2);
                        const spv::Decoration dec = (spv::Decoration)asLiteralValue(start + 3);
#ifdef XKSLANG_DEBUG_MODE
                        if (index >= cbufferData->cbufferMembersData->countMembers()) { error("Invalid member index"); break; }
#endif

                        switch (dec)
                        {
                            case spv::DecorationOffset:
                            {
#ifdef XKSLANG_DEBUG_MODE
                                if (wordCount <= 4) { error("Invalid wordCount"); break; }
#endif
                                const unsigned int offsetValue = asLiteralValue(start + 4);
                                cbufferData->cbufferMembersData->members[index].memberOffset = offsetValue;
                                break;
                            }
                            case spv::DecorationRowMajor:
                            {
                                cbufferData->cbufferMembersData->members[index].matrixLayoutDecoration = (int)(spv::DecorationRowMajor);
                                break;
                            }
                            case spv::DecorationColMajor:
                            {
                                cbufferData->cbufferMembersData->members[index].matrixLayoutDecoration = (int)(spv::DecorationColMajor);
                                break;
                            }
                        }
                    }
                    break;
                }

                case spv::OpFunction:
                case spv::OpTypeFunction:
                {
                    //all information retrieved at this point: can safely stop here
                    start = end;
                    break;
                }
            }
            start += wordCount;
        }

        if (errorMessages.size() > 0) success = false;
    }

    //Get the reflection data for all cbuffer members
    // We analyse the OpStruct instruction and get the reflection data on its members
    if (success)
    {
        vector<unsigned int> startPositionOfAllMemberDecorateInstructions;
        if (!GetStartPositionOfAllMemberDecorateInstructions(startPositionOfAllMemberDecorateInstructions))
        {
            error("failed to call GetStartPositionOfAllMemberDecorateInstructions");
            success = false;
        }

        if (success)
        {
            const unsigned int countCbuffers = (unsigned int)listAllCBuffers.size();
			effectReflection.CountConstantBuffers = countCbuffers;
			if (countCbuffers > 0) effectReflection.ConstantBuffers = new ConstantBufferReflectionDescription[countCbuffers];

            for (unsigned int icb = 0; icb < countCbuffers; icb++)
            {
                CBufferTypeData* cbufferData = listAllCBuffers[icb];

                TypeInstruction* cbufferType = cbufferData->cbufferTypeObject;
                spv::Op opCode = asOpCode(cbufferType->GetBytecodeStartPosition());
                int wordCount = asWordCount(cbufferType->GetBytecodeStartPosition());
                int countMembers = wordCount - 2;

#ifdef XKSLANG_DEBUG_MODE
                if (opCode != spv::OpTypeStruct) { error(string("Invalid OpCode type for the cbuffer instruction (expected OpTypeStruct): ") + OpcodeString(opCode)); break; }
                if (countMembers != cbufferData->cbufferCountMembers) { error("Invalid cbuffer count members property"); break; }
#endif
                if (countMembers != cbufferData->cbufferMembersData->countMembers()) { error("Inconsistent number of members"); break; }

                //Set the cbuffer name
                ConstantBufferReflectionDescription& cbufferReflection = effectReflection.ConstantBuffers[icb];
                cbufferReflection.CbufferName = cbufferData->cbufferName;
                cbufferReflection.Members.clear();
                cbufferReflection.Members.resize(countMembers);

                //Get the reflection data for all cbuffer members
                unsigned int posElemStart = cbufferType->GetBytecodeStartPosition() + 2;
                for (int mIndex = 0; mIndex < countMembers; ++mIndex)
                {
                    TypeStructMember& member = cbufferData->cbufferMembersData->members[mIndex];
                    ConstantBufferMemberReflectionDescription& memberReflection = cbufferReflection.Members[mIndex];
                    
                    string memberDeclarationName = member.GetDeclarationNameOrSemantic();
                    //by default: the member keyName is its declaration name, unless we defined a linkName for this member
                    if (member.HasLinkName()) memberReflection.KeyName = member.linkName;
                    else memberReflection.KeyName = memberDeclarationName;
                    memberReflection.RawName = getRawNameFromKeyName(memberDeclarationName);

                    //get the member type object
                    spv::Id cbufferMemberTypeId = asId(posElemStart + mIndex);
                    TypeInstruction* cbufferMemberType = GetTypeById(cbufferMemberTypeId);
                    if (cbufferMemberType == nullptr) return error("failed to find the cbuffer element type for id: " + member.GetDeclarationNameOrSemantic());

                    member.memberTypeId = cbufferMemberTypeId;
                    member.memberType = cbufferMemberType;

                    //check if the member type is a resource
                    member.isResourceType = IsResourceType(cbufferMemberType->opCode);

                    bool isMatrixRowMajor = true;
                    if (IsMatrixType(cbufferMemberType) || IsMatrixArrayType(cbufferMemberType))
                    {
                        if (member.matrixLayoutDecoration == (int)(spv::DecorationRowMajor)) isMatrixRowMajor = true;
                        else if (member.matrixLayoutDecoration == (int)(spv::DecorationColMajor)) isMatrixRowMajor = false;
                        else { error("undefined matrix member layout"); break; }
                    }

                    if (!GetTypeReflectionDescription(cbufferMemberType, isMatrixRowMajor, &(member.attribute), memberReflection.ReflectionType, &startPositionOfAllMemberDecorateInstructions))
                    {
                        error("Failed to get the reflexon data for the cbuffer member: " + member.GetDeclarationNameOrSemantic());
                        break;
                    }

                    //we can retrieve the offset from the bytecode, no need to recompute it
                    if (member.memberOffset == -1) {
                        error("a cbuffer member is missing its offset decoration" + member.GetDeclarationNameOrSemantic());
                        break;
                    }

                    int memberOffset = 0;
                    if (mIndex > 0)
                    {
                        ConstantBufferMemberReflectionDescription& previousMemberReflection = cbufferReflection.Members[mIndex - 1];
                        int previousMemberOffset = previousMemberReflection.Offset;
                        int previousMemberSize = previousMemberReflection.ReflectionType.Size;

                        memberOffset = previousMemberOffset + previousMemberSize;
                        int memberAlignment = memberReflection.ReflectionType.Alignment;
                        //round to pow2
                        memberOffset = (memberOffset + memberAlignment - 1) & (~(memberAlignment - 1));
                    }
                    cbufferReflection.Size = memberOffset + memberReflection.ReflectionType.Size;

#ifdef XKSLANG_DEBUG_MODE
                    //double check: we can also compute the member offset (depending on the previous member's offset, its size, plus the new member's alignment)
                    if (memberOffset != member.memberOffset) {
                        error("Offsets between reflection data and bytecode are not similar for the member:" + member.GetDeclarationNameOrSemantic());
                        break;
                    }
#endif
                    memberReflection.Offset = member.memberOffset;
                
                }
            }
        }

        if (errorMessages.size() > 0) success = false;
    }

    //for all the struct type: find their member names in the bytecode
    // (those names were never taken into consideration when parsing the bytecode (building all maps): we never needed them before)
    if (success)
    {
        //get the list of all struct members (or sub-members)
        vector<TypeReflectionDescription*> listStructMembers;
		unsigned int countCBuffers = (unsigned int)effectReflection.CountConstantBuffers;
        for (unsigned int cb = 0; cb < countCBuffers; cb++)
        {
            ConstantBufferReflectionDescription& cbuffer = effectReflection.ConstantBuffers[cb];
            unsigned int countMembers = (unsigned int)cbuffer.Members.size();
            for (unsigned int m = 0; m < countMembers; m++)
            {
                cbuffer.Members[m].ReflectionType.AddAllMemberAndSubMembersOfTheGivenClass(EffectParameterReflectionClass::Struct, listStructMembers);
            }
        }

        if (listStructMembers.size() > 0)
        {
            //flag all id we for which we need to recover the member names
            vector<TypeReflectionDescription*> listStructMembersByResultId;
            listStructMembersByResultId.resize(bound(), nullptr);
            for (auto itm = listStructMembers.begin(); itm != listStructMembers.end(); itm++)
            {
                TypeReflectionDescription* structMember = *itm;

                spv::Id spvStructTypeId = structMember->SpvTypeId;

#ifdef XKSLANG_DEBUG_MODE
                if (spvStructTypeId >= (unsigned int)listStructMembersByResultId.size()) { error("Id out of bound: " + to_string(spvStructTypeId)); break; }
#endif
                
                //special case if the member is an array of structs: the structTypeId that we want to match is the id of the array type
                if (structMember->ArrayElements > 0)
                {
                    TypeInstruction* cbufferMemberType = GetTypeById(spvStructTypeId);
                    if (cbufferMemberType == nullptr) { error("failed to find the type object for the cbuffer struct array id: " + to_string(spvStructTypeId)); break; }
                    if (cbufferMemberType->opCode != spv::OpTypeArray) { error("Invalid type for the cbuffer struct array id: " + to_string(spvStructTypeId)); break; }
                    spvStructTypeId = asId(cbufferMemberType->GetBytecodeStartPosition() + 2);
#ifdef XKSLANG_DEBUG_MODE
                    if (spvStructTypeId >= (unsigned int)listStructMembersByResultId.size()) { error("Id out of bound: " + to_string(spvStructTypeId)); break; }
#endif
                }

                //link the struct members having the same typeId (different members can use the same typeId (and will have the same name))
                structMember->nextTypeInList = listStructMembersByResultId[spvStructTypeId];
                listStructMembersByResultId[spvStructTypeId] = structMember;
            }

            //Fill the struct members name
            {
                unsigned int start = header_size;
                const unsigned int end = (unsigned int)spv.size();
                while (start < end)
                {
                    unsigned int wordCount = asWordCount(start);
                    spv::Op opCode = asOpCode(start);

                    switch (opCode)
                    {
                        case spv::OpMemberName:
                        {
                            spv::Id id = asId(start + 1);
                            int memberIndex = asLiteralValue(start + 2);
#ifdef XKSLANG_DEBUG_MODE
                            if (id >= (unsigned int)listStructMembersByResultId.size()) { error("Id out of bound: " + to_string(id)); break; }
                            if (memberIndex < 0) { error("invalid memberIndex: " + to_string(memberIndex)); break; }
#endif
                            if (listStructMembersByResultId[id] != nullptr)
                            {
                                string memberName = literalString(start + 3);
                                TypeReflectionDescription* aType = listStructMembersByResultId[id];
                                while (aType != nullptr)
                                {
#ifdef XKSLANG_DEBUG_MODE
                                    if (memberIndex >= aType->CountMembers) { error("memberIndex out of bound: " + to_string(memberIndex)); break; }
#endif
                                    aType->Members[memberIndex].Name = memberName;
                                    aType = aType->nextTypeInList;
                                }
                            }
                            break;
                        }

                        case spv::OpFunction:
                        case spv::OpTypeFunction:
                        {
                            start = end;
                            break;
                        }
                    }
                    start += wordCount;
                }
            }

        }

        if (errorMessages.size() > 0) success = false;
    }

    //=========================================================================================================================
    //=========================================================================================================================
    //Add ResourceBindings info in EffectReflection
    // Find which cbuffer is used by which output stage
    // Find which variable resource is used by which output stage
    if (success)
    {
		vector<EffectResourceBindingDescription> vecAllResourceBindings;

        //we access the cbuffer through their variable (not their type): update the targeted IDs
        for (auto itcb = listAllCBuffers.begin(); itcb != listAllCBuffers.end(); itcb++)
        {
            CBufferTypeData* cbufferData = *itcb;
            vectorCBuffersIds[cbufferData->cbufferTypeObject->GetId()] = nullptr;
            vectorCBuffersIds[cbufferData->cbufferVariableTypeObject->GetId()] = cbufferData;
        }

        //=========================================================================================================================
        // loop over all stages
        for (unsigned int iStage = 0; iStage < listEntryPoints.size(); ++iStage)
        {
            FunctionInstruction* stageEntryFunction = listEntryPoints[iStage].entryFunction;
            ShadingStageEnum stage = listEntryPoints[iStage].stage;

            //reset the cbuffer and resource variable flag (we insert a cbuffer once per stage)
            for (auto itcb = listAllCBuffers.begin(); itcb != listAllCBuffers.end(); itcb++) {
                CBufferTypeData* cbufferData = *itcb;
                cbufferData->tmpFlag = 0;
            }
            for (auto itv = listAllResourceVariables.begin(); itv != listAllResourceVariables.end(); itv++) {
                VariableInstruction* variable = *itv;
                variable->tmpFlag = 0;
            }

            //Set all functions flag to 0 (to check a function only once)
            for (auto itsf = vecAllFunctions.begin(); itsf != vecAllFunctions.end(); itsf++) {
                FunctionInstruction* aFunction = *itsf;
                aFunction->flag1 = 0;
            }

            //=========================================================================================================================
            //Find all functions being called by the stage
            // mark the used cbuffers and the used resources
            vector<FunctionInstruction*> listAllFunctionsCalledByTheStage;

            vector<FunctionInstruction*> vectorFunctionsToCheck;
            vectorFunctionsToCheck.push_back(stageEntryFunction);
            stageEntryFunction->flag1 = 1;
            int countBindingsToAdd = 0;
            while (vectorFunctionsToCheck.size() > 0)
            {
                FunctionInstruction* aFunctionCalled = vectorFunctionsToCheck.back();
                vectorFunctionsToCheck.pop_back();
                listAllFunctionsCalledByTheStage.push_back(aFunctionCalled);

                //check if the function is calling any other function
                unsigned int start = aFunctionCalled->bytecodeStartPosition;
                const unsigned int end = aFunctionCalled->bytecodeEndPosition;
                while (start < end)
                {
                    unsigned int wordCount = asWordCount(start);
                    spv::Op opCode = asOpCode(start);
                    switch (opCode)
                    {
                        case spv::OpLoad:
                        {
                            spv::Id idLoaded = asId(start + 3);

                            //are we accessing a resource variable
                            if (vectorResourceVariablesId[idLoaded] != nullptr)
                            {
                                VariableInstruction* variable = vectorResourceVariablesId[idLoaded];
                                if (variable->tmpFlag == 0)
                                {
                                    variable->tmpFlag = 1;
                                    countBindingsToAdd++;
                                }
                            }
                            break;
                        }

                        case spv::OpAccessChain:
                        {
                            spv::Id structIdAccessed = asId(start + 3);

                            //are we accessing a cbuffer
                            if (vectorCBuffersIds[structIdAccessed] != nullptr)
                            {
                                CBufferTypeData* cbufferData = vectorCBuffersIds[structIdAccessed];
                                if (cbufferData->tmpFlag == 0)
                                {
                                    cbufferData->tmpFlag = 1;
                                    countBindingsToAdd++;
                                }
                            }
                            break;
                        }

                        case spv::OpFunctionCall:
                        {
                            //pile the function to go check it later
                            spv::Id functionCalledId = asId(start + 3);
                            FunctionInstruction* anotherFunctionCalled = GetFunctionById(functionCalledId);
                            if (anotherFunctionCalled == nullptr) {
                                error("Failed to find the function for id: " + to_string(functionCalledId));
                            }
                            else
                            {
                                if (anotherFunctionCalled->flag1 == 0) {
                                    anotherFunctionCalled->flag1 = 1;
                                    vectorFunctionsToCheck.push_back(anotherFunctionCalled); //we'll analyse the function later
                                }
                            }
                            break;
                        }
                    }

                    start += wordCount;
                }
            }
            if (errorMessages.size() > 0) { success = false; break; }

            //=========================================================================================================================
            //Add the bindings found into ResourceBindings list
            {
                if (countBindingsToAdd > 0)
                {
                    for (auto itcb = listAllCBuffers.begin(); itcb != listAllCBuffers.end(); itcb++)
                    {
                        CBufferTypeData* cbufferData = *itcb;
                        if (cbufferData->tmpFlag == 1)
                        {
							vecAllResourceBindings.push_back(
                                EffectResourceBindingDescription(
                                    stage,
                                    cbufferData->cbufferName,
                                    getRawNameFromKeyName(cbufferData->cbufferName),
                                    EffectParameterReflectionClass::ConstantBuffer,
                                    EffectParameterReflectionType::ConstantBuffer
                                ));
                        }
                    }
                    for (auto itv = listAllResourceVariables.begin(); itv != listAllResourceVariables.end(); itv++)
                    {
                        VariableInstruction* variable = *itv;
                        VariableData* variableData = variable->variableData;

#ifdef XKSLANG_DEBUG_MODE
                        if (!variableData->hasKeyName() || !variableData->hasRawName()) error("A variable is missing name information");
#endif

                        if (variable->tmpFlag == 1)
                        {
							vecAllResourceBindings.push_back(
                                EffectResourceBindingDescription(
                                    stage,
                                    variableData->variableKeyName,
                                    variableData->variableRawName,
                                    variableData->variableTypeReflection.Class,
                                    variableData->variableTypeReflection.Type
                                ));
                        }
                    }
                }
            }

            if (errorMessages.size() > 0) { success = false; break; }
        } //end loop over all stages

		//copy the ResourceBindings vector into the EffectReflection
		if (success)
		{
            effectReflection.SetResourcesBindings(vecAllResourceBindings);
		}
    }

	//=========================================================================================================================
	//=========================================================================================================================
	//delete allocated data
	for (auto itcb = listAllCBuffers.begin(); itcb != listAllCBuffers.end(); itcb++)
	{
		CBufferTypeData* cbufferData = *itcb;
		if (cbufferData->cbufferMembersData != nullptr)
		{
			delete cbufferData->cbufferMembersData;
			cbufferData->cbufferMembersData = nullptr;
		}
	}
	for (auto itrv = listAllResourceVariables.begin(); itrv != listAllResourceVariables.end(); itrv++)
	{
		VariableInstruction* variable = *itrv;
		if (variable->variableData != nullptr)
		{
			delete variable->variableData;
			variable->variableData = nullptr;
		}
	}

    return success;
}

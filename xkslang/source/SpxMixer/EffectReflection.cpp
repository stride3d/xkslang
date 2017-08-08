//
// Copyright (C)

#include <sstream>

#include "EffectReflection.h"

using namespace std;
using namespace xkslang;

//=====================================================================================================================
//=====================================================================================================================
// EffectReflection
EffectReflection::~EffectReflection()
{
	Clear();
}

void EffectReflection::Clear()
{
	if (ConstantBuffers != nullptr) {
		delete[] ConstantBuffers;
		ConstantBuffers = nullptr;
	}
	CountConstantBuffers = 0;

	if (ResourceBindings != nullptr) {
		delete[] ResourceBindings;
		ResourceBindings = nullptr;
	}
	CountResourceBindings = 0;

	if (InputAttributes != nullptr) {
		delete[] InputAttributes;
		InputAttributes = nullptr;
	}
	CountInputAttributes = 0;
}

void EffectReflection::SetResourcesBindings(const vector<EffectResourceBindingDescription>& bindings)
{
    if (ResourceBindings != nullptr) {
        delete[] ResourceBindings;
        ResourceBindings = nullptr;
    }

    CountResourceBindings = (int)bindings.size();
    if (CountResourceBindings > 0)
    {
        ResourceBindings = new EffectResourceBindingDescription[CountResourceBindings];
        for (int k = 0; k < CountResourceBindings; k++)
        {
            ResourceBindings[k] = bindings[k];
        }
    }
}

void EffectReflection::SetInputAttributes(const vector<ShaderInputAttributeDescription>& inputAttributes)
{
    if (InputAttributes != nullptr) {
        delete[] InputAttributes;
        InputAttributes = nullptr;
    }

    CountInputAttributes = (int)inputAttributes.size();
    if (CountInputAttributes > 0)
    {
        InputAttributes = new ShaderInputAttributeDescription[CountInputAttributes];
        for (int k = 0; k < CountInputAttributes; k++)
        {
            InputAttributes[k] = inputAttributes[k];
        }
    }
}

//=====================================================================================================================
//=====================================================================================================================
// ConstantBuffer

//=====================================================================================================================
//=====================================================================================================================
// EffectResourceBindingDescription

/*
EffectResourceBindingDescription::EffectResourceBindingDescription(ShadingStageEnum stage, std::string keyName, EffectParameterReflectionClass c, EffectParameterReflectionType t)
	: Stage(stage), Class(c), Type(t)
{
	const int nameLen = keyName.size();
	if (nameLen == 0) KeyName = nullptr;
	else
	{
		KeyName = new char[nameLen + 1];
		for (int k = 0; k < nameLen; k++) KeyName[k] = keyName[k];
		KeyName[nameLen] = '\0';
	}
}

EffectResourceBindingDescription::EffectResourceBindingDescription(const EffectResourceBindingDescription& e)
	: Stage(e.Stage), Class(e.Class), Type(e.Type)
{
	if (e.KeyName == nullptr) KeyName = nullptr;
	else
	{
		const int nameLen = strlen(e.KeyName);
		KeyName = new char[nameLen + 1];
		strcpy(KeyName, e.KeyName);
	}
}

EffectResourceBindingDescription::~EffectResourceBindingDescription()
{
	if (KeyName != nullptr) delete[] KeyName;
	KeyName = nullptr;
}

EffectResourceBindingDescription& EffectResourceBindingDescription::operator=(const EffectResourceBindingDescription& e)
{
	Stage = e.Stage;
	Class = e.Class;
	Type = e.Type;

	if (KeyName != nullptr) delete[] KeyName;
	if (e.KeyName == nullptr) KeyName = nullptr;
	else
	{
		const int nameLen = strlen(e.KeyName);
		KeyName = new char[nameLen + 1];
		strcpy(KeyName, e.KeyName);
	}

	return *this;
}
*/

//=====================================================================================================================
//=====================================================================================================================
TypeReflectionDescription::~TypeReflectionDescription()
{
    if (Members != nullptr)
    {
        delete[] Members;
        Members = nullptr;
    }
}

void TypeReflectionDescription::SetStructMembers(TypeMemberReflectionDescription* members, int countMembers)
{
    if (Members != nullptr) {
        delete[] Members;
        Members = nullptr;
    }

    this->Members = members;
    this->CountMembers = countMembers;
}

void TypeReflectionDescription::AddAllMemberAndSubMembersOfTheGivenClass(EffectParameterReflectionClass memberClass, vector<TypeReflectionDescription*>& listMembers)
{
    if (Class == memberClass) listMembers.push_back(this);

    if (CountMembers > 0)
    {
        for (int m = 0; m < CountMembers; m++)
        {
            Members[m].Type.AddAllMemberAndSubMembersOfTheGivenClass(memberClass, listMembers);
        }
    }
}

string TypeReflectionDescription::Print(int padding)
{
    string paddingStr = "";
    for (int i = 0; i < padding; i++) paddingStr += ' ';

    std::ostringstream stream;
    stream << paddingStr << "Class=" << EffectReflection::GetEffectParameterReflectionClassLabel(Class) << " Type=" << EffectReflection::GetEffectParameterReflectionTypeLabel(Type)
        << " Size=" << Size << " Alignment=" << Alignment << " ArrayStride=" << ArrayStride << " MatrixStride=" << MatrixStride
        << " ArrayElements=" << ArrayElements << " Rows=" << RowCount << " Columns=" << ColumnCount << endl;
    if (CountMembers > 0)
    {
        paddingStr += ' ';
        stream << paddingStr << "CountMembers=" << CountMembers << endl;
        if (Members == nullptr)
        {
            stream << paddingStr << "Members undefined" << endl;
        }
        else
        {
            for (int m = 0; m < CountMembers; m++) stream << Members[m].Print(padding + 1);
        }
    }
    return stream.str();
}

string TypeMemberReflectionDescription::Print(int padding)
{
    string paddingStr = "";
    for (int i = 0; i < padding; i++) paddingStr += ' ';

    std::ostringstream stream;
    stream << paddingStr << "Name=\"" << Name << "\" Offset=" << Offset << endl;
    stream << Type.Print(padding);
    return stream.str();
}

string ConstantBufferMemberReflectionDescription::Print(int padding)
{
    string paddingStr = "";
    for (int i = 0; i < padding; i++) paddingStr += ' ';

    std::ostringstream stream;
    stream << paddingStr << "KeyName=\"" << KeyName << "\" RawName=\"" << RawName << "\" LogicalGroup=\"" << LogicalGroup << "\" Offset=" << Offset << endl;
    stream << ReflectionType.Print(padding) << endl;
    return stream.str();
}

string EffectReflection::Print()
{
    std::ostringstream stream;

    //print rawname => keyname information (for better visibility)
    stream << "***************************" << endl;
    stream << "****  ConstantBuffers  ****" << endl;
    stream << "***************************" << endl;
    for (int cb = 0; cb < CountConstantBuffers; ++cb)
    {
        ConstantBufferReflectionDescription& cbuffer = ConstantBuffers[cb];
        stream << " cbuffer " << cbuffer.CbufferName << " [size: " << cbuffer.Size << "]" << endl;
        for (unsigned int im = 0; im < cbuffer.Members.size(); im++)
        {
            ConstantBufferMemberReflectionDescription& member = cbuffer.Members[im];
            stream << "@C    " << member.RawName << " => " << member.KeyName << endl;
        }
    }
    stream << "***********************" << endl;
    stream << "*****  Resources  *****" << endl;
    stream << "***********************" << endl;
    for (int rb = 0; rb < CountResourceBindings; ++rb)
    {
        EffectResourceBindingDescription& resource = ResourceBindings[rb];
        stream << "@C    " << resource.RawName << " => " << resource.KeyName << " [Stage: " << GetShadingStageLabel(resource.Stage) << "]" << endl;
    }

    stream << endl;
    stream << "ConstantBuffers. Count=" << CountConstantBuffers << endl;
    for (int cb = 0; cb < CountConstantBuffers; ++cb)
    {
        ConstantBufferReflectionDescription& cbuffer = ConstantBuffers[cb];
        stream << " cbuffer: Name=\"" << cbuffer.CbufferName << "\" Size=" << cbuffer.Size << " MembersCount=" << cbuffer.Members.size() << endl;

        for (unsigned int im = 0; im < cbuffer.Members.size(); im++)
        {
            ConstantBufferMemberReflectionDescription& member = cbuffer.Members[im];
            stream << member.Print(2);
        }
    }

    stream << "ResourceBindings. Count=" << CountResourceBindings << endl;
    for (int rb = 0; rb < CountResourceBindings; ++rb)
    {
        EffectResourceBindingDescription& bindings = ResourceBindings[rb];
        stream << " Stage=" << GetShadingStageLabel(bindings.Stage) << " KeyName=\"" << bindings.KeyName << "\"" << " RawName=\"" << bindings.RawName << "\""
            << " Class=" << EffectReflection::GetEffectParameterReflectionClassLabel(bindings.Class)
            << " Type=" << EffectReflection::GetEffectParameterReflectionTypeLabel(bindings.Type) << endl;
    }

	stream << endl;
	stream << "InputAttributes. Count=" << CountInputAttributes << endl;
	for (int i = 0; i < CountInputAttributes; ++i)
	{
		ShaderInputAttributeDescription& ia = InputAttributes[i];
		stream << " SemanticName=\"" << ia.SemanticName << "\" SemanticIndex=" << ia.SemanticIndex << endl;
	}

    return stream.str();
}

string EffectReflection::GetEffectParameterReflectionClassLabel(EffectParameterReflectionClass parameterClass)
{
    switch (parameterClass)
    {
        case xkslang::EffectParameterReflectionClass::Undefined:           return "Undefined";
        case xkslang::EffectParameterReflectionClass::Scalar:              return "Scalar";
        case xkslang::EffectParameterReflectionClass::Vector:              return "Vector";
        case xkslang::EffectParameterReflectionClass::MatrixRows:          return "MatrixRows";
        case xkslang::EffectParameterReflectionClass::MatrixColumns:       return "MatrixColumns";
        case xkslang::EffectParameterReflectionClass::Object:              return "Object";
        case xkslang::EffectParameterReflectionClass::Struct:              return "Struct";
        case xkslang::EffectParameterReflectionClass::InterfaceClass:      return "InterfaceClass";
        case xkslang::EffectParameterReflectionClass::InterfacePointer:    return "InterfacePointer";
        case xkslang::EffectParameterReflectionClass::Sampler:             return "Sampler";
        case xkslang::EffectParameterReflectionClass::ShaderResourceView:  return "ShaderResourceView";
        case xkslang::EffectParameterReflectionClass::ConstantBuffer:      return "ConstantBuffer";
        case xkslang::EffectParameterReflectionClass::TextureBuffer:       return "TextureBuffer";
        case xkslang::EffectParameterReflectionClass::UnorderedAccessView: return "UnorderedAccessView";
        case xkslang::EffectParameterReflectionClass::Color:               return "Color";
        default: return "Unknown";
    }
}

std::string EffectReflection::GetEffectParameterReflectionTypeLabel(EffectParameterReflectionType parameterClass)
{
    switch (parameterClass)
    {
        case xkslang::EffectParameterReflectionType::Undefined:                    return "Undefined";
        case xkslang::EffectParameterReflectionType::Void:                         return "Void";
        case xkslang::EffectParameterReflectionType::Bool:                         return "Bool";
        case xkslang::EffectParameterReflectionType::Int:                          return "Int";
        case xkslang::EffectParameterReflectionType::Float:                        return "Float";
        case xkslang::EffectParameterReflectionType::String:                       return "String";
        case xkslang::EffectParameterReflectionType::Texture:                      return "Texture";
        case xkslang::EffectParameterReflectionType::Texture1D:                    return "Texture1D";
        case xkslang::EffectParameterReflectionType::Texture2D:                    return "Texture2D";
        case xkslang::EffectParameterReflectionType::Texture3D:                    return "Texture3D";
        case xkslang::EffectParameterReflectionType::TextureCube:                  return "TextureCube";
        case xkslang::EffectParameterReflectionType::Sampler:                      return "Sampler";
        case xkslang::EffectParameterReflectionType::Sampler1D:                    return "Sampler1D";
        case xkslang::EffectParameterReflectionType::Sampler2D:                    return "Sampler2D";
        case xkslang::EffectParameterReflectionType::Sampler3D:                    return "Sampler3D";
        case xkslang::EffectParameterReflectionType::SamplerCube:                  return "SamplerCube";
        case xkslang::EffectParameterReflectionType::UInt:                         return "UInt";
        case xkslang::EffectParameterReflectionType::UInt8:                        return "UInt8";
        case xkslang::EffectParameterReflectionType::Buffer:                       return "Buffer";
        case xkslang::EffectParameterReflectionType::ConstantBuffer:               return "ConstantBuffer";
        case xkslang::EffectParameterReflectionType::TextureBuffer:                return "TextureBuffer";
        case xkslang::EffectParameterReflectionType::Texture1DArray:               return "Texture1DArray";
        case xkslang::EffectParameterReflectionType::Texture2DArray:               return "Texture2DArray";
        case xkslang::EffectParameterReflectionType::Texture2DMultisampled:        return "Texture2DMultisampled";
        case xkslang::EffectParameterReflectionType::Texture2DMultisampledArray:   return "Texture2DMultisampledArray";
        case xkslang::EffectParameterReflectionType::TextureCubeArray:             return "TextureCubeArray";
        case xkslang::EffectParameterReflectionType::Double:                       return "Double";
        case xkslang::EffectParameterReflectionType::RWTexture1D:                  return "RWTexture1D";
        case xkslang::EffectParameterReflectionType::RWTexture1DArray:             return "RWTexture1DArray";
        case xkslang::EffectParameterReflectionType::RWTexture2D:                  return "RWTexture2D";
        case xkslang::EffectParameterReflectionType::RWTexture2DArray:             return "RWTexture2DArray";
        case xkslang::EffectParameterReflectionType::RWTexture3D:                  return "RWTexture3D";
        case xkslang::EffectParameterReflectionType::RWBuffer:                     return "RWBuffer";
        case xkslang::EffectParameterReflectionType::ByteAddressBuffer:            return "ByteAddressBuffer";
        case xkslang::EffectParameterReflectionType::RWByteAddressBuffer:          return "RWByteAddressBuffer";
        case xkslang::EffectParameterReflectionType::StructuredBuffer:             return "StructuredBuffer";
        case xkslang::EffectParameterReflectionType::RWStructuredBuffer:           return "RWStructuredBuffer";
        case xkslang::EffectParameterReflectionType::AppendStructuredBuffer:       return "AppendStructuredBuffer";
        case xkslang::EffectParameterReflectionType::ConsumeStructuredBuffer:      return "ConsumeStructuredBuffer";
        default: return "Unknown";
    }
}
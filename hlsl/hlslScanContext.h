//
// Copyright (C) 2016 Google, Inc.
//
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
//
//    Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
//
//    Redistributions in binary form must reproduce the above
//    copyright notice, this list of conditions and the following
//    disclaimer in the documentation and/or other materials provided
//    with the distribution.
//
//    Neither the name of Google, Inc., nor the names of its
//    contributors may be used to endorse or promote products derived
//    from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
// FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
// COPYRIGHT HOLDERS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
// INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
// BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
// LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
// ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//

//
// This holds context specific to the HLSL scanner, which
// sits between the preprocessor scanner and HLSL parser.
//

#ifndef HLSLSCANCONTEXT_H_
#define HLSLSCANCONTEXT_H_

#include "../glslang/MachineIndependent/ParseHelper.h"
#include "hlslTokens.h"

namespace glslang {

class TPpContext;
class TPpToken;


//
// Everything needed to fully describe a token.
//
struct HlslToken {
    HlslToken() : string(nullptr) { loc.init(); }
    TSourceLoc loc;                // location of token in the source
    EHlslTokenClass tokenClass;    // what kind of token it is
    union {                        // what data the token holds
        glslang::TString *string;  // for identifiers
        int i;                     // for literals
        unsigned int u;
        bool b;
        double d;
    };

    bool IsEqualsToToken(const HlslToken& tok)
    {
        return loc.isLocatedAt(tok.loc) && tokenClass == tok.tokenClass;
    }
};

class XkslShaderDefinition;
struct TShaderClassFunction
{
    XkslShaderDefinition* shader;  //shader owning the method
    TFunction* function;           //function prototype
    HlslToken token;               //token where the function is declared
    TIntermNode* bodyNode;         //resulting node of the function definition
};

class XkslShaderDefinition
{
public:
    enum class MemberLocationTypeEnum
    {
        CBuffer,
        StreamBuffer,
        Const,
        UnresolvedConst,
    };

    enum class ShaderIdentifierTypeEnum
    {
        Unknown,
        Member,
        Method
    };

    enum class ShaderParsingStatusEnum
    {
        Undefined = 0,
        HeaderDeclarationProcessed,
        GenericsResolved,
        ProcessedInheritance,
        CustomTypeDeclared,
        ConstMembersParsed,
        ConstsRegistered,
        UnresolvedConstsResolved,
        MembersAndMethodsDeclarationParsed,
        ProcessedCompositions,
        MembersAndMethodsDeclarationRegistered,
        MethodsDefinitionParsed,
        ParsingCompleted
    };

    //Define the location of an identidier (member or method)
    class ShaderIdentifierLocation
    {
    public:
        XkslShaderDefinition* shader;
        ShaderIdentifierTypeEnum identifierType;

        //for members
        int memberIndex;
        MemberLocationTypeEnum memberLocationType;
        TString* symbolName;
        TString* memberName;

        //for methods
        TFunction* method;

        ShaderIdentifierLocation(): shader(nullptr), identifierType(ShaderIdentifierTypeEnum::Unknown), memberIndex(-1), method(nullptr), memberName(nullptr){}

        bool isUnknown() { return identifierType == ShaderIdentifierTypeEnum::Unknown; }
        bool isMember() { return identifierType == ShaderIdentifierTypeEnum::Member; }
        bool isMethod() { return identifierType == ShaderIdentifierTypeEnum::Method; }

        void SetMemberLocation(XkslShaderDefinition* shader, TString* memberName, MemberLocationTypeEnum locationType, TString* symbolName, int index)
        {
            this->identifierType = ShaderIdentifierTypeEnum::Member;
            this->shader = shader;
            this->memberName = memberName;
            this->memberLocationType = locationType;
            this->symbolName = symbolName;
            this->memberIndex = index;
        }

        void SetMethodLocation(XkslShaderDefinition* shader, TFunction* method)
        {
            this->identifierType = ShaderIdentifierTypeEnum::Method;
            this->shader = shader;
            this->method = method;
        }
    };

    class ParentInformation
    {
    public:
        ShaderInheritedParentDefinition parentDefinition;
        XkslShaderDefinition* parentShader;

        ParentInformation() : parentShader(nullptr) {}
    };

    class XkslShaderMember
    {
    public:
        XkslShaderMember(): type(nullptr), shader(nullptr), resolvedDeclaredExpression(nullptr), expressionTokensList(nullptr){}

        TType* type;
        TSourceLoc loc;

        XkslShaderDefinition* shader;   //reference to the shader that declared the variable

        //For const variables
        TIntermTyped* resolvedDeclaredExpression;  //assignment expression, immediatly resolved when parsing the member declaration
        TVector<HlslToken>* expressionTokensList;  //assignment token list, unresolvable (due to unknown identifier) when parsing the member declaration

        ShaderIdentifierLocation memberLocation;   //How can we access to the member after we parsed its declaration
    };

    //Contain the details about the shader "Stream" and streams type.
    class StreamTypeInformation
    {
    public:
        TType* StreamStructureType;
        TString StreamStructDeclarationName;
        TString StreamStructAssignmentExpression;

        StreamTypeInformation(): StreamStructureType(nullptr) {}

        int GetCountStreamVariables()
        {
            if (StreamStructureType->getStruct() == nullptr) return -1;
            return (int)StreamStructureType->getStruct()->size() - 1; //-1 because of the unused member
        }

        const TString& GetStreamVariableName(int index)
        {
            return StreamStructureType->getStruct()->at(index).type->getFieldName();
        }
    };

public:
    XkslShaderDefinition() : parsingStatus(ShaderParsingStatusEnum::Undefined), tmpFlag(0), isValid(true), shaderUniqueId(-1) {}

    bool isValid;
    TSourceLoc location;  //location where the shader is declared in the file (for logs)

    int shaderUniqueId;
    TString shaderUniqueStringId; 

    TString shaderBaseName;
    //TString shaderNameSpace;
    TString shaderFullName;        //shader name plus generics value (ie ShaderMain<5>)
    TVector<ShaderGenericAttribute> listGenerics;
    TVector<ParentInformation> listParents;

    TVector<TShaderCompositionVariable> listCompositions;  //list of compositions declared in the shader

    StreamTypeInformation streamsTypeInfo;
    TVector<XkslShaderMember> listCustomTypes; //list of new types declared by the shader (such like struct definition)
    TVector<XkslShaderMember> listParsedMembers;  //list of members read by the parser
    TVector<XkslShaderMember> listAllDeclaredMembers;  //list of members after being declared
    TVector<TString*> listDeclaredBlockNames;  //list of block (cbuffer) names declared by the shader

    TVector<TShaderClassFunction> listMethods;

    TVector<HlslToken> listTokens;
    ShaderParsingStatusEnum parsingStatus;
    int tmpFlag;

    int GetCountGenerics() { return (int)(listGenerics.size()); }
    int GetShaderUniqueId() { return shaderUniqueId; }
    TString GetShaderUniqueStringId() { return shaderUniqueStringId; }
    void SetShaderUniqueId(int id) {
        shaderUniqueId = id;
        shaderUniqueStringId = TString("_id") + std::to_string(id).c_str();
    }

    TShaderCompositionVariable* GetCompositionVariableForId(int id){
        for (unsigned int i=0; i<listCompositions.size(); ++i) if (listCompositions[i].shaderCompositionId == id) return &listCompositions[i];
        return nullptr;
    }

    void AddParent(ShaderInheritedParentDefinition& parentDefinition) {
        listParents.push_back(ParentInformation());
        listParents.back().parentDefinition = parentDefinition;
    }

    bool HasForParent(XkslShaderDefinition* aShader) {
        for (unsigned int p = 0; p < listParents.size(); ++p)
            if (listParents[p].parentShader == aShader) return true;
        return false;
    }
};

class XkslShaderLibrary
{
public:
    TVector<XkslShaderDefinition*> listShaders;  //list of all shader parsed

    int GetCountShaderInLibrary() {return (int)(listShaders.size());}
    void AddNewShader(XkslShaderDefinition* shader){
        listShaders.push_back(shader);
        shader->SetShaderUniqueId((int)(listShaders.size()));
    }
};

//
// The state of scanning and translating raw tokens to slightly richer
// semantics, like knowing if an identifier is an existing symbol, or
// user-defined type.
//
class HlslScanContext {
public:
    HlslScanContext(TParseContextBase& parseContext, TPpContext& ppContext)
        : parseContext(parseContext), ppContext(ppContext) { }
    virtual ~HlslScanContext() { }

    static void fillInKeywordMap();
    static void deleteKeywordMap();

    void tokenize(HlslToken&);
    glslang::TBuiltInVariable mapSemantic(const char*);
    
    TString convertTokenToString(const HlslToken& token);
    void tokenizeExpression(const TString& expression, TVector<HlslToken>& listTokens);

protected:
    HlslScanContext(HlslScanContext&);
    HlslScanContext& operator=(HlslScanContext&);

    EHlslTokenClass tokenizeClass(HlslToken&);
    EHlslTokenClass tokenizeIdentifier();
    EHlslTokenClass identifierOrType();
    EHlslTokenClass reservedWord();
    EHlslTokenClass identifierOrReserved(bool reserved);
    EHlslTokenClass nonreservedKeyword(int version);

    TParseContextBase& parseContext;
    TPpContext& ppContext;
    TSourceLoc loc;
    TPpToken* ppToken;
    HlslToken* parserToken;

    const char* tokenText;
    EHlslTokenClass keyword;
};

} // end namespace glslang

#endif // HLSLSCANCONTEXT_H_

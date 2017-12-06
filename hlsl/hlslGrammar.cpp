//
// Copyright (C) 2016 Google, Inc.
// Copyright (C) 2016 LunarG, Inc.
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
// This is a set of mutually recursive methods implementing the HLSL grammar.
// Generally, each returns
//  - through an argument: a type specifically appropriate to which rule it
//    recognized
//  - through the return value: true/false to indicate whether or not it
//    recognized its rule
//
// As much as possible, only grammar recognition should happen in this file,
// with all other work being farmed out to hlslParseHelper.cpp, which in turn
// will build the AST.
//
// The next token, yet to be "accepted" is always sitting in 'token'.
// When a method says it accepts a rule, that means all tokens involved
// in the rule will have been consumed, and none left in 'token'.
//

#include "hlslTokens.h"
#include "hlslGrammar.h"
#include "hlslAttributes.h"

#include "../xkslang/source/Common/xkslangDefine.h"

#include "../SPIRV/spirv.hpp"

namespace glslang {

// Root entry point to this recursive decent parser.
// Return true if compilation unit was successfully accepted.
bool HlslGrammar::parse()
{
    advanceToken();
    return acceptCompilationUnit();
}

TIntermTyped* HlslGrammar::parseXkslShaderAssignmentExpression(XkslShaderLibrary* shaderLibrary, XkslShaderDefinition* currentShader,
    bool errorWhenParsingUnidentifiedSymbol, XkslShaderDefinition* shaderWhereSomeMembersCanBeFound)
{
    if (xkslShaderCurrentlyParsed != nullptr || xkslShaderLibrary != nullptr)
    {
        error("an xksl shader is or have already being parsed");
        return nullptr;
    }

    this->xkslShaderParsingOperation = XkslShaderParsingOperationEnum::ParseXkslShaderConstStatements;
    this->throwErrorWhenParsingUnidentifiedSymbol = errorWhenParsingUnidentifiedSymbol;
    this->shaderWhereMembersCanBeFound = shaderWhereSomeMembersCanBeFound;
    this->xkslShaderLibrary = shaderLibrary;
    ResetShaderLibraryFlag();
    this->xkslShaderCurrentlyParsed = currentShader;
    this->functionCurrentlyParsed = nullptr;

	advanceToken();
    while (!peekTokenClass(EHTokAssign))
    {
        if (peekTokenClass(EHTokNone))
        {
            error("failed to find the assignment token");
            return nullptr;
        }

        advanceToken();
    }

    TIntermTyped* expressionNode = nullptr;
    if (acceptTokenClass(EHTokAssign))
    {
        if (!acceptAssignmentExpression(expressionNode))
        {
            //not necessary an error, we just cannot resolved the assignment expression yet
            expressionNode = nullptr;

            if (!advanceUntilEndOfTokenList())
            {
                error("failed to advance until the end of the token list");
                return nullptr;
            }
        }
    }

    this->xkslShaderLibrary = nullptr;
    this->xkslShaderCurrentlyParsed = nullptr;

    return expressionNode;
}

bool HlslGrammar::parseXKslShaderDeclaration(XkslShaderLibrary* shaderLibrary)
{
    if (xkslShaderCurrentlyParsed != nullptr || xkslShaderLibrary != nullptr || this->xkslShaderParsingOperation != XkslShaderParsingOperationEnum::Undefined)
    {
        error("an xksl shader is or have already being parsed");
        return false;
    }

    this->xkslShaderParsingOperation = XkslShaderParsingOperationEnum::ParseXkslShaderDeclarations; //Tell the parser to only parse shader declaration
    this->throwErrorWhenParsingUnidentifiedSymbol = true;
    this->xkslShaderLibrary = shaderLibrary;
    ResetShaderLibraryFlag();

    TVector<EHlslTokenClass> listTokens;
    listTokens.push_back(EHTokShaderClass);
    listTokens.push_back(EHTokNamespace);
    advanceUntilFirstTokenFromList(listTokens, true);  //skip all previous declaration

    bool res = acceptCompilationUnit();
    if (!res) return false;

    return true;
}

bool HlslGrammar::parseXKslShaderNewTypesDefinition(XkslShaderLibrary* shaderLibrary, XkslShaderDefinition* shaderToParse)
{
    //root entry point for parsing xksl shader membes and methods declaration
    if (xkslShaderCurrentlyParsed != nullptr || xkslShaderLibrary != nullptr || this->xkslShaderParsingOperation != XkslShaderParsingOperationEnum::Undefined)
    {
        error("an xksl shader is or have already being parsed");
        return false;
    }

    this->xkslShaderParsingOperation = XkslShaderParsingOperationEnum::ParseXkslShaderNewTypesDefinition; //Tell the parser to only parse shader new types declaration
    this->throwErrorWhenParsingUnidentifiedSymbol = true;
    this->xkslShaderLibrary = shaderLibrary;
    this->xkslShaderToParse = shaderToParse;
    ResetShaderLibraryFlag();

    TVector<EHlslTokenClass> listTokens;
    listTokens.push_back(EHTokShaderClass);
    listTokens.push_back(EHTokNamespace);
    advanceUntilFirstTokenFromList(listTokens, true);  //skip all previous declaration

    bool res = acceptCompilationUnit();
    if (!res) return false;

    return true;
}

bool HlslGrammar::parseXKslShaderConstVariables(XkslShaderLibrary* shaderLibrary, XkslShaderDefinition* shaderToParse)
{
    //root entry point for parsing xksl shader membes and methods declaration
    if (xkslShaderCurrentlyParsed != nullptr || xkslShaderLibrary != nullptr || this->xkslShaderParsingOperation != XkslShaderParsingOperationEnum::Undefined)
    {
        error("an xksl shader is or have already being parsed");
        return false;
    }

    this->xkslShaderParsingOperation = XkslShaderParsingOperationEnum::ParseXkslShaderConstVariables; //Tell the parser to only parse const variables
    this->throwErrorWhenParsingUnidentifiedSymbol = true;
    this->xkslShaderLibrary = shaderLibrary;
    this->xkslShaderToParse = shaderToParse;
    ResetShaderLibraryFlag();

    TVector<EHlslTokenClass> listTokens;
    listTokens.push_back(EHTokShaderClass);
    listTokens.push_back(EHTokNamespace);
    advanceUntilFirstTokenFromList(listTokens, true);  //skip all previous declaration

    bool res = acceptCompilationUnit();
    if (!res) return false;

    return true;
}

bool HlslGrammar::parseXKslShaderMembersAndMethodsDeclaration(XkslShaderLibrary* shaderLibrary, XkslShaderDefinition* shaderToParse)
{
    //root entry point for parsing xksl shader membes and methods declaration
    if (xkslShaderCurrentlyParsed != nullptr || xkslShaderLibrary != nullptr || this->xkslShaderParsingOperation != XkslShaderParsingOperationEnum::Undefined)
    {
        error("an xksl shader is or have already being parsed");
        return false;
    }

    this->xkslShaderParsingOperation = XkslShaderParsingOperationEnum::ParseXkslShaderMembersAndMethodsDeclarations; //Tell the parser to only parse shader members and methods declaration
    this->throwErrorWhenParsingUnidentifiedSymbol = true;
    this->xkslShaderLibrary = shaderLibrary;
    this->xkslShaderToParse = shaderToParse;
    ResetShaderLibraryFlag();

    TVector<EHlslTokenClass> listTokens;
    listTokens.push_back(EHTokShaderClass);
    listTokens.push_back(EHTokNamespace);
    advanceUntilFirstTokenFromList(listTokens, true);  //skip all previous declaration

    bool res = acceptCompilationUnit();
    if (!res) return false;

    return true;
}

bool HlslGrammar::parseXKslShaderMethodsDefinition(XkslShaderLibrary* shaderLibrary, XkslShaderDefinition* shaderToParse)
{
    //root entry point for parsing xksl shader definition
    if (xkslShaderCurrentlyParsed != nullptr || xkslShaderLibrary != nullptr || this->xkslShaderParsingOperation != XkslShaderParsingOperationEnum::Undefined)
    {
        error("an xksl shader is or have already being parsed");
        return false;
    }

    this->xkslShaderParsingOperation = XkslShaderParsingOperationEnum::ParseXkslShaderMethodsDefinition;  //Tell the parser to parse shader method definition
    this->throwErrorWhenParsingUnidentifiedSymbol = false;
    this->xkslShaderLibrary = shaderLibrary;
    this->xkslShaderToParse = shaderToParse;
    ResetShaderLibraryFlag();

    TVector<EHlslTokenClass> listTokens;
    listTokens.push_back(EHTokShaderClass);
    listTokens.push_back(EHTokNamespace);
    advanceUntilFirstTokenFromList(listTokens, true);  //skip all previous declaration

    bool res = acceptCompilationUnit();
    return res;
}

void HlslGrammar::expected(const char* syntax)
{
    if (this->hasAnyErrorToBeProcessedAtTheTop()) return; //The error will be processed later.

    TString msgError(syntax);
    if (this->xkslShaderCurrentlyParsed != nullptr)
    {
        msgError = this->xkslShaderCurrentlyParsed->shaderFullName + ": " + msgError;
    }

    parseContext.error(token.loc, "Expected", msgError.c_str(), "");
}

void HlslGrammar::unimplemented(const char* error)
{
    if (this->hasAnyErrorToBeProcessedAtTheTop()) return; //The error will be processed later.

    parseContext.error(token.loc, "Unimplemented", error, "");
}

void HlslGrammar::error(const char* error)
{
    if (this->hasAnyErrorToBeProcessedAtTheTop()) return; //The error will be processed later.

    TString msgError(error);
    if (this->xkslShaderCurrentlyParsed != nullptr)
    {
        msgError = this->xkslShaderCurrentlyParsed->shaderFullName + ": " + msgError;
    }

    parseContext.error(token.loc, "Error", msgError.c_str(), "");
}

void HlslGrammar::error(const TString& str)
{
    error(str.c_str());
}

void HlslGrammar::warning(const char* warning)
{
    parseContext.warn(token.loc, "Warning", warning, "");
}

void HlslGrammar::warning(const TString& str)
{
    warning(str.c_str());
}

bool HlslGrammar::acceptShaderCustomType(const TString& shaderName, TType& type)
{
    switch (peek())
    {
        case EHTokIdentifier:
        {
            bool areTheNextTokensAKnownShaderName = false;
            TString* referredClassName = nullptr;

            //check if the next tokens defines a shader from our shader library (we can have shaders with some generics value)
            {
                int tokenCurrentIndex = getTokenCurrentIndex();
                TString name;
                if (!acceptIdentifierTokenClass(name)) {
                    error("failed to accept the identifier token");
                    recedeToTokenIndex(tokenCurrentIndex);
                    return false;
                }

                //is the name a base name of a known shader?
                if (isRecordedAsAShaderBaseName(name))
                {
                    //Any Generics?
                    TVector<TString*> listGenericValuesExpression;
                    if (peekTokenClass(EHTokLeftAngle))
                    {
                        if (!checkShaderGenericValuesExpression(listGenericValuesExpression)) {
                            error("failed to check for shader generic values");
                            recedeToTokenIndex(tokenCurrentIndex);
                            return false;
                        }
                    }

                    //concatenate the generic to the shader name
                    unsigned int countGenerics = (unsigned int)(listGenericValuesExpression.size());
                    if (countGenerics > 0)
                    {
                        TString nameExtension = "<";
                        for (unsigned int g = 0; g < countGenerics; g++)
                        {
                            nameExtension += *(listGenericValuesExpression[g]);
                            if (g == countGenerics - 1) nameExtension += ">";
                            else nameExtension += ",";
                        }
                        name += nameExtension;
                    }

                    if (isRecordedAsAShaderName(name))
                    {
                        //The token is a known shader class (ShaderA.XXX or ShaderA<1,2>.XXX)
                        referredClassName = NewPoolTString(name.c_str());
                        areTheNextTokensAKnownShaderName = true;
                    }
                }

                if (!areTheNextTokensAKnownShaderName) recedeToTokenIndex(tokenCurrentIndex);
            }

            if (areTheNextTokensAKnownShaderName)
            {
                //The token is a known shader class (ShaderA.XXX)
                if (!acceptTokenClass(EHTokDot))
                {
                    expected("dot");
                    return false;
                }
                return acceptShaderCustomType(*referredClassName, type);
            }
            else
            {
                TString* tokenName = token.string;
                if (tokenName == nullptr) { error("invalid token"); return false; }

                TType* typeDefinedByShader = getTypeDefinedByTheShaderOrItsParents(shaderName, *tokenName);
                if (typeDefinedByShader != nullptr)
                {
                    advanceToken();
                    type.shallowCopy(*typeDefinedByShader);
                    return true;
                }
                else return false;
            }
        }

        case EHTokThis:
        {
            TString* referredClassName = getCurrentShaderName();
            if (referredClassName == nullptr) { error("Failed to get the current shader name"); return false; }
            advanceToken();
            if (!acceptTokenClass(EHTokDot))
            {
                expected("dot");
                return false;
            }

            return acceptShaderCustomType(*referredClassName, type);
        }

        case EHTokBase:
        {
            //Base refers to the first parent
            int countParent = getCurrentShaderCountParents();
            if (countParent <= 0) { error("Invalid \"base\" accessor: the current shader has no inherited parents"); return false; }
            TString* referredClassName = getCurrentShaderParentName(0);
            if (referredClassName == nullptr) { error("Failed to get the current shader name"); return false; }

            advanceToken();
            if (!acceptTokenClass(EHTokDot))
            {
                expected("dot");
                return false;
            }

            return acceptShaderCustomType(*referredClassName, type);
        }
    }
    return false;
}

//Process class accessor: this, base, Knwon ClassName, composition variable name, ...
bool HlslGrammar::acceptClassReferenceAccessor(TString*& className, bool& isBase, bool& isACallThroughStaticShaderClassName,
    bool& isStream, TShaderCompositionVariable& compositionTargeted, bool& isStreamsUsedAsAType)
{
    if (getCurrentShaderName() == nullptr) return false;

    if (compositionTargeted.shaderCompositionId >= 0) return false; //we are already accessing a composition: don't accept any other accessor

    TShaderCompositionVariable aCompositionTargeted;
    switch (peek())
    {
        case EHTokThis:
        {
            className = getCurrentShaderName();
            advanceToken();
            break;
        }

        case EHTokBase:
        {
            //Base refers to the first parent
            int countParent = getCurrentShaderCountParents();
            if (countParent <= 0)
            {
                error("Invalid \"base\" accessor: the current shader has no inherited parents");
                return false;
            }
            isBase = true;
            className = getCurrentShaderName();
            advanceToken();
            break;
        }

        case EHTokStreams:
        {
            if (isStream)
            {
                error("We cannot use streams twice in the same expression");
                return false;
            }

            //2 cases: if we have streams.XXX: then we're referring to a stream variable
            //If we only have streams: we're declaring a Streams type
            advanceToken();
            if (peekTokenClass(EHTokDot))
            {
                isStream = true;
            }
            else
            {
                recedeToken();
                isStreamsUsedAsAType = true;
                return false;
            }

            break;
        }

        case EHTokIdentifier:
        {
            bool isExpressionAKnownShaderName = false;

            //check if the next tokens defines a shader from our shader library (we can have shaders with some generics value)
            {
                int tokenCurrentIndex = getTokenCurrentIndex();
                TString name;
                if (!acceptIdentifierTokenClass(name)) {
                    error("failed to accept the identifier token");
                    recedeToTokenIndex(tokenCurrentIndex);
                    return false;
                }

                if (isRecordedAsAShaderBaseName(name))
                {
                    //Any Generics?
                    TVector<TString*> listGenericValuesExpression;
                    if (peekTokenClass(EHTokLeftAngle))
                    {
                        if (!checkShaderGenericValuesExpression(listGenericValuesExpression)) {
                            error("failed to check for shader generic values");
                            recedeToTokenIndex(tokenCurrentIndex);
                            return false;
                        }
                    }

                    //concatenate the generic to the shader name
                    unsigned int countGenerics = (unsigned int)(listGenericValuesExpression.size());
                    if (countGenerics > 0)
                    {
                        TString nameExtension = "<";
                        for (unsigned int g = 0; g < countGenerics; g++)
                        {
                            nameExtension += *(listGenericValuesExpression[g]);
                            if (g == countGenerics - 1) nameExtension += ">";
                            else nameExtension += ",";
                        }
                        name += nameExtension;
                    }

                    if (isRecordedAsAShaderName(name))
                    {
                        //The token is a known shader class (ShaderA.XXX or ShaderA<1,2>.XXX)
                        className = NewPoolTString(name.c_str());
                        isACallThroughStaticShaderClassName = true;
                        isExpressionAKnownShaderName = true;
                    }
                }

                if (!isExpressionAKnownShaderName) recedeToTokenIndex(tokenCurrentIndex);
            }

            if (isExpressionAKnownShaderName)
            {
                break;
            }
            else if (isIdentifierRecordedAsACompositionVariableName(className, *token.string, false, aCompositionTargeted))
            {
                //The token is a composition that belongs to the shader class or its parents (Comp.XXX)
                //className = NewPoolTString(compositionTargeted.shaderOwnerName.c_str());
                compositionTargeted = aCompositionTargeted;
                advanceToken();
                break;
            }
            else return false;
        }

        default:
            return false;
    }

    if (!acceptTokenClass(EHTokDot))
    {
        expected("dot");
        return false;
    }

    return true;
}

// IDENTIFIER
// THIS
// type that can be used as IDENTIFIER
// Only process the next token if it is an identifier.
// Return true if it was an identifier.
bool HlslGrammar::acceptIdentifier(HlslToken& idToken)
{
    // IDENTIFIER
    if (peekTokenClass(EHTokIdentifier)) {
        idToken = token;
        advanceToken();
        return true;
    }

    // THIS
    // -> maps to the IDENTIFIER spelled with the internal special name for 'this'
    if (peekTokenClass(EHTokThis)) {
        idToken = token;
        advanceToken();
        idToken.tokenClass = EHTokIdentifier;
        idToken.string = NewPoolTString(intermediate.implicitThisName);
        return true;
    }

    // type that can be used as IDENTIFIER

    // Even though "sample", "bool", "float", etc keywords (for types, interpolation modifiers),
    // they ARE still accepted as identifiers.  This is not a dense space: e.g, "void" is not a
    // valid identifier, nor is "linear".  This code special cases the known instances of this, so
    // e.g, "int sample;" or "float float;" is accepted.  Other cases can be added here if needed.

    const char* idString = getTypeString(peek());
    if (idString == nullptr)
        return false;

    token.string     = NewPoolTString(idString);
    token.tokenClass = EHTokIdentifier;
    idToken = token;
    typeIdentifiers = true;

    advanceToken();

    return true;
}

// compilationUnit
//      : declaration_list EOF
//
bool HlslGrammar::acceptCompilationUnit()
{
    TIntermNode* unitNode = nullptr;

    if (! acceptDeclarationList(unitNode))
        return false;
    
    // set root of AST
    if (unitNode && !unitNode->getAsAggregate())
        unitNode = intermediate.growAggregate(nullptr, unitNode);
    intermediate.setTreeRoot(unitNode);

    return true;
}

// Recognize the following, but with the extra condition that it can be
// successfully terminated by EOF or '}'.
//
// declaration_list
//      : list of declaration_or_semicolon followed by EOF or RIGHT_BRACE
//
// declaration_or_semicolon
//      : declaration
//      : SEMICOLON
//
bool HlslGrammar::acceptDeclarationList(TIntermNode*& nodeList)
{
    do {
        // HLSL allows extra semicolons between global declarations
        do { } while (acceptTokenClass(EHTokSemicolon));

        // EOF or RIGHT_BRACE
        if (peekTokenClass(EHTokNone) || peekTokenClass(EHTokRightBrace))
            return true;

        // declaration
        if (! acceptDeclaration(nodeList))
            return false;
    } while (true);

    return true;
}

// sampler_state
//      : LEFT_BRACE [sampler_state_assignment ... ] RIGHT_BRACE
//
// sampler_state_assignment
//     : sampler_state_identifier EQUAL value SEMICOLON
//
// sampler_state_identifier
//     : ADDRESSU
//     | ADDRESSV
//     | ADDRESSW
//     | BORDERCOLOR
//     | FILTER
//     | MAXANISOTROPY
//     | MAXLOD
//     | MINLOD
//     | MIPLODBIAS
//
bool HlslGrammar::acceptSamplerState()
{
    // TODO: this should be genericized to accept a list of valid tokens and
    // return token/value pairs.  Presently it is specific to texture values.

    if (! acceptTokenClass(EHTokLeftBrace))
        return true;

    parseContext.warn(token.loc, "unimplemented", "immediate sampler state", "");

    do {
        // read state name
        HlslToken state;
        if (! acceptIdentifier(state))
            break;  // end of list

        // FXC accepts any case
        TString stateName = *state.string;
        std::transform(stateName.begin(), stateName.end(), stateName.begin(), ::tolower);

        if (! acceptTokenClass(EHTokAssign)) {
            expected("assign");
            return false;
        }

        if (stateName == "minlod" || stateName == "maxlod") {
            if (! peekTokenClass(EHTokIntConstant)) {
                expected("integer");
                return false;
            }

            TIntermTyped* lod = nullptr;
            if (! acceptLiteral(lod))  // should never fail, since we just looked for an integer
                return false;
        } else if (stateName == "maxanisotropy") {
            if (! peekTokenClass(EHTokIntConstant)) {
                expected("integer");
                return false;
            }

            TIntermTyped* maxAnisotropy = nullptr;
            if (! acceptLiteral(maxAnisotropy))  // should never fail, since we just looked for an integer
                return false;
        } else if (stateName == "filter") {
            HlslToken filterMode;
            if (! acceptIdentifier(filterMode)) {
                expected("filter mode");
                return false;
            }
        } else if (stateName == "addressu" || stateName == "addressv" || stateName == "addressw") {
            HlslToken addrMode;
            if (! acceptIdentifier(addrMode)) {
                expected("texture address mode");
                return false;
            }
        } else if (stateName == "miplodbias") {
            TIntermTyped* lodBias = nullptr;
            if (! acceptLiteral(lodBias)) {
                expected("lod bias");
                return false;
            }
        } else if (stateName == "bordercolor") {
            return false;
        } else if (stateName == "comparisonfunc") { //XKSL extension
            HlslToken compFunc;
            if (!acceptIdentifier(compFunc)) {
                expected("comparison function");
                return false;
            }
        } else {
            expected("texture state");
            return false;
        }

        // SEMICOLON
        if (! acceptTokenClass(EHTokSemicolon)) {
            expected("semicolon");
            return false;
        }
    } while (true);

    if (! acceptTokenClass(EHTokRightBrace))
        return false;

    return true;
}

bool HlslGrammar::acceptSamplerState(TSamplerStateDefinition*& samplerDef)
{
    if (!acceptTokenClass(EHTokLeftBrace))
        return true;

    //create and set samplerState default values
    samplerDef = new TSamplerStateDefinition;
    samplerDef->Filter = (int)spv::SamplerStateTextureFilterEnum::Linear;
    samplerDef->AddressU = (int)spv::SamplerStateTextureAddressMode::Clamp;
    samplerDef->AddressV = (int)spv::SamplerStateTextureAddressMode::Clamp;
    samplerDef->AddressW = (int)spv::SamplerStateTextureAddressMode::Clamp;
    samplerDef->SetBorderColor(0.0, 0.0, 0.0, 0.0);
    samplerDef->MaxAnisotropy = 16;
    samplerDef->MinMipLevel = -(FLT_MAX);
    samplerDef->MaxMipLevel = FLT_MAX;
    samplerDef->MipMapLevelOfDetailBias = 0.0f;
    samplerDef->CompareFunction = (int)spv::SamplerStateCompareFunction::Never;

    do {
        // read state name
        HlslToken state;
        if (! acceptIdentifier(state))
            break;  // end of list

        // FXC accepts any case
        TString stateName = *state.string;
        //std::transform(stateName.begin(), stateName.end(), stateName.begin(), ::tolower);

        if (! acceptTokenClass(EHTokAssign)) {
            expected("sampler state: assign");
            return false;
        }

        if (stateName == "Filter")
        {
            HlslToken filterMode;
            if (!acceptIdentifier(filterMode)) {
                expected("filter mode");
                return false;
            }
            TString filter = *filterMode.string;

            if (filter == "COMPARISON_MIN_MAG_LINEAR_MIP_POINT") {
                samplerDef->Filter = (int)spv::SamplerStateTextureFilterEnum::ComparisonMinMagLinearMipPoint;
            }
            else if (filter == "COMPARISON_MIN_MAG_MIP_POINT") {
                samplerDef->Filter = (int)spv::SamplerStateTextureFilterEnum::ComparisonPoint;
            }
            else if (filter == "MIN_MAG_LINEAR_MIP_POINT") {
                samplerDef->Filter = (int)spv::SamplerStateTextureFilterEnum::MinMagLinearMipPoint;
            }
            else if (filter == "MIN_MAG_MIP_LINEAR") {
                samplerDef->Filter = (int)spv::SamplerStateTextureFilterEnum::Linear;
            }
            else if (filter == "ANISOTROPIC") {
                samplerDef->Filter = (int)spv::SamplerStateTextureFilterEnum::Anisotropic;
            }
            else if (filter == "MIN_MAG_MIP_POINT") {
                samplerDef->Filter = (int)spv::SamplerStateTextureFilterEnum::Point;
            }
            else {
                error("Unknown sampler filter: " + filter);
                return false;
            }
        }
        else if (stateName == "ComparisonFunc")
        {
            HlslToken comparisonFuncToken;
            if (!acceptIdentifier(comparisonFuncToken)) {
                expected("comparison function");
                return false;
            }
            TString comparisonFunc = *comparisonFuncToken.string;

            if (comparisonFunc == "Never") {
                samplerDef->CompareFunction = (int)spv::SamplerStateCompareFunction::Never;
            }
            else if (comparisonFunc == "Less") {
                samplerDef->CompareFunction = (int)spv::SamplerStateCompareFunction::Less;
            }
            else if (comparisonFunc == "Equal") {
                samplerDef->CompareFunction = (int)spv::SamplerStateCompareFunction::Equal;
            }
            else if (comparisonFunc == "LessEqual") {
                samplerDef->CompareFunction = (int)spv::SamplerStateCompareFunction::LessEqual;
            }
            else if (comparisonFunc == "Greater") {
                samplerDef->CompareFunction = (int)spv::SamplerStateCompareFunction::Greater;
            }
            else if (comparisonFunc == "NotEqual") {
                samplerDef->CompareFunction = (int)spv::SamplerStateCompareFunction::NotEqual;
            }
            else if (comparisonFunc == "GreaterEqual") {
                samplerDef->CompareFunction = (int)spv::SamplerStateCompareFunction::GreaterEqual;
            }
            else if (comparisonFunc == "Always") {
                samplerDef->CompareFunction = (int)spv::SamplerStateCompareFunction::Always;
            }
            else {
                error("Unknown sampler comparison function: " + comparisonFunc);
                return false;
            }
        }
        else if (stateName == "AddressU" || stateName == "AddressV" || stateName == "AddressW")
        {
            HlslToken textureAddressModeToken;
            if (!acceptIdentifier(textureAddressModeToken)) {
                expected("address mode");
                return false;
            }
            TString textureAddressMode = *textureAddressModeToken.string;

            int textureAddressModeValue = 0;
            if (textureAddressMode == "Wrap") {
                textureAddressModeValue = (int)spv::SamplerStateTextureAddressMode::Wrap;
            }
            else if (textureAddressMode == "Mirror") {
                textureAddressModeValue = (int)spv::SamplerStateTextureAddressMode::Mirror;
            }
            else if (textureAddressMode == "Clamp") {
                textureAddressModeValue = (int)spv::SamplerStateTextureAddressMode::Clamp;
            }
            else if (textureAddressMode == "Border") {
                textureAddressModeValue = (int)spv::SamplerStateTextureAddressMode::Border;
            }
            else if (textureAddressMode == "MirrorOnce") {
                textureAddressModeValue = (int)spv::SamplerStateTextureAddressMode::MirrorOnce;
            }
            else {
                error("Unknown sampler texture address mode: " + textureAddressMode);
                return false;
            }

            if (stateName == "AddressU") samplerDef->AddressU = textureAddressModeValue;
            else if (stateName == "AddressV") samplerDef->AddressV = textureAddressModeValue;
            else samplerDef->AddressW = textureAddressModeValue;
        }
        else if (stateName == "BorderColor")
        {
            //format: BorderColor = (r,g,b,a);
            float r, g, b, a;

            if (!acceptTokenClass(EHTokLeftParen)) { expected("sampler state BorderColor: \"(\" missing"); return false; }

            if (!acceptLiteralFloatValue(r)) { expected("sampler state BorderColor: r component missing"); return false; }
            if (!acceptTokenClass(EHTokComma)) { expected("sampler state BorderColor: \",\" missing"); return false; }
            if (!acceptLiteralFloatValue(g)) { expected("sampler state BorderColor: g component missing"); return false; }
            if (!acceptTokenClass(EHTokComma)) { expected("sampler state BorderColor: \",\" missing"); return false; }
            if (!acceptLiteralFloatValue(b)) { expected("sampler state BorderColor: b component missing"); return false; }
            if (!acceptTokenClass(EHTokComma)) { expected("sampler state BorderColor: \",\" missing"); return false; }
            if (!acceptLiteralFloatValue(a)) { expected("sampler state BorderColor: a component missing"); return false; }

            if (!acceptTokenClass(EHTokRightParen)) { expected("sampler state BorderColor: \")\" missing"); return false; }

            samplerDef->SetBorderColor(r, g, b, a);
        }
        else if (stateName == "MinLOD")
        {
            float value;
            if (!acceptLiteralFloatValue(value)) {
                expected("sampler state MinLOD value");
                return false;
            }
            samplerDef->MinMipLevel = value;
        }
        else if (stateName == "MaxLOD")
        {
            float value;
            if (!acceptLiteralFloatValue(value)) {
                expected("sampler state MaxLOD value");
                return false;
            }
            samplerDef->MaxMipLevel = value;
        }
        else if (stateName == "MipLODBias")
        {
            float value;
            if (!acceptLiteralFloatValue(value)) {
                expected("sampler state MipMapLevelOfDetailBias value");
                return false;
            }
            samplerDef->MipMapLevelOfDetailBias = value;
        }
        else if (stateName == "MaxAnisotropy")
        {
            int value;
            if (!acceptLiteralIntValue(value)) {
                expected("sampler state MaxAnisotropy integer");
                return false;
            }

            samplerDef->MaxAnisotropy = value;
        }
        else
        {
            error("unknown sample texture state: " + stateName);
            return false;
        }

        // SEMICOLON
        if (! acceptTokenClass(EHTokSemicolon)) {
            expected("sampler state: semicolon");
            return false;
        }
    } while (true);

    if (! acceptTokenClass(EHTokRightBrace)) {
        error("Invalid sampler state syntax");
        return false;
    }

    return true;
}

// sampler_declaration_dx9
//    : SAMPLER identifier EQUAL sampler_type sampler_state
//
bool HlslGrammar::acceptSamplerDeclarationDX9(TType& /*type*/)
{
    if (! acceptTokenClass(EHTokSampler))
        return false;

    // TODO: remove this when DX9 style declarations are implemented.
    unimplemented("Direct3D 9 sampler declaration");

    // read sampler name
    HlslToken name;
    if (! acceptIdentifier(name)) {
        expected("sampler name");
        return false;
    }

    if (! acceptTokenClass(EHTokAssign)) {
        expected("=");
        return false;
    }

    return false;
}

bool HlslGrammar::GetTypeDeclarationLabel(const TType& type, const TString& variableName, TString& typeDeclarationLabel)
{
    typeDeclarationLabel = TType::getBasicString(type.getBasicType());

    if (type.isVector()) typeDeclarationLabel += std::to_string(type.getVectorSize()).c_str();
    else if (type.isMatrix()) typeDeclarationLabel += std::to_string(type.getMatrixCols()).c_str() + TString("x") + std::to_string(type.getMatrixRows()).c_str();

    typeDeclarationLabel += " " + variableName;

    if (type.isArray()) typeDeclarationLabel += TString("[") + std::to_string(type.getOuterArraySize()).c_str() + TString("]");

    return true;
}

// declaration
//      : attributes attributed_declaration
//      | NAMESPACE IDENTIFIER LEFT_BRACE declaration_list RIGHT_BRACE
//
// attributed_declaration
//      : sampler_declaration_dx9 post_decls SEMICOLON
//      | fully_specified_type                           // for cbuffer/tbuffer
//      | fully_specified_type declarator_list SEMICOLON // for non cbuffer/tbuffer
//      | fully_specified_type identifier function_parameters post_decls compound_statement  // function definition
//      | fully_specified_type identifier sampler_state post_decls compound_statement        // sampler definition
//      | typedef declaration
//
// declarator_list
//      : declarator COMMA declarator COMMA declarator...  // zero or more declarators
//
// declarator
//      : identifier array_specifier post_decls
//      | identifier array_specifier post_decls EQUAL assignment_expression
//      | identifier function_parameters post_decls                                          // function prototype
//
// Parsing has to go pretty far in to know whether it's a variable, prototype, or
// function definition, so the implementation below doesn't perfectly divide up the grammar
// as above.  (The 'identifier' in the first item in init_declarator list is the
// same as 'identifier' for function declarations.)
//
// This can generate more than one subtree, one per initializer or a function body.
// All initializer subtrees are put in their own aggregate node, making one top-level
// node for all the initializers. Each function created is a top-level node to grow
// into the passed-in nodeList.
//
// If 'nodeList' is passed in as non-null, it must an aggregate to extend for
// each top-level node the declaration creates. Otherwise, if only one top-level
// node in generated here, that is want is returned in nodeList.
//
bool HlslGrammar::acceptDeclaration(TIntermNode*& nodeList)
{
    // NAMESPACE IDENTIFIER LEFT_BRACE declaration_list RIGHT_BRACE
    if (acceptTokenClass(EHTokNamespace)) {
        HlslToken namespaceToken;

        //xksl extensions: we can declare several namespace (separated by a dot) into one declaration
        int countNameSpaceToPop = 0;
        while (true)
        {
            if (!acceptIdentifier(namespaceToken)) {
                expected("namespace name");
                return false;
            }

            countNameSpaceToPop++;
            parseContext.pushNamespace(*namespaceToken.string);

            if (acceptTokenClass(EHTokDot)) continue;
            break;
        }

        if (!acceptTokenClass(EHTokLeftBrace)) {
            expected("{");
            return false;
        }
        if (!acceptDeclarationList(nodeList)) {
            expected("declaration list");
            return false;
        }
        if (!acceptTokenClass(EHTokRightBrace)) {
            expected("}");
            return false;
        }

        while (countNameSpaceToPop-- > 0){
            parseContext.popNamespace();
        }
        return true;
    }

    bool declarator_list = false; // true when processing comma separation

    // attributes
    TFunctionDeclarator declarator;
    acceptAttributes(declarator.attributes);

    // typedef
    bool typedefDecl = acceptTokenClass(EHTokTypedef);

    TType declaredType;

    // DX9 sampler declaration use a different syntax
    // DX9 shaders need to run through HLSL compiler (fxc) via a back compat mode, it isn't going to
    // be possible to simultaneously compile D3D10+ style shaders and DX9 shaders. If we want to compile DX9
    // HLSL shaders, this will have to be a master level switch
    // As such, the sampler keyword in D3D10+ turns into an automatic sampler type, and is commonly used
    // For that reason, this line is commented out
    // if (acceptSamplerDeclarationDX9(declaredType))
    //     return true;

    bool forbidDeclarators = (peekTokenClass(EHTokCBuffer) || peekTokenClass(EHTokTBuffer));
    // fully_specified_type
    if (!acceptFullySpecifiedType(declaredType, nodeList))
        return false;

    parseContext.transferTypeAttributes(declarator.attributes, declaredType);

    // cbuffer and tbuffer end with the closing '}'.
    // No semicolon is included.
    if (forbidDeclarators)
        return true;

    if (this->xkslShaderParsingOperation != XkslShaderParsingOperationEnum::Undefined)
    {
        if (this->xkslShaderCurrentlyParsed == nullptr)
        {
            if (declaredType.getBasicType() != EbtShaderClass)
            {
                error("Cannot parse anything outside a XKSL shader boundary");
                return false;
            }
        }
    }

    //XKSL extension: the statement declaration can lead into inserting some new tokens
    TVector<HlslToken> listNewTokensToAddAfterDeclarationIsCompleted;

    // declarator_list
    //    : declarator
    //         : identifier
    HlslToken idToken;
    TIntermAggregate* initializers = nullptr;
    while (acceptIdentifier(idToken)) {
        TString *fullName = idToken.string;
        if (parseContext.symbolTable.atGlobalLevel())
            parseContext.getFullNamespaceName(fullName);
        if (peekTokenClass(EHTokLeftParen)) {
            // looks like function parameters

            // Potentially rename shader entry point function.  No-op most of the time.
            parseContext.renameShaderFunction(fullName);

            // function_parameters
            declarator.function = new TFunction(fullName, declaredType);
            if (!acceptFunctionParameters(*declarator.function)) {
                expected("function parameter list");
                return false;
            }

            // post_decls
            acceptPostDecls(declarator.function->getWritableType().getQualifier());

            // compound_statement (function body definition) or just a prototype?
            declarator.loc = token.loc;
            if (peekTokenClass(EHTokLeftBrace)) {
                if (declarator_list)
                    parseContext.error(idToken.loc, "function body can't be in a declarator list", "{", "");
                if (typedefDecl)
                    parseContext.error(idToken.loc, "function body can't be in a typedef", "{", "");
                return acceptFunctionDefinition(declarator, nodeList, nullptr);
            } else {
                if (typedefDecl)
                    parseContext.error(idToken.loc, "function typedefs not implemented", "{", "");
                parseContext.handleFunctionDeclarator(declarator.loc, *declarator.function, true);
            }
        } else {
            // A variable declaration. Fix the storage qualifier if it's a global.
            if (declaredType.getQualifier().storage == EvqTemporary && parseContext.symbolTable.atGlobalLevel())
                declaredType.getQualifier().storage = EvqUniform;

            // We can handle multiple variables per type declaration, so
            // the number of types can expand when arrayness is different.
            TType variableType;
            variableType.shallowCopy(declaredType);

            // recognize array_specifier
            TArraySizes* arraySizes = nullptr;
            acceptArraySpecifier(arraySizes);

            // Fix arrayness in the variableType
            if (declaredType.isImplicitlySizedArray()) {
                // Because "int[] a = int[2](...), b = int[3](...)" makes two arrays a and b
                // of different sizes, for this case sharing the shallow copy of arrayness
                // with the parseType oversubscribes it, so get a deep copy of the arrayness.
                variableType.newArraySizes(declaredType.getArraySizes());
            }
            if (arraySizes || variableType.isArray()) {
                // In the most general case, arrayness is potentially coming both from the
                // declared type and from the variable: "int[] a[];" or just one or the other.
                // Merge it all to the variableType, so all arrayness is part of the variableType.
                parseContext.arrayDimMerge(variableType, arraySizes);
            }

            // samplers accept immediate sampler state
            if (variableType.getBasicType() == EbtSampler) {
                if (! acceptSamplerState())
                    return false;
            }

            // post_decls
            acceptPostDecls(variableType.getQualifier());

            // EQUAL assignment_expression
            TIntermTyped* expressionNode = nullptr;
            if (acceptTokenClass(EHTokAssign)) {
                if (typedefDecl)
                    parseContext.error(idToken.loc, "can't have an initializer", "typedef", "");
                if (! acceptAssignmentExpression(expressionNode)) {
                    expected("initializer");
                    return false;
                }
            }

            // TODO: things scoped within an annotation need their own name space;
            // TODO: strings are not yet handled.
            if (variableType.getBasicType() != EbtString && parseContext.getAnnotationNestingLevel() == 0) {
                if (typedefDecl)
                    parseContext.declareTypedef(idToken.loc, *fullName, variableType);
                else if (variableType.getBasicType() == EbtBlock) {
                    parseContext.declareBlock(idToken.loc, variableType, fullName,
                                              variableType.isArray() ? &variableType.getArraySizes() : nullptr);
                    parseContext.declareStructBufferCounter(idToken.loc, variableType, *fullName);
                } else {
                    if (variableType.getQualifier().storage == EvqUniform && ! variableType.containsOpaque()) {
                        // this isn't really an individual variable, but a member of the $Global buffer
                        parseContext.growGlobalUniformBlock(idToken.loc, variableType, *fullName);
                    } else {

                        bool acceptCurrentDeclaration = true;

                        //=======================================================================================================
                        //=======================================================================================================
                        //XKSL extensions:
                        //if we have a declaration (with or without assignment) involving "Streams" types, we override and replace the expression and its Stream types by their actual meaning
                        if (this->xkslShaderParsingOperation == XkslShaderParsingOperationEnum::ParseXkslShaderMethodsDefinition)
                        {
                            TType* leftStreamType = nullptr;
                            TType* rightStreamType = nullptr;
                            if (variableType.getBasicType() == EbtStreams) leftStreamType = &variableType;
                            TIntermSymbol* rightIntermSymbol = expressionNode == nullptr? nullptr: expressionNode->getAsSymbolNode();
                            if (rightIntermSymbol != nullptr && rightIntermSymbol->getBasicType() == EbtStreams) rightStreamType = &(rightIntermSymbol->getWritableType());

                            bool anyStreamsTypeAreInvolved = (leftStreamType != nullptr || rightStreamType != nullptr);
                            if (anyStreamsTypeAreInvolved)
                            {
                                //special case where the expression involves "Streams" type and we declared the left type with the "var" keyword
                                if (leftStreamType == nullptr && variableType.getBasicType() == EbtUndefinedVar)
                                {
                                    variableType.setBasicType(EbtStreams);
                                    leftStreamType = &variableType;
                                }

                                //We will replace the current declaration by a new one
                                acceptCurrentDeclaration = false;

                                //Get the shader list of stream variables
                                XkslShaderDefinition* currentShader = getShaderCurrentlyParsed();
                                if (currentShader == nullptr) {error("Failed to get the current shader"); return false; }

                                //Build the struct definition and its assignment expression
                                TString structName = *(idToken.string);
                                TString expressionStructDef = currentShader->streamsTypeInfo.StreamStructDeclarationName + TString(" ") + structName;

                                TString replacementExpression = "";
                                if (leftStreamType != nullptr)
                                {
                                    bool isUndefinedStreamType = leftStreamType->GetStreamsTypeProperties() == nullptr || leftStreamType->GetStreamsTypeProperties()->IsUndefined;

                                    if (isUndefinedStreamType)
                                    {
                                        //replace the "Stream" declaration by its corresponding struct type
                                        replacementExpression += expressionStructDef;
                                    }
                                    else
                                    {
                                        error("Unprocessed Streams declaration (1)");
                                        return false;
                                    }
                                }
                                else
                                {
                                    error("Unprocessed Streams declaration (2)");
                                    return false;
                                }

                                if (rightStreamType != nullptr)
                                {
                                    bool isUndefinedStreamType = rightStreamType->GetStreamsTypeProperties() == nullptr || rightStreamType->GetStreamsTypeProperties()->IsUndefined;

                                    if (isUndefinedStreamType || !rightStreamType->GetStreamsTypeProperties()->IsFromStreamsKeyword)
                                    {
                                        error("Unprocessed Streams declaration (3)");
                                        return false;
                                    }
                                    else
                                    {
                                        //replace the "Stream" declaration by its corresponding struct type
                                        replacementExpression += TString(" = ") + currentShader->streamsTypeInfo.StreamStructAssignmentExpression;
                                    }
                                }
                                else if (rightIntermSymbol != nullptr)
                                {
                                    if (rightIntermSymbol->getBasicType() == EbtStruct)
                                    {
                                        //we have an declaration such like: "Streams backup3 = backup1;"
                                        //We convert the left Stream to a struct and assign it with the initial right struct
                                        replacementExpression += TString(" = ") + rightIntermSymbol->getName();
                                    }
                                    else
                                    {
                                        error("Unprocessed Streams declaration: rightIntermSymbol has an invalid type");
                                        return false;
                                    }
                                }
                                else
                                {
                                    //We have a Stream declaration without assignment: do nothing else
                                }

                                replacementExpression += ";";

                                //Precompute the list of tokens for our new expression
                                if (!getListTokensForExpression(replacementExpression, listNewTokensToAddAfterDeclarationIsCompleted)) {
                                    error("Failed to create the list of tokens for the Streams declaration expression"); return false;
                                }
                                //We remove the termination token from the list
                                while (listNewTokensToAddAfterDeclarationIsCompleted.size() > 0 && listNewTokensToAddAfterDeclarationIsCompleted.back().tokenClass == EHTokNone)
                                    listNewTokensToAddAfterDeclarationIsCompleted.pop_back();
                            }
                        }
                        //=======================================================================================================
                        //=======================================================================================================

                        if (acceptCurrentDeclaration)
                        {
                            // Declare the variable and add any initializer code to the AST.
                            // The top-level node is always made into an aggregate, as that's
                            // historically how the AST has been.
                            initializers = intermediate.growAggregate(initializers,
                                parseContext.declareVariable(idToken.loc, *fullName, variableType, expressionNode),
                                idToken.loc);
                        }
                    }
                }
            }
        }

        // COMMA
        if (acceptTokenClass(EHTokComma))
            declarator_list = true;
    }

    // The top-level initializer node is a sequence.
    if (initializers != nullptr)
        initializers->setOperator(EOpSequence);

    // Add the initializers' aggregate to the nodeList we were handed.
    if (nodeList)
        nodeList = intermediate.growAggregate(nodeList, initializers);
    else
        nodeList = initializers;

    //XKSL Extension
    if (listNewTokensToAddAfterDeclarationIsCompleted.size() > 0)
    {
        //New tokens are to be inserted BEFORE accepting the SemiColon token
        if (!peekTokenClass(EHTokSemicolon)) {
            error("An end of instruction token (;) is expected before we insert some new expression tokens");
            return false;
        }

        if (!insertListOfTokensAtCurrentPosition(listNewTokensToAddAfterDeclarationIsCompleted)) {
            error("Failed to insert the list of tokens for the Streams expression");
            return false;
        }
    }

    // SEMICOLON
    if (! acceptTokenClass(EHTokSemicolon)) {
        if (declaredType.getBasicType() != EbtShaderClass) //XKSL extension, exception wish shader: we can ommit the ";"
        {
            // This may have been a false detection of what appeared to be a declaration, but
            // was actually an assignment such as "float = 4", where "float" is an identifier.
            // We put the token back to let further parsing happen for cases where that may
            // happen.  This errors on the side of caution, and mostly triggers the error.
            if (peek() == EHTokAssign || peek() == EHTokLeftBracket || peek() == EHTokDot || peek() == EHTokComma) {
                recedeToken();
                return false;
            } else {
                expected(";");
                return false;
            }
        }
    }

    return true;
}

// control_declaration
//      : fully_specified_type identifier EQUAL expression
//
bool HlslGrammar::acceptControlDeclaration(TIntermNode*& node)
{
    node = nullptr;

    // fully_specified_type
    TType type;
    if (!acceptFullySpecifiedType(type))
        return false;

    // filter out type casts
    if (peekTokenClass(EHTokLeftParen)) {
        recedeToken();
        return false;
    }

    // identifier
    HlslToken idToken;
    if (! acceptIdentifier(idToken)) {
        expected("identifier");
        return false;
    }

    // EQUAL
    TIntermTyped* expressionNode = nullptr;
    if (! acceptTokenClass(EHTokAssign)) {
        expected("=");
        return false;
    }

    // expression
    if (! acceptExpression(expressionNode)) {
        expected("initializer");
        return false;
    }

    node = parseContext.declareVariable(idToken.loc, *idToken.string, type, expressionNode);

    return true;
}

// fully_specified_type
//      : type_specifier
//      | type_qualifier type_specifier
//
bool HlslGrammar::acceptFullySpecifiedType(TType& type)
{
    TIntermNode* nodeList = nullptr;
    return acceptFullySpecifiedType(type, nodeList);
}
bool HlslGrammar::acceptFullySpecifiedType(TType& type, TIntermNode*& nodeList)
{
    // type_qualifier
    TQualifier qualifier;
    qualifier.clear();
    if (! acceptQualifier(qualifier))
        return false;
    TSourceLoc loc = token.loc;
    TString* potentialTypeName = token.string;

    // type_specifier
    if (! acceptType(type, nodeList)) {
        // If this is not a type, we may have inadvertently gone down a wrong path
        // by parsing "sample", which can be treated like either an identifier or a
        // qualifier.  Back it out, if we did.
        if (qualifier.sample)
            recedeToken();

        return false;
    }

    if (type.getBasicType() == EbtBlock) {
        // the type was a block, which set some parts of the qualifier
        parseContext.mergeQualifiers(type.getQualifier(), qualifier);
        // further, it can create an anonymous instance of the block
        if (peek() != EHTokIdentifier)
        {
            //if parsing an xksl shader, we will declare the block by ourselves
            if (this->xkslShaderParsingOperation == XkslShaderParsingOperationEnum::Undefined)
            {
                parseContext.declareBlock(loc, type);
            }
        }
    } else {
        // Some qualifiers are set when parsing the type.  Merge those with
        // whatever comes from acceptQualifier.
        assert(qualifier.layoutFormat == ElfNone);

        qualifier.GSOutputPrimite = type.getQualifier().GSOutputPrimite; //GSOutputPrimitive is set with the type

        qualifier.layoutFormat = type.getQualifier().layoutFormat;
        qualifier.precision    = type.getQualifier().precision;

        if (type.getQualifier().storage == EvqOut ||
            type.getQualifier().storage == EvqBuffer) {
            qualifier.storage      = type.getQualifier().storage;
            qualifier.readonly     = type.getQualifier().readonly;
        }

        if (type.isBuiltIn())
            qualifier.builtIn = type.getQualifier().builtIn;

        type.getQualifier()    = qualifier;
    }

    return true;
}

// type_qualifier
//      : qualifier qualifier ...
//
// Zero or more of these, so this can't return false.
//
bool HlslGrammar::acceptQualifier(TQualifier& qualifier)
{
    do {
        switch (peek()) {
        case EHTokStatic:
            qualifier.isStatic = true;
            if (qualifier.storage != EvqConst)  //to avoid a "const static" declaration to move back the storage from const to global
            {
                qualifier.storage = parseContext.symbolTable.atGlobalLevel() ? EvqGlobal : EvqTemporary;
            }
            break;
        case EHTokExtern:
            // TODO: no meaning in glslang?
            break;
        case EHTokShared:
            // TODO: hint
            break;
        case EHTokGroupShared:
            qualifier.storage = EvqShared;
            break;
        case EHTokUniform:
            qualifier.storage = EvqUniform;
            break;
        case EHTokConst:
            qualifier.storage = EvqConst;
            break;
        case EHTokVolatile:
            qualifier.volatil = true;
            break;
        case EHTokLinear:
            qualifier.smooth = true;
            break;
        case EHTokCentroid:
            qualifier.centroid = true;
            break;
        case EHTokNointerpolation:
            qualifier.flat = true;
            break;
        case EHTokNoperspective:
            qualifier.nopersp = true;
            break;
        case EHTokSample:
            qualifier.sample = true;
            break;
        case EHTokRowMajor:
            qualifier.layoutMatrix = ElmColumnMajor;
            break;
        case EHTokColumnMajor:
            qualifier.layoutMatrix = ElmRowMajor;
            break;
        case EHTokPrecise:
            qualifier.noContraction = true;
            break;
        case EHTokIn:
            qualifier.storage = (qualifier.storage == EvqOut) ? EvqInOut : EvqIn;
            break;
        case EHTokOut:
            qualifier.storage = (qualifier.storage == EvqIn) ? EvqInOut : EvqOut;
            break;
        case EHTokInOut:
            qualifier.storage = EvqInOut;
            break;
        case EHTokLayout:
            if (! acceptLayoutQualifierList(qualifier))
                return false;
            continue;
        case EHTokGloballyCoherent:
            qualifier.coherent = true;
            break;
        case EHTokInline:
            // TODO: map this to SPIR-V function control
            break;

        // GS geometries: these are specified on stage input variables, and are an error (not verified here)
        // for output variables.
        case EHTokPoint:
            qualifier.GSInputPrimite = ElgPoints; //xksl extensions
            qualifier.storage = EvqIn;
            if (!parseContext.handleInputGeometry(token.loc, ElgPoints))
                return false;
            break;
        case EHTokLine:
            qualifier.GSInputPrimite = ElgLines; //xksl extensions
            qualifier.storage = EvqIn;
            if (!parseContext.handleInputGeometry(token.loc, ElgLines))
                return false;
            break;
        case EHTokTriangle:
            qualifier.GSInputPrimite = ElgTriangles; //xksl extensions
            qualifier.storage = EvqIn;
            if (!parseContext.handleInputGeometry(token.loc, ElgTriangles))
                return false;
            break;
        case EHTokLineAdj:
            qualifier.GSInputPrimite = ElgLinesAdjacency; //xksl extensions
            qualifier.storage = EvqIn;
            if (!parseContext.handleInputGeometry(token.loc, ElgLinesAdjacency))
                return false;
            break;
        case EHTokTriangleAdj:
            qualifier.GSInputPrimite = ElgTrianglesAdjacency; //xksl extensions
            qualifier.storage = EvqIn;
            if (!parseContext.handleInputGeometry(token.loc, ElgTrianglesAdjacency))
                return false;
            break;

        /*****************************************************************************************************/
        //XKSL qualifiers extensions
        case EHTokPublic:
        case EHTokPrivate:
        case EHTokProtected:
        case EHTokInternal:
            break;  //we simply ignore accessibility qualifiers for now
        case EHTokStage:
            if (qualifier.isStage) error("stage qualifier has been set twice");
            qualifier.isStage = true;
            break;
        case EHTokStream:
            if (qualifier.isStream) error("stream qualifier has been set twice");
            qualifier.isStream = true;
            break;
        case EHTokOverride:
            if (qualifier.isOverride) error("override qualifier has been set twice");
            qualifier.isOverride = true;
            break;
        case EHTokAbstract:
            if (qualifier.isAbstract) error("abstract qualifier has been set twice");
            qualifier.isAbstract = true;
            break;
        case EHTokClone:
            if (qualifier.isClone) error("clone qualifier has been set twice");
            qualifier.isClone = true;
            break;
        /*****************************************************************************************************/

        default:
            return true;
        }
        advanceToken();
    } while (true);
}

// layout_qualifier_list
//      : LAYOUT LEFT_PAREN layout_qualifier COMMA layout_qualifier ... RIGHT_PAREN
//
// layout_qualifier
//      : identifier
//      | identifier EQUAL expression
//
// Zero or more of these, so this can't return false.
//
bool HlslGrammar::acceptLayoutQualifierList(TQualifier& qualifier)
{
    if (! acceptTokenClass(EHTokLayout))
        return false;

    // LEFT_PAREN
    if (! acceptTokenClass(EHTokLeftParen))
        return false;

    do {
        // identifier
        HlslToken idToken;
        if (! acceptIdentifier(idToken))
            break;

        // EQUAL expression
        if (acceptTokenClass(EHTokAssign)) {
            TIntermTyped* expr;
            if (! acceptConditionalExpression(expr)) {
                expected("expression");
                return false;
            }
            parseContext.setLayoutQualifier(idToken.loc, qualifier, *idToken.string, expr);
        } else
            parseContext.setLayoutQualifier(idToken.loc, qualifier, *idToken.string);

        // COMMA
        if (! acceptTokenClass(EHTokComma))
            break;
    } while (true);

    // RIGHT_PAREN
    if (! acceptTokenClass(EHTokRightParen)) {
        expected(")");
        return false;
    }

    return true;
}

// template_type
//      : FLOAT
//      | DOUBLE
//      | INT
//      | DWORD
//      | UINT
//      | BOOL
//
bool HlslGrammar::acceptTemplateVecMatBasicType(TBasicType& basicType)
{
    switch (peek()) {
    case EHTokFloat:
        basicType = EbtFloat;
        break;
    case EHTokDouble:
        basicType = EbtDouble;
        break;
    case EHTokInt:
    case EHTokDword:
        basicType = EbtInt;
        break;
    case EHTokUint:
        basicType = EbtUint;
        break;
    case EHTokBool:
        basicType = EbtBool;
        break;
    default:
        return false;
    }

    advanceToken();

    return true;
}

// vector_template_type
//      : VECTOR
//      | VECTOR LEFT_ANGLE template_type COMMA integer_literal RIGHT_ANGLE
//
bool HlslGrammar::acceptVectorTemplateType(TType& type)
{
    if (! acceptTokenClass(EHTokVector))
        return false;

    if (! acceptTokenClass(EHTokLeftAngle)) {
        // in HLSL, 'vector' alone means float4.
        new(&type) TType(EbtFloat, EvqTemporary, 4);
        return true;
    }

    TBasicType basicType;
    if (! acceptTemplateVecMatBasicType(basicType)) {
        expected("scalar type");
        return false;
    }

    // COMMA
    if (! acceptTokenClass(EHTokComma)) {
        expected(",");
        return false;
    }

    // integer
    if (! peekTokenClass(EHTokIntConstant)) {
        expected("literal integer");
        return false;
    }

    TIntermTyped* vecSize;
    if (! acceptLiteral(vecSize))
        return false;

    const int vecSizeI = vecSize->getAsConstantUnion()->getConstArray()[0].getIConst();

    new(&type) TType(basicType, EvqTemporary, vecSizeI);

    if (vecSizeI == 1)
        type.makeVector();

    if (!acceptTokenClass(EHTokRightAngle)) {
        expected("right angle bracket");
        return false;
    }

    return true;
}

// matrix_template_type
//      : MATRIX
//      | MATRIX LEFT_ANGLE template_type COMMA integer_literal COMMA integer_literal RIGHT_ANGLE
//
bool HlslGrammar::acceptMatrixTemplateType(TType& type)
{
    if (! acceptTokenClass(EHTokMatrix))
        return false;

    if (! acceptTokenClass(EHTokLeftAngle)) {
        // in HLSL, 'matrix' alone means float4x4.
        new(&type) TType(EbtFloat, EvqTemporary, 0, 4, 4);
        return true;
    }

    TBasicType basicType;
    if (! acceptTemplateVecMatBasicType(basicType)) {
        expected("scalar type");
        return false;
    }

    // COMMA
    if (! acceptTokenClass(EHTokComma)) {
        expected(",");
        return false;
    }

    // integer rows
    if (! peekTokenClass(EHTokIntConstant)) {
        expected("literal integer");
        return false;
    }

    TIntermTyped* rows;
    if (! acceptLiteral(rows))
        return false;

    // COMMA
    if (! acceptTokenClass(EHTokComma)) {
        expected(",");
        return false;
    }

    // integer cols
    if (! peekTokenClass(EHTokIntConstant)) {
        expected("literal integer");
        return false;
    }

    TIntermTyped* cols;
    if (! acceptLiteral(cols))
        return false;

    new(&type) TType(basicType, EvqTemporary, 0,
                     rows->getAsConstantUnion()->getConstArray()[0].getIConst(),
                     cols->getAsConstantUnion()->getConstArray()[0].getIConst());

    if (!acceptTokenClass(EHTokRightAngle)) {
        expected("right angle bracket");
        return false;
    }

    return true;
}

// layout_geometry
//      : LINESTREAM
//      | POINTSTREAM
//      | TRIANGLESTREAM
//
bool HlslGrammar::acceptOutputPrimitiveGeometry(TLayoutGeometry& geometry)
{
    // read geometry type
    const EHlslTokenClass geometryType = peek();

    switch (geometryType) {
    case EHTokPointStream:    geometry = ElgPoints;        break;
    case EHTokLineStream:     geometry = ElgLineStrip;     break;
    case EHTokTriangleStream: geometry = ElgTriangleStrip; break;
    default:
        return false;  // not a layout geometry
    }

    advanceToken();  // consume the layout keyword
    return true;
}

// tessellation_decl_type
//      : INPUTPATCH
//      | OUTPUTPATCH
//
bool HlslGrammar::acceptTessellationDeclType(TBuiltInVariable& patchType)
{
    // read geometry type
    const EHlslTokenClass tessType = peek();

    switch (tessType) {
    case EHTokInputPatch:    patchType = EbvInputPatch;  break;
    case EHTokOutputPatch:   patchType = EbvOutputPatch; break;
    default:
        return false;  // not a tessellation decl
    }

    advanceToken();  // consume the keyword
    return true;
}

// tessellation_patch_template_type
//      : tessellation_decl_type LEFT_ANGLE type comma integer_literal RIGHT_ANGLE
//
bool HlslGrammar::acceptTessellationPatchTemplateType(TType& type)
{
    TBuiltInVariable patchType;

    if (! acceptTessellationDeclType(patchType))
        return false;
    
    if (! acceptTokenClass(EHTokLeftAngle))
        return false;

    if (! acceptType(type)) {
        expected("tessellation patch type");
        return false;
    }

    if (! acceptTokenClass(EHTokComma))
        return false;

    // integer size
    if (! peekTokenClass(EHTokIntConstant)) {
        expected("literal integer");
        return false;
    }

    TIntermTyped* size;
    if (! acceptLiteral(size))
        return false;

    TArraySizes* arraySizes = new TArraySizes;
    arraySizes->addInnerSize(size->getAsConstantUnion()->getConstArray()[0].getIConst());
    type.newArraySizes(*arraySizes);
    type.getQualifier().builtIn = patchType;

    if (! acceptTokenClass(EHTokRightAngle)) {
        expected("right angle bracket");
        return false;
    }

    return true;
}
    
// stream_out_template_type
//      : output_primitive_geometry_type LEFT_ANGLE type RIGHT_ANGLE
//
bool HlslGrammar::acceptStreamOutTemplateType(TType& type, TLayoutGeometry& geometry)
{
    geometry = ElgNone;

    if (! acceptOutputPrimitiveGeometry(geometry))
        return false;

    if (! acceptTokenClass(EHTokLeftAngle))
        return false;

    if (! acceptType(type)) {
        expected("stream output type");
        return false;
    }

    type.getQualifier().storage = EvqOut;
    type.getQualifier().builtIn = EbvGsOutputStream;

    if (! acceptTokenClass(EHTokRightAngle)) {
        expected("right angle bracket");
        return false;
    }

    return true;
}

// annotations
//      : LEFT_ANGLE declaration SEMI_COLON ... declaration SEMICOLON RIGHT_ANGLE
//
bool HlslGrammar::acceptAnnotations(TQualifier&)
{
    if (! acceptTokenClass(EHTokLeftAngle))
        return false;

    // note that we are nesting a name space
    parseContext.nestAnnotations();

    // declaration SEMI_COLON ... declaration SEMICOLON RIGHT_ANGLE
    do {
        // eat any extra SEMI_COLON; don't know if the grammar calls for this or not
        while (acceptTokenClass(EHTokSemicolon))
            ;

        if (acceptTokenClass(EHTokRightAngle))
            break;

        // declaration
        TIntermNode* node = nullptr;
        if (! acceptDeclaration(node)) {
            expected("declaration in annotation");
            return false;
        }
    } while (true);

    parseContext.unnestAnnotations();
    return true;
}

// subpass input type
//      : SUBPASSINPUT
//      | SUBPASSINPUT VECTOR LEFT_ANGLE template_type RIGHT_ANGLE
//      | SUBPASSINPUTMS
//      | SUBPASSINPUTMS VECTOR LEFT_ANGLE template_type RIGHT_ANGLE
bool HlslGrammar::acceptSubpassInputType(TType& type)
{
    // read subpass type
    const EHlslTokenClass subpassInputType = peek();

    bool multisample;

    switch (subpassInputType) {
    case EHTokSubpassInput:   multisample = false; break;
    case EHTokSubpassInputMS: multisample = true;  break;
    default:
        return false;  // not a subpass input declaration
    }

    advanceToken();  // consume the sampler type keyword

    TType subpassType(EbtFloat, EvqUniform, 4); // default type is float4

    if (acceptTokenClass(EHTokLeftAngle)) {
        if (! acceptType(subpassType)) {
            expected("scalar or vector type");
            return false;
        }

        const TBasicType basicRetType = subpassType.getBasicType() ;

        switch (basicRetType) {
        case EbtFloat:
        case EbtUint:
        case EbtInt:
        case EbtStruct:
            break;
        default:
            unimplemented("basic type in subpass input");
            return false;
        }

        if (! acceptTokenClass(EHTokRightAngle)) {
            expected("right angle bracket");
            return false;
        }
    }

    const TBasicType subpassBasicType = subpassType.isStruct() ? (*subpassType.getStruct())[0].type->getBasicType()
        : subpassType.getBasicType();

    TSampler sampler;
    sampler.setSubpass(subpassBasicType, multisample);

    // Remember the declared return type.  Function returns false on error.
    if (!parseContext.setTextureReturnType(sampler, subpassType, token.loc))
        return false;

    type.shallowCopy(TType(sampler, EvqUniform));

    return true;
}

// sampler_type
//      : SAMPLER
//      | SAMPLER1D
//      | SAMPLER2D
//      | SAMPLER3D
//      | SAMPLERCUBE
//      | SAMPLERSTATE
//      | SAMPLERCOMPARISONSTATE
bool HlslGrammar::acceptSamplerType(TType& type)
{
    // read sampler type
    const EHlslTokenClass samplerType = peek();

    // TODO: for DX9
    // TSamplerDim dim = EsdNone;

    bool isShadow = false;

    switch (samplerType) {
    case EHTokSampler:      break;
    case EHTokSampler1d:    /*dim = Esd1D*/; break;
    case EHTokSampler2d:    /*dim = Esd2D*/; break;
    case EHTokSampler3d:    /*dim = Esd3D*/; break;
    case EHTokSamplerCube:  /*dim = EsdCube*/; break;
    case EHTokSamplerState: break;
    case EHTokSamplerComparisonState:
        isShadow = true;
        break;
    default:
        return false;  // not a sampler declaration
    }

    advanceToken();  // consume the sampler type keyword

    TArraySizes* arraySizes = nullptr; // TODO: array

    TSampler sampler;
    sampler.setPureSampler(isShadow);

    type.shallowCopy(TType(sampler, EvqUniform, arraySizes));

    return true;
}

// texture_type
//      | BUFFER
//      | TEXTURE1D
//      | TEXTURE1DARRAY
//      | TEXTURE2D
//      | TEXTURE2DARRAY
//      | TEXTURE3D
//      | TEXTURECUBE
//      | TEXTURECUBEARRAY
//      | TEXTURE2DMS
//      | TEXTURE2DMSARRAY
//      | RWBUFFER
//      | RWTEXTURE1D
//      | RWTEXTURE1DARRAY
//      | RWTEXTURE2D
//      | RWTEXTURE2DARRAY
//      | RWTEXTURE3D

bool HlslGrammar::acceptTextureType(TType& type)
{
    const EHlslTokenClass textureType = peek();

    TSamplerDim dim = EsdNone;
    bool array = false;
    bool ms    = false;
    bool image = false;
    bool combined = true;

    switch (textureType) {
    case EHTokBuffer:            dim = EsdBuffer; combined = false;    break;
    case EHTokTexture1d:         dim = Esd1D;                          break;
    case EHTokTexture1darray:    dim = Esd1D; array = true;            break;
    case EHTokTexture2d:         dim = Esd2D;                          break;
    case EHTokTexture2darray:    dim = Esd2D; array = true;            break;
    case EHTokTexture3d:         dim = Esd3D;                          break;
    case EHTokTextureCube:       dim = EsdCube;                        break;
    case EHTokTextureCubearray:  dim = EsdCube; array = true;          break;
    case EHTokTexture2DMS:       dim = Esd2D; ms = true;               break;
    case EHTokTexture2DMSarray:  dim = Esd2D; array = true; ms = true; break;
    case EHTokRWBuffer:          dim = EsdBuffer; image=true;          break;
    case EHTokRWTexture1d:       dim = Esd1D; array=false; image=true; break;
    case EHTokRWTexture1darray:  dim = Esd1D; array=true;  image=true; break;
    case EHTokRWTexture2d:       dim = Esd2D; array=false; image=true; break;
    case EHTokRWTexture2darray:  dim = Esd2D; array=true;  image=true; break;
    case EHTokRWTexture3d:       dim = Esd3D; array=false; image=true; break;
    default:
        return false;  // not a texture declaration
    }

    advanceToken();  // consume the texture object keyword

    TType txType(EbtFloat, EvqUniform, 4); // default type is float4

    TIntermTyped* msCount = nullptr;

    // texture type: required for multisample types and RWBuffer/RWTextures!
    if (acceptTokenClass(EHTokLeftAngle)) {
        if (! acceptType(txType)) {
            expected("scalar or vector type");
            return false;
        }

        const TBasicType basicRetType = txType.getBasicType() ;

        switch (basicRetType) {
        case EbtFloat:
        case EbtUint:
        case EbtInt:
        case EbtStruct:
            break;
        default:
            unimplemented("basic type in texture");
            return false;
        }

        // Buffers can handle small mats if they fit in 4 components
        if (dim == EsdBuffer && txType.isMatrix()) {
            if ((txType.getMatrixCols() * txType.getMatrixRows()) > 4) {
                expected("components < 4 in matrix buffer type");
                return false;
            }

            // TODO: except we don't handle it yet...
            unimplemented("matrix type in buffer");
            return false;
        }

        if (!txType.isScalar() && !txType.isVector() && !txType.isStruct()) {
            expected("scalar, vector, or struct type");
            return false;
        }

        if (ms && acceptTokenClass(EHTokComma)) {
            // read sample count for multisample types, if given
            if (! peekTokenClass(EHTokIntConstant)) {
                expected("multisample count");
                return false;
            }

            if (! acceptLiteral(msCount))  // should never fail, since we just found an integer
                return false;
        }

        if (! acceptTokenClass(EHTokRightAngle)) {
            expected("right angle bracket");
            return false;
        }
    } else if (ms) {
        expected("texture type for multisample");
        return false;
    } else if (image) {
        expected("type for RWTexture/RWBuffer");
        return false;
    }

    TArraySizes* arraySizes = nullptr;
    const bool shadow = false; // declared on the sampler

    TSampler sampler;
    TLayoutFormat format = ElfNone;

    // Buffer, RWBuffer and RWTexture (images) require a TLayoutFormat.  We handle only a limit set.
    if (image || dim == EsdBuffer)
        format = parseContext.getLayoutFromTxType(token.loc, txType);

    const TBasicType txBasicType = txType.isStruct() ? (*txType.getStruct())[0].type->getBasicType()
        : txType.getBasicType();

    // Non-image Buffers are combined
    if (dim == EsdBuffer && !image) {
        sampler.set(txType.getBasicType(), dim, array);
    } else {
        // DX10 textures are separated.  TODO: DX9.
        if (image) {
            sampler.setImage(txBasicType, dim, array, shadow, ms);
        } else {
            sampler.setTexture(txBasicType, dim, array, shadow, ms);
        }
    }

    // Remember the declared return type.  Function returns false on error.
    if (!parseContext.setTextureReturnType(sampler, txType, token.loc))
        return false;

    // Force uncombined, if necessary
    if (!combined)
        sampler.combined = false;

    type.shallowCopy(TType(sampler, EvqUniform, arraySizes));
    type.getQualifier().layoutFormat = format;

    return true;
}

// If token is for a type, update 'type' with the type information,
// and return true and advance.
// Otherwise, return false, and don't advance
bool HlslGrammar::acceptType(TType& type)
{
    TIntermNode* nodeList = nullptr;
    return acceptType(type, nodeList);
}
bool HlslGrammar::acceptType(TType& type, TIntermNode*& nodeList)
{
    // Basic types for min* types, broken out here in case of future
    // changes, e.g, to use native halfs.
    static const TBasicType min16float_bt = EbtFloat;
    static const TBasicType min10float_bt = EbtFloat;
    static const TBasicType half_bt       = EbtFloat;
    static const TBasicType min16int_bt   = EbtInt;
    static const TBasicType min12int_bt   = EbtInt;
    static const TBasicType min16uint_bt  = EbtUint;

    // Some types might have turned into identifiers. Take the hit for checking
    // when this has happened.
    if (typeIdentifiers) {
        const char* identifierString = getTypeString(peek());
        if (identifierString != nullptr) {
            TString name = identifierString;
            // if it's an identifier, it's not a type
            if (parseContext.symbolTable.find(name) != nullptr)
                return false;
        }
    }

    bool isUnorm = false;
    bool isSnorm = false;

    // Accept snorm and unorm.  Presently, this is ignored, save for an error check below.
    switch (peek()) {
    case EHTokUnorm:
        isUnorm = true;
        advanceToken();  // eat the token
        break;
    case EHTokSNorm:
        isSnorm = true;
        advanceToken();  // eat the token
        break;
    default:
        break;
    }

    switch (peek()) {
    case EHTokVector:
        return acceptVectorTemplateType(type);
        break;

    case EHTokMatrix:
        return acceptMatrixTemplateType(type);
        break;

    case EHTokPointStream:            // fall through
    case EHTokLineStream:             // ...
    case EHTokTriangleStream:         // ...
        {
            TLayoutGeometry geometry;
            if (! acceptStreamOutTemplateType(type, geometry))
                return false;

            if (! parseContext.handleOutputGeometry(token.loc, geometry))
                return false;

            type.getQualifier().GSOutputPrimite = geometry;

            return true;
        }

    case EHTokInputPatch:             // fall through
    case EHTokOutputPatch:            // ...
        {
            if (! acceptTessellationPatchTemplateType(type))
                return false;

            return true;
        }

    case EHTokSampler:                // fall through
    case EHTokSampler1d:              // ...
    case EHTokSampler2d:              // ...
    case EHTokSampler3d:              // ...
    case EHTokSamplerCube:            // ...
    case EHTokSamplerState:           // ...
    case EHTokSamplerComparisonState: // ...
        return acceptSamplerType(type);
        break;

    case EHTokSubpassInput:           // fall through
    case EHTokSubpassInputMS:         // ...
        return acceptSubpassInputType(type);
        break;

    case EHTokBuffer:                 // fall through
    case EHTokTexture1d:              // ...
    case EHTokTexture1darray:         // ...
    case EHTokTexture2d:              // ...
    case EHTokTexture2darray:         // ...
    case EHTokTexture3d:              // ...
    case EHTokTextureCube:            // ...
    case EHTokTextureCubearray:       // ...
    case EHTokTexture2DMS:            // ...
    case EHTokTexture2DMSarray:       // ...
    case EHTokRWTexture1d:            // ...
    case EHTokRWTexture1darray:       // ...
    case EHTokRWTexture2d:            // ...
    case EHTokRWTexture2darray:       // ...
    case EHTokRWTexture3d:            // ...
    case EHTokRWBuffer:               // ...
        return acceptTextureType(type);
        break;

    case EHTokAppendStructuredBuffer:
    case EHTokByteAddressBuffer:
    case EHTokConsumeStructuredBuffer:
    case EHTokRWByteAddressBuffer:
    case EHTokRWStructuredBuffer:
    case EHTokStructuredBuffer:
        return acceptStructBufferType(type);
        break;

    case EHTokTextureBuffer:
        return acceptTextureBufferType(type);
        break;

    case EHTokConstantBuffer:
        return acceptConstantBufferType(type);

    case EHTokClass:
    case EHTokStruct:
    case EHTokCBuffer:
    case EHTokTBuffer:
    case EHTokRGroup:
        return acceptStruct(type, nodeList);

    case EHTokShaderClass:
    {
        bool res = acceptShaderClass(type);
        return res;
        break;
    }

    case EHTokThis:
    case EHTokBase:
    case EHTokIdentifier:

        if (peek() == EHTokIdentifier)
        {
            // An identifier could be for a user-defined type.
            // Note we cache the symbol table lookup, to save for a later rule
            // when this is not a type.
            if (parseContext.lookupUserType(*token.string, type) != nullptr)
            {
                advanceToken();
                return true;
            }
        }

        //XKSL extensions: we look if the type is a custom type defined by the current shader, or its parents
        if (this->xkslShaderParsingOperation == XkslShaderParsingOperationEnum::ParseXkslShaderConstVariables ||
            this->xkslShaderParsingOperation == XkslShaderParsingOperationEnum::ParseXkslShaderMembersAndMethodsDeclarations ||
            this->xkslShaderParsingOperation == XkslShaderParsingOperationEnum::ParseXkslShaderMethodsDefinition)
        {
            bool parsedShaderCustomType = false;
            int tokenIndex = getTokenCurrentIndex();
            XkslShaderDefinition* shader = getShaderCurrentlyParsed();
            if (shader != nullptr) {
                if (acceptShaderCustomType(shader->shaderFullName, type))
                    return true;
            }
            if (!recedeToTokenIndex(tokenIndex)) {
                error("Failed to recede to token index");
            }
        }
        return false;

    case EHTokVoid:
        new(&type) TType(EbtVoid);
        break;

    case EHTokString:
        new(&type) TType(EbtString);
        break;

    case EHTokVar:
        new(&type) TType(EbtUndefinedVar);
        break;

    case EHTokLinkType:
        new(&type) TType(EbtLinkType);
        break;

    case EHTokMemberNameType:
        new(&type) TType(EbtMemberNameType);
        break;

    case EHTokSemanticType:
        new(&type) TType(EbtSemanticType);
        break;

    case EHTokStreamsType:
        new(&type) TType(EbtStreams);
        break;

    case EHTokFloat:
        new(&type) TType(EbtFloat);
        break;
    case EHTokFloat1:
        new(&type) TType(EbtFloat);
        type.makeVector();
        break;
    case EHTokFloat2:
        new(&type) TType(EbtFloat, EvqTemporary, 2);
        break;
    case EHTokFloat3:
        new(&type) TType(EbtFloat, EvqTemporary, 3);
        break;
    case EHTokFloat4:
        new(&type) TType(EbtFloat, EvqTemporary, 4);
        break;

    case EHTokDouble:
        new(&type) TType(EbtDouble);
        break;
    case EHTokDouble1:
        new(&type) TType(EbtDouble);
        type.makeVector();
        break;
    case EHTokDouble2:
        new(&type) TType(EbtDouble, EvqTemporary, 2);
        break;
    case EHTokDouble3:
        new(&type) TType(EbtDouble, EvqTemporary, 3);
        break;
    case EHTokDouble4:
        new(&type) TType(EbtDouble, EvqTemporary, 4);
        break;

    case EHTokInt:
    case EHTokDword:
        new(&type) TType(EbtInt);
        break;
    case EHTokInt1:
        new(&type) TType(EbtInt);
        type.makeVector();
        break;
    case EHTokInt2:
        new(&type) TType(EbtInt, EvqTemporary, 2);
        break;
    case EHTokInt3:
        new(&type) TType(EbtInt, EvqTemporary, 3);
        break;
    case EHTokInt4:
        new(&type) TType(EbtInt, EvqTemporary, 4);
        break;

    case EHTokUint:
        new(&type) TType(EbtUint);
        break;
    case EHTokUint1:
        new(&type) TType(EbtUint);
        type.makeVector();
        break;
    case EHTokUint2:
        new(&type) TType(EbtUint, EvqTemporary, 2);
        break;
    case EHTokUint3:
        new(&type) TType(EbtUint, EvqTemporary, 3);
        break;
    case EHTokUint4:
        new(&type) TType(EbtUint, EvqTemporary, 4);
        break;

    case EHTokBool:
        new(&type) TType(EbtBool);
        break;
    case EHTokBool1:
        new(&type) TType(EbtBool);
        type.makeVector();
        break;
    case EHTokBool2:
        new(&type) TType(EbtBool, EvqTemporary, 2);
        break;
    case EHTokBool3:
        new(&type) TType(EbtBool, EvqTemporary, 3);
        break;
    case EHTokBool4:
        new(&type) TType(EbtBool, EvqTemporary, 4);
        break;

    case EHTokHalf:
        new(&type) TType(half_bt, EvqTemporary);
        break;
    case EHTokHalf1:
        new(&type) TType(half_bt, EvqTemporary);
        type.makeVector();
        break;
    case EHTokHalf2:
        new(&type) TType(half_bt, EvqTemporary, 2);
        break;
    case EHTokHalf3:
        new(&type) TType(half_bt, EvqTemporary, 3);
        break;
    case EHTokHalf4:
        new(&type) TType(half_bt, EvqTemporary, 4);
        break;

    case EHTokMin16float:
        new(&type) TType(min16float_bt, EvqTemporary, EpqMedium);
        break;
    case EHTokMin16float1:
        new(&type) TType(min16float_bt, EvqTemporary, EpqMedium);
        type.makeVector();
        break;
    case EHTokMin16float2:
        new(&type) TType(min16float_bt, EvqTemporary, EpqMedium, 2);
        break;
    case EHTokMin16float3:
        new(&type) TType(min16float_bt, EvqTemporary, EpqMedium, 3);
        break;
    case EHTokMin16float4:
        new(&type) TType(min16float_bt, EvqTemporary, EpqMedium, 4);
        break;

    case EHTokMin10float:
        new(&type) TType(min10float_bt, EvqTemporary, EpqMedium);
        break;
    case EHTokMin10float1:
        new(&type) TType(min10float_bt, EvqTemporary, EpqMedium);
        type.makeVector();
        break;
    case EHTokMin10float2:
        new(&type) TType(min10float_bt, EvqTemporary, EpqMedium, 2);
        break;
    case EHTokMin10float3:
        new(&type) TType(min10float_bt, EvqTemporary, EpqMedium, 3);
        break;
    case EHTokMin10float4:
        new(&type) TType(min10float_bt, EvqTemporary, EpqMedium, 4);
        break;

    case EHTokMin16int:
        new(&type) TType(min16int_bt, EvqTemporary, EpqMedium);
        break;
    case EHTokMin16int1:
        new(&type) TType(min16int_bt, EvqTemporary, EpqMedium);
        type.makeVector();
        break;
    case EHTokMin16int2:
        new(&type) TType(min16int_bt, EvqTemporary, EpqMedium, 2);
        break;
    case EHTokMin16int3:
        new(&type) TType(min16int_bt, EvqTemporary, EpqMedium, 3);
        break;
    case EHTokMin16int4:
        new(&type) TType(min16int_bt, EvqTemporary, EpqMedium, 4);
        break;

    case EHTokMin12int:
        new(&type) TType(min12int_bt, EvqTemporary, EpqMedium);
        break;
    case EHTokMin12int1:
        new(&type) TType(min12int_bt, EvqTemporary, EpqMedium);
        type.makeVector();
        break;
    case EHTokMin12int2:
        new(&type) TType(min12int_bt, EvqTemporary, EpqMedium, 2);
        break;
    case EHTokMin12int3:
        new(&type) TType(min12int_bt, EvqTemporary, EpqMedium, 3);
        break;
    case EHTokMin12int4:
        new(&type) TType(min12int_bt, EvqTemporary, EpqMedium, 4);
        break;

    case EHTokMin16uint:
        new(&type) TType(min16uint_bt, EvqTemporary, EpqMedium);
        break;
    case EHTokMin16uint1:
        new(&type) TType(min16uint_bt, EvqTemporary, EpqMedium);
        type.makeVector();
        break;
    case EHTokMin16uint2:
        new(&type) TType(min16uint_bt, EvqTemporary, EpqMedium, 2);
        break;
    case EHTokMin16uint3:
        new(&type) TType(min16uint_bt, EvqTemporary, EpqMedium, 3);
        break;
    case EHTokMin16uint4:
        new(&type) TType(min16uint_bt, EvqTemporary, EpqMedium, 4);
        break;

    case EHTokInt1x1:
        new(&type) TType(EbtInt, EvqTemporary, 0, 1, 1);
        break;
    case EHTokInt1x2:
        new(&type) TType(EbtInt, EvqTemporary, 0, 1, 2);
        break;
    case EHTokInt1x3:
        new(&type) TType(EbtInt, EvqTemporary, 0, 1, 3);
        break;
    case EHTokInt1x4:
        new(&type) TType(EbtInt, EvqTemporary, 0, 1, 4);
        break;
    case EHTokInt2x1:
        new(&type) TType(EbtInt, EvqTemporary, 0, 2, 1);
        break;
    case EHTokInt2x2:
        new(&type) TType(EbtInt, EvqTemporary, 0, 2, 2);
        break;
    case EHTokInt2x3:
        new(&type) TType(EbtInt, EvqTemporary, 0, 2, 3);
        break;
    case EHTokInt2x4:
        new(&type) TType(EbtInt, EvqTemporary, 0, 2, 4);
        break;
    case EHTokInt3x1:
        new(&type) TType(EbtInt, EvqTemporary, 0, 3, 1);
        break;
    case EHTokInt3x2:
        new(&type) TType(EbtInt, EvqTemporary, 0, 3, 2);
        break;
    case EHTokInt3x3:
        new(&type) TType(EbtInt, EvqTemporary, 0, 3, 3);
        break;
    case EHTokInt3x4:
        new(&type) TType(EbtInt, EvqTemporary, 0, 3, 4);
        break;
    case EHTokInt4x1:
        new(&type) TType(EbtInt, EvqTemporary, 0, 4, 1);
        break;
    case EHTokInt4x2:
        new(&type) TType(EbtInt, EvqTemporary, 0, 4, 2);
        break;
    case EHTokInt4x3:
        new(&type) TType(EbtInt, EvqTemporary, 0, 4, 3);
        break;
    case EHTokInt4x4:
        new(&type) TType(EbtInt, EvqTemporary, 0, 4, 4);
        break;

    case EHTokUint1x1:
        new(&type) TType(EbtUint, EvqTemporary, 0, 1, 1);
        break;
    case EHTokUint1x2:
        new(&type) TType(EbtUint, EvqTemporary, 0, 1, 2);
        break;
    case EHTokUint1x3:
        new(&type) TType(EbtUint, EvqTemporary, 0, 1, 3);
        break;
    case EHTokUint1x4:
        new(&type) TType(EbtUint, EvqTemporary, 0, 1, 4);
        break;
    case EHTokUint2x1:
        new(&type) TType(EbtUint, EvqTemporary, 0, 2, 1);
        break;
    case EHTokUint2x2:
        new(&type) TType(EbtUint, EvqTemporary, 0, 2, 2);
        break;
    case EHTokUint2x3:
        new(&type) TType(EbtUint, EvqTemporary, 0, 2, 3);
        break;
    case EHTokUint2x4:
        new(&type) TType(EbtUint, EvqTemporary, 0, 2, 4);
        break;
    case EHTokUint3x1:
        new(&type) TType(EbtUint, EvqTemporary, 0, 3, 1);
        break;
    case EHTokUint3x2:
        new(&type) TType(EbtUint, EvqTemporary, 0, 3, 2);
        break;
    case EHTokUint3x3:
        new(&type) TType(EbtUint, EvqTemporary, 0, 3, 3);
        break;
    case EHTokUint3x4:
        new(&type) TType(EbtUint, EvqTemporary, 0, 3, 4);
        break;
    case EHTokUint4x1:
        new(&type) TType(EbtUint, EvqTemporary, 0, 4, 1);
        break;
    case EHTokUint4x2:
        new(&type) TType(EbtUint, EvqTemporary, 0, 4, 2);
        break;
    case EHTokUint4x3:
        new(&type) TType(EbtUint, EvqTemporary, 0, 4, 3);
        break;
    case EHTokUint4x4:
        new(&type) TType(EbtUint, EvqTemporary, 0, 4, 4);
        break;

    case EHTokBool1x1:
        new(&type) TType(EbtBool, EvqTemporary, 0, 1, 1);
        break;
    case EHTokBool1x2:
        new(&type) TType(EbtBool, EvqTemporary, 0, 1, 2);
        break;
    case EHTokBool1x3:
        new(&type) TType(EbtBool, EvqTemporary, 0, 1, 3);
        break;
    case EHTokBool1x4:
        new(&type) TType(EbtBool, EvqTemporary, 0, 1, 4);
        break;
    case EHTokBool2x1:
        new(&type) TType(EbtBool, EvqTemporary, 0, 2, 1);
        break;
    case EHTokBool2x2:
        new(&type) TType(EbtBool, EvqTemporary, 0, 2, 2);
        break;
    case EHTokBool2x3:
        new(&type) TType(EbtBool, EvqTemporary, 0, 2, 3);
        break;
    case EHTokBool2x4:
        new(&type) TType(EbtBool, EvqTemporary, 0, 2, 4);
        break;
    case EHTokBool3x1:
        new(&type) TType(EbtBool, EvqTemporary, 0, 3, 1);
        break;
    case EHTokBool3x2:
        new(&type) TType(EbtBool, EvqTemporary, 0, 3, 2);
        break;
    case EHTokBool3x3:
        new(&type) TType(EbtBool, EvqTemporary, 0, 3, 3);
        break;
    case EHTokBool3x4:
        new(&type) TType(EbtBool, EvqTemporary, 0, 3, 4);
        break;
    case EHTokBool4x1:
        new(&type) TType(EbtBool, EvqTemporary, 0, 4, 1);
        break;
    case EHTokBool4x2:
        new(&type) TType(EbtBool, EvqTemporary, 0, 4, 2);
        break;
    case EHTokBool4x3:
        new(&type) TType(EbtBool, EvqTemporary, 0, 4, 3);
        break;
    case EHTokBool4x4:
        new(&type) TType(EbtBool, EvqTemporary, 0, 4, 4);
        break;

    case EHTokFloat1x1:
        new(&type) TType(EbtFloat, EvqTemporary, 0, 1, 1);
        break;
    case EHTokFloat1x2:
        new(&type) TType(EbtFloat, EvqTemporary, 0, 1, 2);
        break;
    case EHTokFloat1x3:
        new(&type) TType(EbtFloat, EvqTemporary, 0, 1, 3);
        break;
    case EHTokFloat1x4:
        new(&type) TType(EbtFloat, EvqTemporary, 0, 1, 4);
        break;
    case EHTokFloat2x1:
        new(&type) TType(EbtFloat, EvqTemporary, 0, 2, 1);
        break;
    case EHTokFloat2x2:
        new(&type) TType(EbtFloat, EvqTemporary, 0, 2, 2);
        break;
    case EHTokFloat2x3:
        new(&type) TType(EbtFloat, EvqTemporary, 0, 2, 3);
        break;
    case EHTokFloat2x4:
        new(&type) TType(EbtFloat, EvqTemporary, 0, 2, 4);
        break;
    case EHTokFloat3x1:
        new(&type) TType(EbtFloat, EvqTemporary, 0, 3, 1);
        break;
    case EHTokFloat3x2:
        new(&type) TType(EbtFloat, EvqTemporary, 0, 3, 2);
        break;
    case EHTokFloat3x3:
        new(&type) TType(EbtFloat, EvqTemporary, 0, 3, 3);
        break;
    case EHTokFloat3x4:
        new(&type) TType(EbtFloat, EvqTemporary, 0, 3, 4);
        break;
    case EHTokFloat4x1:
        new(&type) TType(EbtFloat, EvqTemporary, 0, 4, 1);
        break;
    case EHTokFloat4x2:
        new(&type) TType(EbtFloat, EvqTemporary, 0, 4, 2);
        break;
    case EHTokFloat4x3:
        new(&type) TType(EbtFloat, EvqTemporary, 0, 4, 3);
        break;
    case EHTokFloat4x4:
        new(&type) TType(EbtFloat, EvqTemporary, 0, 4, 4);
        break;

    case EHTokHalf1x1:
        new(&type) TType(half_bt, EvqTemporary, 0, 1, 1);
        break;
    case EHTokHalf1x2:
        new(&type) TType(half_bt, EvqTemporary, 0, 1, 2);
        break;
    case EHTokHalf1x3:
        new(&type) TType(half_bt, EvqTemporary, 0, 1, 3);
        break;
    case EHTokHalf1x4:
        new(&type) TType(half_bt, EvqTemporary, 0, 1, 4);
        break;
    case EHTokHalf2x1:
        new(&type) TType(half_bt, EvqTemporary, 0, 2, 1);
        break;
    case EHTokHalf2x2:
        new(&type) TType(half_bt, EvqTemporary, 0, 2, 2);
        break;
    case EHTokHalf2x3:
        new(&type) TType(half_bt, EvqTemporary, 0, 2, 3);
        break;
    case EHTokHalf2x4:
        new(&type) TType(half_bt, EvqTemporary, 0, 2, 4);
        break;
    case EHTokHalf3x1:
        new(&type) TType(half_bt, EvqTemporary, 0, 3, 1);
        break;
    case EHTokHalf3x2:
        new(&type) TType(half_bt, EvqTemporary, 0, 3, 2);
        break;
    case EHTokHalf3x3:
        new(&type) TType(half_bt, EvqTemporary, 0, 3, 3);
        break;
    case EHTokHalf3x4:
        new(&type) TType(half_bt, EvqTemporary, 0, 3, 4);
        break;
    case EHTokHalf4x1:
        new(&type) TType(half_bt, EvqTemporary, 0, 4, 1);
        break;
    case EHTokHalf4x2:
        new(&type) TType(half_bt, EvqTemporary, 0, 4, 2);
        break;
    case EHTokHalf4x3:
        new(&type) TType(half_bt, EvqTemporary, 0, 4, 3);
        break;
    case EHTokHalf4x4:
        new(&type) TType(half_bt, EvqTemporary, 0, 4, 4);
        break;

    case EHTokDouble1x1:
        new(&type) TType(EbtDouble, EvqTemporary, 0, 1, 1);
        break;
    case EHTokDouble1x2:
        new(&type) TType(EbtDouble, EvqTemporary, 0, 1, 2);
        break;
    case EHTokDouble1x3:
        new(&type) TType(EbtDouble, EvqTemporary, 0, 1, 3);
        break;
    case EHTokDouble1x4:
        new(&type) TType(EbtDouble, EvqTemporary, 0, 1, 4);
        break;
    case EHTokDouble2x1:
        new(&type) TType(EbtDouble, EvqTemporary, 0, 2, 1);
        break;
    case EHTokDouble2x2:
        new(&type) TType(EbtDouble, EvqTemporary, 0, 2, 2);
        break;
    case EHTokDouble2x3:
        new(&type) TType(EbtDouble, EvqTemporary, 0, 2, 3);
        break;
    case EHTokDouble2x4:
        new(&type) TType(EbtDouble, EvqTemporary, 0, 2, 4);
        break;
    case EHTokDouble3x1:
        new(&type) TType(EbtDouble, EvqTemporary, 0, 3, 1);
        break;
    case EHTokDouble3x2:
        new(&type) TType(EbtDouble, EvqTemporary, 0, 3, 2);
        break;
    case EHTokDouble3x3:
        new(&type) TType(EbtDouble, EvqTemporary, 0, 3, 3);
        break;
    case EHTokDouble3x4:
        new(&type) TType(EbtDouble, EvqTemporary, 0, 3, 4);
        break;
    case EHTokDouble4x1:
        new(&type) TType(EbtDouble, EvqTemporary, 0, 4, 1);
        break;
    case EHTokDouble4x2:
        new(&type) TType(EbtDouble, EvqTemporary, 0, 4, 2);
        break;
    case EHTokDouble4x3:
        new(&type) TType(EbtDouble, EvqTemporary, 0, 4, 3);
        break;
    case EHTokDouble4x4:
        new(&type) TType(EbtDouble, EvqTemporary, 0, 4, 4);
        break;

    default:
        return false;
    }

    advanceToken();

    if ((isUnorm || isSnorm) && !type.isFloatingDomain()) {
        parseContext.error(token.loc, "unorm and snorm only valid in floating point domain", "", "");
        return false;
    }

    return true;
}

TString HlslGrammar::getLabelForTokenType(EHlslTokenClass tokenType)
{
    switch (tokenType)
    {
        case EHTokString:      return "string";
        case EHTokBool:        return "bool";
        case EHTokInt:         return "int";
        case EHTokUint:        return "uint";
        case EHTokDword:       return "dword";
        case EHTokHalf:        return "half";
        case EHTokFloat:       return "float";
        case EHTokDouble:      return "double";
        case EHTokMin16float:  return "min16float";
        case EHTokMin10float:  return "min10float";
        case EHTokMin16int:    return "min16int";
        case EHTokMin12int:    return "min12int";
        case EHTokMin16uint:   return "min16uint";

        case EHTokBool1:       return "bool1";
        case EHTokBool2:       return "bool2";
        case EHTokBool3:       return "bool3";
        case EHTokBool4:       return "bool4";
        case EHTokFloat1:      return "float1";
        case EHTokFloat2:      return "float2";
        case EHTokFloat3:      return "float3";
        case EHTokFloat4:      return "float4";
        case EHTokInt1:        return "int1";
        case EHTokInt2:        return "int2";
        case EHTokInt3:        return "int3";
        case EHTokInt4:        return "int4";
        case EHTokDouble1:     return "double1";
        case EHTokDouble2:     return "double2";
        case EHTokDouble3:     return "double3";
        case EHTokDouble4:     return "double4";
        case EHTokUint1:       return "uint1";
        case EHTokUint2:       return "uint2";
        case EHTokUint3:       return "uint3";
        case EHTokUint4:       return "uint4";

        case EHTokHalf1:       return "half1";
        case EHTokHalf2:       return "half2";
        case EHTokHalf3:       return "half3";
        case EHTokHalf4:       return "half4";
        case EHTokMin16float1: return "min16float1";
        case EHTokMin16float2: return "min16float2";
        case EHTokMin16float3: return "min16float3";
        case EHTokMin16float4: return "min16float4";
        case EHTokMin10float1: return "min10float1";
        case EHTokMin10float2: return "min10float2";
        case EHTokMin10float3: return "min10float3";
        case EHTokMin10float4: return "min10float4";
        case EHTokMin16int1:   return "min16int1";
        case EHTokMin16int2:   return "min16int2";
        case EHTokMin16int3:   return "min16int3";
        case EHTokMin16int4:   return "min16int4";
        case EHTokMin12int1:   return "min12int1";
        case EHTokMin12int2:   return "min12int2";
        case EHTokMin12int3:   return "min12int3";
        case EHTokMin12int4:   return "min12int4";
        case EHTokMin16uint1:  return "min16uint1";
        case EHTokMin16uint2:  return "min16uint2";
        case EHTokMin16uint3:  return "min16uint3";
        case EHTokMin16uint4:  return "min16uint4";

        case EHTokBool1x1:     return "bool1x1";
        case EHTokBool1x2:     return "bool1x2";
        case EHTokBool1x3:     return "bool1x3";
        case EHTokBool1x4:     return "bool1x4";
        case EHTokBool2x1:     return "bool2x1";
        case EHTokBool2x2:     return "bool2x2";
        case EHTokBool2x3:     return "bool2x3";
        case EHTokBool2x4:     return "bool2x4";
        case EHTokBool3x1:     return "bool3x1";
        case EHTokBool3x2:     return "bool3x2";
        case EHTokBool3x3:     return "bool3x3";
        case EHTokBool3x4:     return "bool3x4";
        case EHTokBool4x1:     return "bool4x1";
        case EHTokBool4x2:     return "bool4x2";
        case EHTokBool4x3:     return "bool4x3";
        case EHTokBool4x4:     return "bool4x4";
        case EHTokInt1x1:      return "int1x1";
        case EHTokInt1x2:      return "int1x2";
        case EHTokInt1x3:      return "int1x3";
        case EHTokInt1x4:      return "int1x4";
        case EHTokInt2x1:      return "int2x1";
        case EHTokInt2x2:      return "int2x2";
        case EHTokInt2x3:      return "int2x3";
        case EHTokInt2x4:      return "int2x4";
        case EHTokInt3x1:      return "int3x1";
        case EHTokInt3x2:      return "int3x2";
        case EHTokInt3x3:      return "int3x3";
        case EHTokInt3x4:      return "int3x4";
        case EHTokInt4x1:      return "int4x1";
        case EHTokInt4x2:      return "int4x2";
        case EHTokInt4x3:      return "int4x3";
        case EHTokInt4x4:      return "int4x4";
        case EHTokUint1x1:     return "uint1x1";
        case EHTokUint1x2:     return "uint1x2";
        case EHTokUint1x3:     return "uint1x3";
        case EHTokUint1x4:     return "uint1x4";
        case EHTokUint2x1:     return "uint2x1";
        case EHTokUint2x2:     return "uint2x2";
        case EHTokUint2x3:     return "uint2x3";
        case EHTokUint2x4:     return "uint2x4";
        case EHTokUint3x1:     return "uint3x1";
        case EHTokUint3x2:     return "uint3x2";
        case EHTokUint3x3:     return "uint3x3";
        case EHTokUint3x4:     return "uint3x4";
        case EHTokUint4x1:     return "uint4x1";
        case EHTokUint4x2:     return "uint4x2";
        case EHTokUint4x3:     return "uint4x3";
        case EHTokUint4x4:     return "uint4x4";
        case EHTokFloat1x1:    return "float1x1";
        case EHTokFloat1x2:    return "float1x2";
        case EHTokFloat1x3:    return "float1x3";
        case EHTokFloat1x4:    return "float1x4";
        case EHTokFloat2x1:    return "float2x1";
        case EHTokFloat2x2:    return "float2x2";
        case EHTokFloat2x3:    return "float2x3";
        case EHTokFloat2x4:    return "float2x4";
        case EHTokFloat3x1:    return "float3x1";
        case EHTokFloat3x2:    return "float3x2";
        case EHTokFloat3x3:    return "float3x3";
        case EHTokFloat3x4:    return "float3x4";
        case EHTokFloat4x1:    return "float4x1";
        case EHTokFloat4x2:    return "float4x2";
        case EHTokFloat4x3:    return "float4x3";
        case EHTokFloat4x4:    return "float4x4";
        case EHTokDouble1x1:   return "double1x1";
        case EHTokDouble1x2:   return "double1x2";
        case EHTokDouble1x3:   return "double1x3";
        case EHTokDouble1x4:   return "double1x4";
        case EHTokDouble2x1:   return "double2x1";
        case EHTokDouble2x2:   return "double2x2";
        case EHTokDouble2x3:   return "double2x3";
        case EHTokDouble2x4:   return "double2x4";
        case EHTokDouble3x1:   return "double3x1";
        case EHTokDouble3x2:   return "double3x2";
        case EHTokDouble3x3:   return "double3x3";
        case EHTokDouble3x4:   return "double3x4";
        case EHTokDouble4x1:   return "double4x1";
        case EHTokDouble4x2:   return "double4x2";
        case EHTokDouble4x3:   return "double4x3";
        case EHTokDouble4x4:   return "double4x4";
        
        case EHTokLinkType:         return "LinkType";
        case EHTokMemberNameType:   return "MemberName";
        case EHTokSemanticType:     return "Semantic";
        case EHTokStreamsType:      return "Streams";
    }

    return "";
}

bool HlslGrammar::checkShaderGenericValuesExpression(TVector<TString*>& listGenericValues)
{
    if (acceptTokenClass(EHTokLeftAngle))
    {
        do
        {
            //get all token until we parse ',' or '>' symbol
            int tokenIndexStart = getTokenCurrentIndex();

            while (peek() != EHTokRightAngle && peek() != EHTokComma)
            {
                if (peek() == EHTokLeftBrace || peek() == EHTokNone) {
                    error("Invalid shader generic value expression");
                    return false;
                }

                advanceToken();
            }
            int tokenIndexEnd = getTokenCurrentIndex();

            if (tokenIndexStart == tokenIndexEnd) {
                error("Invalid shader generic value expression");
                return false;
            }

            TString valueExpression;
            for (int tokInd = tokenIndexStart; tokInd < tokenIndexEnd; ++tokInd)
            {
                HlslToken aToken = getTokenAtIndex(tokInd);
                if (aToken.tokenClass == EHTokNone) {
                    error("Invalid shader generic value expression");
                    return false;
                }

                TString tokenString;
                if (!convertTokenToString(aToken, tokenString)) {
                    error("Failed to convert the token into string");
                    return false;
                }

                valueExpression += tokenString;
            }

            if (valueExpression.size() == 0) {
                error("Failed to build the generic value expression string");
                return false;
            }

            listGenericValues.push_back(NewPoolTString(valueExpression.c_str()));

            if (acceptTokenClass(EHTokComma)) continue;
            else break;
        }
        while (true);

        if (!acceptTokenClass(EHTokRightAngle))
        {
            expected(">");
            return false;
        }
    }
    
    return true;
}

bool HlslGrammar::checkShaderGenericsList(TVector<TType*>& listGenericTypes)
{
    if (acceptTokenClass(EHTokLeftAngle))
    {
        do
        {
            //accept type definition
            TType* genericType = new TType;
            const TString* identifierName = nullptr;

            HlslToken typeToken = token;
            if (!acceptType(*genericType)) {
                expected("generic: invalid type (unrecognized)");
                return false;
            }
            genericType->getQualifier().storage = EvqConst;

            TString typeLabel = getLabelForTokenType(typeToken.tokenClass);
            if (typeLabel.size() == 0) {
                error("generic: invalid type (no label)");
                return false;
            }

            //filter out some invalid type
            switch (genericType->getBasicType()) {
                case EbtSampler:
                case EbtStruct:
                case EbtBlock:
                case EbtShaderClass:
                    error("invalid generic type");
                    return false;
            }

            HlslToken idToken = token;
            if (!acceptIdentifier(idToken)) {
                error("Invalid generic name");
                return false;
            }
            identifierName = idToken.string;

            TString typeExpression = typeLabel + TString(" ") + (*idToken.string);

            genericType->setTypeDefinitionExpression(typeExpression.c_str());
            genericType->setUserIdentifierName(identifierName->c_str());
            genericType->setFieldName(identifierName->c_str());
            genericType->setTypeName(identifierName->c_str());

            listGenericTypes.push_back(genericType);

            if (acceptTokenClass(EHTokComma)) continue;
            else break;

        } while (true);

        if (!acceptTokenClass(EHTokRightAngle))
        {
            expected(">");
            return false;
        }
    }

    return true;
}

// XKSL language extension
// shader
//      : shader IDENTIFIER post_decls LEFT_BRACE class_declaration_list RIGHT_BRACE
//
// class_type
//      : SHADER
//
bool HlslGrammar::acceptShaderClass(TType& type)
{
    TStorageQualifier storageQualifier = EvqTemporary;

    int tokenStart = getTokenCurrentIndex();

    if (!acceptTokenClass(EHTokShaderClass))
        return false;

    if (this->xkslShaderLibrary == nullptr)
    {
        error("shader library is null");
        return false;
    }

    if (this->xkslShaderCurrentlyParsed != nullptr)
    {
        unimplemented("Cannot accept nested shader class");
        return false;
    }

    // IDENTIFIER
    TString shaderName;
    if (!acceptIdentifierTokenClass(shaderName)) {
        expected("shader class name");
        return false;
    }

    if (shaderName.size() == 0) {
        error("Invalid shader name");
        return false;
    }

    //Any Generics?
    TVector<TType*> listGenericTypes;
    if (!checkShaderGenericsList(listGenericTypes)) {
        error("failed to check for shader generics");
        return false;
    }

    //Get shader parents
    TVector<ShaderInheritedParentDefinition> listParents;
    if (!acceptShaderClassParentsInheritance(listParents))
    {
        error("failed to check for shader parents");
        return false;
    }

    // LEFT_BRACE
    if (!acceptTokenClass(EHTokLeftBrace)) {
        expected("{");
        //listShaderCurrentlyParsed.pop_back();
        return false;
    }

    switch (xkslShaderParsingOperation)
    {
        case XkslShaderParsingOperationEnum::ParseXkslShaderDeclarations:
        {
            //=================================================================================
            // We're declaring a new shader class 

            //make sure the shader has not already been declared
            for (unsigned int i = 0; i < this->xkslShaderLibrary->listShaders.size(); ++i)
            {
                XkslShaderDefinition* aShader = this->xkslShaderLibrary->listShaders.at(i);
                if (aShader->shaderFullName == shaderName)
                {
                    error(TString("The shader is already declared: " + shaderName).c_str());
                    return false;
                }
            }

            //Create the new shader declaration
            XkslShaderDefinition* shaderDefinition = new XkslShaderDefinition();
            shaderDefinition->location = token.loc;
            shaderDefinition->shaderBaseName = shaderName;
            shaderDefinition->shaderFullName = shaderName;
            unsigned int countGenerics = (unsigned int)(listGenericTypes.size());
            for (unsigned int i = 0; i < countGenerics; ++i) {
                shaderDefinition->listGenerics.push_back(ShaderGenericAttribute(listGenericTypes[i]));
            }
            unsigned int countParents = (unsigned int)(listParents.size());
            for (unsigned int i = 0; i < countParents; ++i) {
                shaderDefinition->AddParent(listParents[i]);
            }

            //Don't use the namespace for now
            ////Add the namespace data to the shader
            //{
            //    TString nameSpace = parseContext.getFullNamespace();
            //    int nameSpaceLen = (int)nameSpace.size();
            //    if (nameSpaceLen > 0)
            //    {
            //        //convert the nameSpace to C# format (replace "::" by ".")
            //        TString reformatedNamespace;
            //        reformatedNamespace.resize(nameSpaceLen, 0);
            //        const char* srcPtr = nameSpace.c_str();
            //        int dstIndex = 0;
            //        bool addDot = false;
            //        char srcChar;
            //        while ((srcChar = *srcPtr++) != 0)
            //        {
            //            if (srcChar != ':') {
            //                if (addDot) {
            //                    reformatedNamespace[dstIndex++] = '.';
            //                    addDot = false;
            //                }
            //                reformatedNamespace[dstIndex++] = srcChar;
            //            }
            //            else addDot = true;
            //        }
            //
            //        shaderDefinition->shaderNameSpace = reformatedNamespace;
            //    }
            //}

            if (!advanceUntilEndOfBlock(EHTokRightBrace))
            {
                error("Error parsing until end of shader block");
                return false;
            }

            //Store the list of tokens into the new shader object
            int tokenEnd = getTokenCurrentIndex() - 1;
            CopyTokenBufferInto(shaderDefinition->listTokens, tokenStart, tokenEnd);
            //shaderDefinition->listTokens[shaderDefinition->listTokens.size() - 1].tokenClass = EHTokNone;

            //Add the shader definition at the END (warning: new shaders must be added at the end) of the list of parsed shader
            this->xkslShaderLibrary->AddNewShader(shaderDefinition);

            break;
        }

        case XkslShaderParsingOperationEnum::ParseXkslShaderNewTypesDefinition:
        case XkslShaderParsingOperationEnum::ParseXkslShaderConstVariables:
        case XkslShaderParsingOperationEnum::ParseXkslShaderMembersAndMethodsDeclarations:
        case XkslShaderParsingOperationEnum::ParseXkslShaderMethodsDefinition:
        {
            ////retrieve the shader's declaration in the list of declared shader
            //shaderDefinition = nullptr;
            //for (unsigned int i = 0; i < this->xkslShaderLibrary->listShaders.size(); ++i)
            //{
            //    XkslShaderDefinition* aShader = this->xkslShaderLibrary->listShaders.at(i);
            //    if (aShader->shaderName == *shaderName)
            //    {
            //        shaderDefinition = aShader;
            //        break;
            //    }
            //}

            XkslShaderDefinition* shaderToParse = this->xkslShaderToParse;
            if (shaderToParse == nullptr)
            {
                error(TString("No shader to parse set for: " + shaderName).c_str());
                return false;
            }

            this->xkslShaderCurrentlyParsed = shaderToParse;

            //======================================================================================
            //parse the shader new members and methods
            TVector<TShaderClassFunction> listMethodDeclaration;  //list of functions declared by the shader
            TVector<TShaderClassFunction>* plistMethodDeclaration = nullptr;
            if (xkslShaderParsingOperation == XkslShaderParsingOperationEnum::ParseXkslShaderMembersAndMethodsDeclarations) plistMethodDeclaration = &listMethodDeclaration;

            bool success = parseShaderMembersAndMethods(shaderToParse, plistMethodDeclaration);

            this->xkslShaderCurrentlyParsed = nullptr;

            //Add all defined methods
            if (success && plistMethodDeclaration != nullptr)
            {
                for (unsigned int i = 0; i < plistMethodDeclaration->size(); ++i)
                {
                    TShaderClassFunction& shaderFunction = plistMethodDeclaration->at(i);
                    shaderToParse->listMethods.push_back(shaderFunction);
                }
            }

            if (!success)
            {
                error("failed to parse the shader's members and methods declarations");
                return false;
            }

            // RIGHT_BRACE
            if (!acceptTokenClass(EHTokRightBrace)) {
                expected("}");
                return false;
            }

            break;
        }

        default:
            error("Undefined Xksl Shader Parsing Operation");
            return false;
    }

    //Even if we don't directly use a shader type, we return its basictype as EbtShaderClass
    new(&type) TType(EbtShaderClass);

    return true;
}

bool HlslGrammar::addShaderClassFunctionDeclaration(XkslShaderDefinition* shader, TFunction& function, TVector<TShaderClassFunction>& functionList)
{
    if (shader == nullptr)
    {
        error("shader parameter is null");
        return false;
    }

    const TString& newFunctionMangledName = function.getMangledName();

    //check if the function name already exists in the list
    int index = -1;
    for (unsigned int i = 0; i < functionList.size(); i++)
    {
        if (functionList.at(i).function->getMangledName() == newFunctionMangledName)
        {
            index = i;
            break;
        }
    }

    bool functionAlreadyDeclared = (index != -1);

    //Function declaration
    if (!functionAlreadyDeclared)
    {
        TShaderClassFunction shaderFunction;
        shaderFunction.shader = shader;
        shaderFunction.function = &function;
        shaderFunction.token = token;
        shaderFunction.bodyNode = nullptr;

        functionList.push_back(shaderFunction);
    }

    return true;
}

bool HlslGrammar::validateShaderDeclaredType(const TType& type)
{
    if (type.getBasicType() == EbtStruct)
    {
        //Check that a struct type is valid
        TTypeList* memberList = type.getWritableStruct();
        unsigned int countMembers = (unsigned int)memberList->size();
        if (countMembers == 0) {
            error("A struct type is empty");
            return false;
        }

        for (unsigned int indexInStruct = 0; indexInStruct < countMembers; ++indexInStruct)
        {
            TType& blockMemberType = *(memberList->at(indexInStruct).type);
            if (blockMemberType.getBasicType() == EbtVoid) {
                error("A struct type cannot contains a void type");
                return false;
            }
            if (!validateShaderDeclaredType(blockMemberType)) return false;
        }
    }

    switch (type.getBasicType())
    {
        case EbtUndefinedVar:
            error("a shader member or method cannot be defined as a \"var\" type: " + type.getTypeNameSafe());
            return false;

        case EbtShaderClass:
        case EbtLinkType:
        case EbtMemberNameType:
        case EbtSemanticType:
            error("a shader member or method has an invalid type: " + type.getTypeNameSafe());
            return false;
    }

    return true;
}

//Parse a shader class: check for all variables and functions declaration (don't parse into function definition)
bool HlslGrammar::parseShaderMembersAndMethods(XkslShaderDefinition* shader, TVector<TShaderClassFunction>* listMethodDeclaration)
{
    do {
        // some extra SEMI_COLON?
        while (acceptTokenClass(EHTokSemicolon)) {}

        // success on seeing the RIGHT_BRACE '}'
        if (peekTokenClass(EHTokRightBrace)) return true;

        ////any attributes?
        TFunctionDeclarator declarator;
        //acceptAttributes(declarator.attributes);

        // xksl extensions: members can have attributes
        TVector<TShaderMemberAttribute> memberAttributes;
        if (!checkForXkslStructMemberAttribute(memberAttributes)) {
            error("failed to check the struct member attribute");
            return false;
        }

        // typedef?
        if (peekTokenClass(EHTokTypedef)) {
            error("Cannot have \"typedef\" within a Shader class");
            return false;
        }

        //=================================================
        //declare either a shader's member or method

        // check if we're declaring a composition
        bool isStageBeforeCompose = acceptTokenClass(EHTokStage);  //stage before type is normally processed later, but we can have the case of having "stage compose"
        bool isComposition = acceptTokenClass(EHTokCompose);
        if (isComposition)
        {
            if (xkslShaderParsingOperation == XkslShaderParsingOperationEnum::ParseXkslShaderMembersAndMethodsDeclarations)
            {
                TShaderCompositionVariable composition;
                composition.location = token.loc;
                composition.shaderOwnerName = shader->shaderFullName;
                if (!acceptXkslShaderComposition(composition)) {
                    error("Failed to accept the composition declaration");
                    return false;
                }
                composition.shaderCompositionId = (int)(shader->listCompositions.size());
                composition.isStage = isStageBeforeCompose;
            
                //Make sure the shader does not already declared a composition with the same variable name
                for (unsigned int i = 0; i < shader->listCompositions.size(); ++i)
                {
                    if (shader->listCompositions[i].variableName == composition.variableName)
                    {
                        error((TString("A composition already exist with the variable name: ") + composition.variableName).c_str());
                        return false;
                    }
                }
                shader->listCompositions.push_back(composition);
            }
            else
            {
                if (!advanceUntilToken(EHTokSemicolon, true)) {
                    error("Error advancing until the end of the expression");
                    return false;
                }
            }

            continue;
        }
        else
        {
            if (isStageBeforeCompose) recedeToken();
        }

        // check the type (plus any post-declaration qualifiers)
        TType declaredType;
        int tokenCurrentIndex = getTokenCurrentIndex();
        if (!acceptFullySpecifiedType(declaredType))
        {
            //Unknown type
            if (xkslShaderParsingOperation == XkslShaderParsingOperationEnum::ParseXkslShaderNewTypesDefinition)
            {
                recedeToTokenIndex(tokenCurrentIndex);

                //but we're currently parsing the shader for new types definition, so we just skip the instruction or function for now
                TVector<EHlslTokenClass> listTokens; listTokens.push_back(EHTokSemicolon); listTokens.push_back(EHTokLeftBrace);
                if (!advanceUntilFirstTokenFromList(listTokens, true)){
                    error("Error advancing until the end of the expression");
                    return false;
                }

                if (peekTokenClass(EHTokLeftBrace)) { //we were parsing a function, advance until the end of the function
                    advanceToken();
                    if (!advanceUntilEndOfBlock(EHTokRightBrace)) {
                        error("Error parsing until end of function block");
                        return false;
                    }
                }

                continue;
            }
            else
            {   
                error("invalid keyword, member or function type: " + convertTokenToString(token));
                return false;
            }
        }
        
        if (this->shaderMethodOrMemberTypeCurrentlyParsed != nullptr) { error("Another shader's method or member type is currently being parsed"); return false; }
        this->shaderMethodOrMemberTypeCurrentlyParsed = &declaredType;

        if (memberAttributes.size() > 0)
        {
            declaredType.SetMemberAttributes(&memberAttributes);
        }

        const TString* identifierName = nullptr;
        bool acceptIdentifierAfterTypeDeclaration = true;
        if (declaredType.getBasicType() == EbtBlock)
        {
            //XKSL extension: cbuffer declaration, no identifier needed after the block declaration
            //with xksl, a cbuffer does not require ";" after its declaration, so accepting an identifier after the cbuffer declaration would create some confusions
            acceptIdentifierAfterTypeDeclaration = false;
            identifierName = &(declaredType.getTypeName());
        }

        if (acceptIdentifierAfterTypeDeclaration)
        {
            // get the Identifier (variable name)
            HlslToken idToken = token;
            if (acceptIdentifier(idToken))
            {
                identifierName = idToken.string;
            }
        }
        
        if (identifierName == nullptr)
        {
            //We're defining a new type (likely a new struct type), check the type name
            if (declaredType.getTypeNamePtr() == nullptr || declaredType.getTypeNamePtr()->size() == 0){ error("The type needs a name"); return false; }
        }
        else declaredType.setUserIdentifierName(identifierName->c_str());

        //validate type
        if (!validateShaderDeclaredType(declaredType))
        {
            error("Parsed an invalid shader type: " + declaredType.getTypeNameSafe());
            return false;
        }

        bool isAFunctionDeclaration = peekTokenClass(EHTokLeftParen);

        if (isAFunctionDeclaration)
        {
            //=======================================================================================================
            //Accept function declaration / definition

            if (identifierName == nullptr){
                error("A function cannot be unnamed");
                return false;
            }

            switch (xkslShaderParsingOperation)
            {
                case XkslShaderParsingOperationEnum::ParseXkslShaderConstVariables:
                case XkslShaderParsingOperationEnum::ParseXkslShaderNewTypesDefinition:
                {
                    //skip the method
                    if (!acceptTokenClass(EHTokLeftParen)) { expected("("); return false; }
                    if (!advanceUntilToken(EHTokRightParen, true)) { expected("failed to advance until )"); return false; }
                    advanceToken();
                    if (peekTokenClass(EHTokLeftBrace)) {
                        advanceToken();
                        if (!advanceUntilEndOfBlock(EHTokRightBrace)) {
                            error("Error parsing until end of function block");
                            return false;
                        }
                    }
                    //else {} //function without body, no need to do anything
                }
                break;

                case XkslShaderParsingOperationEnum::ParseXkslShaderMembersAndMethodsDeclarations:
                {
                    TString shaderBaseName = shader->shaderFullName;
                    /*TString shaderBaseName = shader->shaderBaseName;
                    if (shader->GetCountGenerics() > 0)
                    {
                        //Previously a function name was concatenated by the shaderFullName, but this name could be very long in some case
                        //(ComputeColorTextureScaledOffsetDynamicSampler_Material_DiffuseMap_TEXCOORD0_Material_Sampler_i0_rgba_Material_TextureScale_Material_TextureOffset_1)
                        //so we changed this
                        shaderBaseName += shader->GetShaderUniqueStringId();
                    }*/
                    TFunction* function = new TFunction(&shaderBaseName, identifierName, declaredType);

                    if (!acceptFunctionParameters(*function))
                    {
                        error("Failed to parse the function parameters");
                        return false;
                    }
                    //The function declared name will be mangled with the function parameters
                    declaredType.setUserIdentifierName(function->getDeclaredMangledName().c_str());
                    function->getWritableType().shallowCopy(declaredType);

                    bool functionIsUnresolvedUntilWeCallIt = false;
                    int countParams = function->getParamCount();
                    for (int k = 0; k < countParams; k++)
                    {
                        const TParameter& param = (*function)[k];
                        if (param.type->getBasicType() == EbtStreams)
                        {
                            functionIsUnresolvedUntilWeCallIt = true;
                        }
                    }
                    function->SetFunctionIsUnresolvedUntilWeCallIt(functionIsUnresolvedUntilWeCallIt);

                    //only record the method declaration
                    if (peekTokenClass(EHTokLeftBrace)) // compound_statement (function body definition) or just a declaration?
                    {
                        //function definition: but we add the function prototype only
                        if (!addShaderClassFunctionDeclaration(shader, *function, *listMethodDeclaration)) return false;

                        advanceToken();
                        if (!advanceUntilEndOfBlock(EHTokRightBrace)) {
                            error("Error parsing until end of function block");
                            return false;
                        }
                    }
                    else
                    {
                        //add the function prototype
                        if (!addShaderClassFunctionDeclaration(shader, *function, *listMethodDeclaration)) return false;
                    }
                }
                break;

                case XkslShaderParsingOperationEnum::ParseXkslShaderMethodsDefinition:
                {
                    TString shaderBaseName = shader->shaderFullName;
                    /*TString shaderBaseName = shader->shaderBaseName;
                    if (shader->GetCountGenerics() > 0)
                    {
                        //Previously a function name was concatenated by the shaderFullName, but this name could be very long in some case
                        //(ComputeColorTextureScaledOffsetDynamicSampler_Material_DiffuseMap_TEXCOORD0_Material_Sampler_i0_rgba_Material_TextureScale_Material_TextureOffset_1)
                        //so we changed this
                        shaderBaseName += shader->GetShaderUniqueStringId();
                    }*/
                    TFunction* tmpFunction = new TFunction(&shaderBaseName, identifierName, declaredType);

                    if (!acceptFunctionParameters(*tmpFunction))
                    {
                        error("Failed to parse the function parameters");
                        return false;
                    }
                    //The function declared name will be mangled with the function parameters
                    declaredType.setUserIdentifierName(tmpFunction->getDeclaredMangledName().c_str());
                    tmpFunction->getWritableType().shallowCopy(declaredType);

                    if (peekTokenClass(EHTokLeftBrace)) // compound_statement (function body definition) or just a declaration?
                    {
                        //Find the function from list of all declared function
                        TVector<TShaderClassFunction>& functionList = shader->listMethods;
                        const TString& newFunctionMangledName = tmpFunction->getMangledName();
                        TFunction* function = nullptr;
                        TShaderClassFunction* shaderClassFunction;
                        for (unsigned int i = 0; i < functionList.size(); ++i)
                        {
                            shaderClassFunction = &(functionList.at(i));
                            if (shaderClassFunction->function->getMangledName() == newFunctionMangledName)
                            {
                                function = functionList.at(i).function;
                                break;
                            }
                        }

                        if (function == nullptr)
                        {
                            error("failed to retrieve the function in the shader list of declared functions");
                            return false;
                        }
                        shaderClassFunction->token = token;  //in case of the token was those from the function prototype

                        bool canRecordFunctionDefinition = true;

                        if (function->IsFunctionIsUnresolvedUntilWeCallIt())
                        {
                            //The function definition will be created in another step, depending who is calling it
                            canRecordFunctionDefinition = false;
                        }

                        if (shaderClassFunction->bodyNode != nullptr)
                        {
                            //the function has already been processed (it already has a body), we can skip it
                            canRecordFunctionDefinition = false;
                        }

                        if (!canRecordFunctionDefinition)
                        {
                            advanceToken();
                            if (!advanceUntilEndOfBlock(EHTokRightBrace)) {
                                error("Error parsing until end of function block");
                                return false;
                            }
                        }
                        else
                        {
                            //==============================================================================================================
                            // start parsing a new shader method
                            if (listForeachArrayCompositionVariable.size() > 0) {
                                error("shader: list of foreach array composition variable should be empty"); return false;
                            }

                            this->functionCurrentlyParsed = function;

                            declarator.function = function;
                            TIntermNode* nodeList = nullptr;
                            if (!acceptFunctionDefinition(declarator, nodeList, nullptr))
                            {
                                this->functionCurrentlyParsed = nullptr;

                                if (this->hasAnyErrorToBeProcessedAtTheTop())
                                {
                                    //failed due to an unknown identifier. Just return false, the error will be processed later.

                                    //unset the function, in the case of we wanna try to parse its definition again later on
                                    if (!parseContext.unsetFunctionDefinition(declarator.loc, *declarator.function))
                                    {
                                        this->resetErrorsToBeProcessedAtTheTop();
                                        error("Failed to unset the function definition");
                                        return false;
                                    }
                                }
                                else
                                {
                                    error("shader: invalid function definition");
                                }

                                return false;
                            }
                            this->functionCurrentlyParsed = nullptr;

                            if (listForeachArrayCompositionVariable.size() > 0) {
                                error("shader: list of foreach array composition variable should be empty"); return false;
                            }
                            shaderClassFunction->bodyNode = nodeList;
                            //==============================================================================================================
                        }
                    }
                }
                break;

                default:
                    error("Invalid parsing operation");
                    return false;
            }
        }
        else
        {
            //=======================================================================================================
            //member declaration

            switch (xkslShaderParsingOperation)
            {
                case XkslShaderParsingOperationEnum::ParseXkslShaderConstVariables:
                case XkslShaderParsingOperationEnum::ParseXkslShaderMembersAndMethodsDeclarations:
                {
                    if (identifierName == nullptr)
                    {
                        //we skip the member/type declaration
                        if (declaredType.getBasicType() == EbtBlock) {
                            //block declaration have no identifier (and doesn't necessary require a semicolon after the cbuffer declaration)
                        }
                        else {
                            if (!advanceUntilToken(EHTokSemicolon, true)) {
                                error("Error advancing until the end of the expression");
                                return false;
                            }
                        }
                    }
                    else
                    {
                        bool addTheVariables;
                        bool isConstVariable = declaredType.getQualifier().storage == EvqConst;

                        if (isConstVariable && xkslShaderParsingOperation == XkslShaderParsingOperationEnum::ParseXkslShaderConstVariables) addTheVariables = true;
                        else if (!isConstVariable && xkslShaderParsingOperation == XkslShaderParsingOperationEnum::ParseXkslShaderMembersAndMethodsDeclarations) addTheVariables = true;
                        else addTheVariables = false;

                        if (!addTheVariables)
                        {
                            //we skip the member declaration
                            if (declaredType.getBasicType() == EbtBlock) {
                                //block declaration have no identifier (and doesn't necessary require a semicolon after the cbuffer declaration)
                            }
                            else {
                                if (!advanceUntilToken(EHTokSemicolon, true)) {
                                    error("Error advancing until the end of the expression");
                                    return false;
                                }
                            }
                        }
                        else
                        {
                            do
                            {
                                declaredType.setFieldName(*identifierName);
                                TSourceLoc memberLoc = token.loc;

                                // array_specifier
                                TArraySizes* arraySizes = nullptr;
                                acceptArraySpecifier(arraySizes);

                                // Fix arrayness in the variableType
                                if (declaredType.isImplicitlySizedArray()) {
                                    // Because "int[] a = int[2](...), b = int[3](...)" makes two arrays a and b
                                    // of different sizes, for this case sharing the shallow copy of arrayness
                                    // with the parseType oversubscribes it, so get a deep copy of the arrayness.
                                    declaredType.newArraySizes(declaredType.getArraySizes());
                                }
                                if (arraySizes || declaredType.isArray()) {
                                    // In the most general case, arrayness is potentially coming both from the
                                    // declared type and from the variable: "int[] a[];" or just one or the other.
                                    // Merge it all to the variableType, so all arrayness is part of the variableType.
                                    parseContext.arrayDimMerge(declaredType, arraySizes);
                                }

                                // samplers accept immediate sampler state
                                if (declaredType.getBasicType() == EbtSampler)
                                {
                                    TSamplerStateDefinition* samplerDef = nullptr;
                                    if (!acceptSamplerState(samplerDef))
                                        return false;

                                    if (samplerDef != nullptr)
                                    {
                                        declaredType.SetSamplerStateDef(samplerDef);
                                    }
                                }

                                TString userDefinedSemantic;
                                acceptPostDecls(declaredType.getQualifier(), &userDefinedSemantic);
                                if (userDefinedSemantic.length() > 0) {
                                    if (!declaredType.getQualifier().isStage)
                                    {
#ifdef XKSLANG_ENFORCE_NEW_XKSL_RULES
                                        error("A variable cannot declare a user-defined semantic if it's not a stage variable. Invalid variable: " + declaredType.getFieldName());
                                        return false;
#else
                                        warning("A variable declare a semantic but is not set as a stage variable: " + declaredType.getFieldName());
#endif
                                    }

                                    //add the semantic with the variable
                                    declaredType.setUserDefinedSemantic(userDefinedSemantic.c_str());
                                }

                                // EQUAL assignment_expression
                                TIntermTyped* expressionNode = nullptr;
                                TVector<HlslToken>* listTokens = nullptr;
                                HlslToken tokenAtAssignmentStart = token;
                                if (acceptTokenClass(EHTokAssign))
                                {
                                    if (!acceptAssignmentExpression(expressionNode))
                                    {
                                        //we're initializing a variable while parsing the shader declaration, but we meet unknown symbol
                                        //we ignore it for now and will resolve it during the next step
                                        expressionNode = nullptr;

                                        //advance until the next variable definition, or end of line
                                        recedeToToken(tokenAtAssignmentStart);
                                        TVector<EHlslTokenClass> toks; toks.push_back(EHTokSemicolon); toks.push_back(EHTokComma);
                                        if (!advanceUntilFirstTokenFromList(toks, true)){
                                            error("Error finding the end of assignment expression");
                                            return false;
                                        }

                                        HlslToken tokenAtAssignmentEnd = token;

                                        listTokens = new TVector<HlslToken>();
                                        getListPreviouslyParsedToken(tokenAtAssignmentStart, tokenAtAssignmentEnd, *listTokens);
                                    }
                                }

                                // add the new member into the list of class members
                                {
                                    XkslShaderDefinition::XkslShaderMember shaderMember;

                                    shaderMember.shader = shader;
                                    shaderMember.type = new TType(EbtVoid);
                                    shaderMember.type->shallowCopy(declaredType);
                                    shaderMember.loc = memberLoc;

                                    if (declaredType.getQualifier().storage == EvqConst && expressionNode != nullptr)
                                    {
                                        //the const value can directly be assigned with a const assignment if the expression has been resolved
                                        shaderMember.resolvedDeclaredExpression = expressionNode;
                                        shaderMember.expressionTokensList = nullptr;
                                    }
                                    else
                                    {
                                        shaderMember.resolvedDeclaredExpression = nullptr;
                                        shaderMember.expressionTokensList = listTokens;  //const values will be resolved later
                                    }

                                    shader->listParsedMembers.push_back(shaderMember);
                                }

                                // success on seeing the SEMICOLON coming up
                                if (peekTokenClass(EHTokSemicolon)) break;
                                if (declaredType.getBasicType() == EbtBlock) break; //exception with cbuffer. xksl shaders doesn't necessary require a semicolon after the cbuffer declaration

                                // declare another variable of the same type
                                // COMMA
                                if (!acceptTokenClass(EHTokComma)) {
                                    expected(",");
                                    return false;
                                }

                                {
                                    //get new type identifier
                                    HlslToken idToken = token;
                                    if (!acceptIdentifier(idToken))
                                    {
                                        expected("shader: member name");
                                        return false;
                                    }
                                    identifierName = idToken.string;
                                    declaredType.setUserIdentifierName(identifierName->c_str());
                                }

                            } while (true);
                        } //end of adding the variables
                    }
                }
                break;

                case XkslShaderParsingOperationEnum::ParseXkslShaderNewTypesDefinition:
                {
                    if (identifierName == nullptr)
                    {
                        //if identifier is null: we're defining a new type ("struct aStruct { };")
                        //add a new type definition into the shader

                        if (declaredType.getBasicType() != EbtStruct)
                        {
                            error("A shader defined a new type can only define a struct type");
                            return false;
                        }

                        declaredType.SetTypeAsDefinedByShader(true);
                        XkslShaderDefinition::XkslShaderMember shaderType;
                        shaderType.shader = shader;
                        shaderType.type = new TType(EbtVoid);
                        shaderType.type->shallowCopy(declaredType);
                        shaderType.loc = token.loc;
                        shader->listCustomTypes.push_back(shaderType);
                    }

                    //we skip the member/type declaration
                    if (declaredType.getBasicType() == EbtBlock) {
                        //block declaration have no identifier (and doesn't necessary require a semicolon after the cbuffer declaration)
                    }
                    else {
                        if (!advanceUntilToken(EHTokSemicolon, true)) {
                            error("Error advancing until the end of the expression");
                            return false;
                        }
                    }
                }
                break;

                case XkslShaderParsingOperationEnum::ParseXkslShaderMethodsDefinition:
                {
                    //we skip the member/type declaration
                    if (declaredType.getBasicType() == EbtBlock) {
                        //block declaration have no identifier (and doesn't necessary require a semicolon after the cbuffer declaration)
                    }
                    else {
                        if (!advanceUntilToken(EHTokSemicolon, true)) {
                            error("Error advancing until the end of the expression");
                            return false;
                        }
                    }
                }
                break;

                default:
                    error("Invalid parsing operation");
                    return false;
            }

            // SEMI_COLON
            if (!acceptTokenClass(EHTokSemicolon) && declaredType.getBasicType() != EbtBlock) {
                expected("; expected at the end of type declaration");
                return false;
            }
        }  //end member declaration

        this->shaderMethodOrMemberTypeCurrentlyParsed = nullptr;

    } while (true);
}

// struct
//      : struct_type IDENTIFIER post_decls LEFT_BRACE struct_declaration_list RIGHT_BRACE
//      | struct_type            post_decls LEFT_BRACE struct_declaration_list RIGHT_BRACE
//      | struct_type IDENTIFIER // use of previously declared struct type
//
// struct_type
//      : STRUCT
//      | CLASS
//      | CBUFFER
//      | TBUFFER
//
bool HlslGrammar::acceptStruct(TType& type, TIntermNode*& nodeList)
{
    // This storage qualifier will tell us whether it's an AST
    // block type or just a generic structure type.
    TStorageQualifier storageQualifier = EvqTemporary;
    bool readonly = false;

    bool canDeclareBufferInSymbolTable = true;
    if (this->xkslShaderParsingOperation != XkslShaderParsingOperationEnum::Undefined) canDeclareBufferInSymbolTable = false;

    bool isCBuffer = false;
    bool isRGroupBuffer = acceptTokenClass(EHTokRGroup);
    if (isRGroupBuffer)
    {
        storageQualifier = EvqUniform;
    }
    else
    {
        if (acceptTokenClass(EHTokCBuffer)) {
            // CBUFFER
            storageQualifier = EvqUniform;
            isCBuffer = true;
        } else if (acceptTokenClass(EHTokTBuffer)) {
            // TBUFFER
            storageQualifier = EvqBuffer;
            readonly = true;
        } else if (! acceptTokenClass(EHTokClass) && ! acceptTokenClass(EHTokStruct)) {
            // Neither CLASS nor STRUCT
            return false;
        }
    }

    // Now known to be one of CBUFFER, TBUFFER, CLASS, or STRUCT

    // IDENTIFIER
    TString structName = "";
    TString structSubpartName = "";
    if (peekTokenClass(EHTokIdentifier)) {
        structName = *token.string;
        advanceToken();

        //XKSL extensions: we can have struct name composed with a subpart (cbuffer PerLighting.subpart1)
        if (isCBuffer || isRGroupBuffer)
        {
            if (acceptTokenClass(EHTokDot))
            {
                if (peekTokenClass(EHTokIdentifier)) {
                    structSubpartName = *token.string;
                    advanceToken();
                }
            }
        }
    }

    //XKSL extensions: a shader can override struct or rgroup name with "MemberName" generic
    if (this->xkslShaderCurrentlyParsed != nullptr)
    {
        unsigned int countGenerics = (unsigned int)(this->xkslShaderCurrentlyParsed->listGenerics.size());
        for (unsigned int c = 0; c < countGenerics; c++)
        {
            const ShaderGenericAttribute& aGeneric = this->xkslShaderCurrentlyParsed->listGenerics[c];
            if (aGeneric.type->getBasicType() == EbtMemberNameType)
            {
                const TString& genericName = *(aGeneric.type->getUserIdentifierName());
                if (genericName == structName)
                {
                    structName = aGeneric.expressionConstValue;
                    break;
                }
            }
        }
    }

    // post_decls
    TQualifier postDeclQualifier;
    postDeclQualifier.clear();
    bool postDeclsFound = acceptPostDecls(postDeclQualifier);

    // LEFT_BRACE, or
    // struct_type IDENTIFIER
    if (! acceptTokenClass(EHTokLeftBrace)) {
        if (structName.size() > 0 && !postDeclsFound && parseContext.lookupUserType(structName, type) != nullptr) {
            // struct_type IDENTIFIER
            return true;
        } else {
            expected("{");
            return false;
        }
    }


    // struct_declaration_list
    TTypeList* typeList;
    // Save each member function so they can be processed after we have a fully formed 'this'.
    TVector<TFunctionDeclarator> functionDeclarators;

    parseContext.pushNamespace(structName);
    bool acceptedList = acceptStructDeclarationList(typeList, nodeList, functionDeclarators);
    parseContext.popNamespace();

    if (! acceptedList) {
        if (this->xkslShaderParsingOperation == XkslShaderParsingOperationEnum::ParseXkslShaderNewTypesDefinition)
            return false; //return false but it's not necessary an error (maybe the struct is using a custom type)

        expected("struct member declarations");
        return false;
    }

    // RIGHT_BRACE
    if (! acceptTokenClass(EHTokRightBrace)) {
        expected("}");
        return false;
    }

    // create the user-defined type
    if (storageQualifier == EvqTemporary)
        new(&type) TType(typeList, structName);
    else {
        postDeclQualifier.storage = storageQualifier;
        postDeclQualifier.readonly = readonly;
        new(&type) TType(typeList, structName, postDeclQualifier); // sets EbtBlock
    }

    if (structSubpartName.size() > 0)
    {
        type.setCbufferSubpartName(structSubpartName.c_str());
    }

    if (isRGroupBuffer)
    {
        type.getQualifier().isRGroup = true;
    }

    if (canDeclareBufferInSymbolTable)
    {
        parseContext.declareStruct(token.loc, structName, type);
    }

    // For member functions: now that we know the type of 'this', go back and
    // - add their implicit argument with 'this' (not to the mangling, just the argument list)
    // - parse the functions, their tokens were saved for deferred parsing (now)
    for (int b = 0; b < (int)functionDeclarators.size(); ++b) {
        // update signature
        if (functionDeclarators[b].function->hasImplicitThis())
            functionDeclarators[b].function->addThisParameter(type, intermediate.implicitThisName);
    }

    // All member functions get parsed inside the class/struct namespace and with the
    // class/struct members in a symbol-table level.
    parseContext.pushNamespace(structName);
    parseContext.pushThisScope(type, functionDeclarators);
    bool deferredSuccess = true;
    for (int b = 0; b < (int)functionDeclarators.size() && deferredSuccess; ++b) {
        // parse body
        if (!pushTokenStream(functionDeclarators[b].body)) error("Failed to call pushTokenStream function");
        if (! acceptFunctionBody(functionDeclarators[b], nodeList))
            deferredSuccess = false;
        if (!popTokenStream()) error("Failed to call popTokenStream function");
    }
    parseContext.popThisScope();
    parseContext.popNamespace();

    return deferredSuccess;
}

// constantbuffer
//    : CONSTANTBUFFER LEFT_ANGLE type RIGHT_ANGLE
bool HlslGrammar::acceptConstantBufferType(TType& type)
{
    if (! acceptTokenClass(EHTokConstantBuffer))
        return false;

    if (! acceptTokenClass(EHTokLeftAngle)) {
        expected("left angle bracket");
        return false;
    }
    
    TType templateType;
    if (! acceptType(templateType)) {
        expected("type");
        return false;
    }

    if (! acceptTokenClass(EHTokRightAngle)) {
        expected("right angle bracket");
        return false;
    }

    TQualifier postDeclQualifier;
    postDeclQualifier.clear();
    postDeclQualifier.storage = EvqUniform;

    if (templateType.isStruct()) {
        // Make a block from the type parsed as the template argument
        TTypeList* typeList = templateType.getWritableStruct();
        new(&type) TType(typeList, "", postDeclQualifier); // sets EbtBlock

        type.getQualifier().storage = EvqUniform;

        return true;
    } else {
        parseContext.error(token.loc, "non-structure type in ConstantBuffer", "", "");
        return false;
    }
}

// texture_buffer
//    : TEXTUREBUFFER LEFT_ANGLE type RIGHT_ANGLE
bool HlslGrammar::acceptTextureBufferType(TType& type)
{
    if (! acceptTokenClass(EHTokTextureBuffer))
        return false;

    if (! acceptTokenClass(EHTokLeftAngle)) {
        expected("left angle bracket");
        return false;
    }
    
    TType templateType;
    if (! acceptType(templateType)) {
        expected("type");
        return false;
    }

    if (! acceptTokenClass(EHTokRightAngle)) {
        expected("right angle bracket");
        return false;
    }

    templateType.getQualifier().storage = EvqBuffer;
    templateType.getQualifier().readonly = true;

    TType blockType(templateType.getWritableStruct(), "", templateType.getQualifier());

    blockType.getQualifier().storage = EvqBuffer;
    blockType.getQualifier().readonly = true;

    type.shallowCopy(blockType);

    return true;
}


// struct_buffer
//    : APPENDSTRUCTUREDBUFFER
//    | BYTEADDRESSBUFFER
//    | CONSUMESTRUCTUREDBUFFER
//    | RWBYTEADDRESSBUFFER
//    | RWSTRUCTUREDBUFFER
//    | STRUCTUREDBUFFER
bool HlslGrammar::acceptStructBufferType(TType& type)
{
    const EHlslTokenClass structBuffType = peek();

    // TODO: globallycoherent
    bool hasTemplateType = true;
    bool readonly = false;

    TStorageQualifier storage = EvqBuffer;
    TBuiltInVariable  builtinType = EbvNone;

    switch (structBuffType) {
    case EHTokAppendStructuredBuffer:
        builtinType = EbvAppendConsume;
        break;
    case EHTokByteAddressBuffer:
        hasTemplateType = false;
        readonly = true;
        builtinType = EbvByteAddressBuffer;
        break;
    case EHTokConsumeStructuredBuffer:
        builtinType = EbvAppendConsume;
        break;
    case EHTokRWByteAddressBuffer:
        hasTemplateType = false;
        builtinType = EbvRWByteAddressBuffer;
        break;
    case EHTokRWStructuredBuffer:
        builtinType = EbvRWStructuredBuffer;
        break;
    case EHTokStructuredBuffer:
        builtinType = EbvStructuredBuffer;
        readonly = true;
        break;
    default:
        return false;  // not a structure buffer type
    }

    advanceToken();  // consume the structure keyword

    // type on which this StructedBuffer is templatized.  E.g, StructedBuffer<MyStruct> ==> MyStruct
    TType* templateType = new TType;

    if (hasTemplateType) {
        if (! acceptTokenClass(EHTokLeftAngle)) {
            expected("left angle bracket");
            return false;
        }
    
        if (! acceptType(*templateType)) {
            expected("type");
            return false;
        }
        if (! acceptTokenClass(EHTokRightAngle)) {
            expected("right angle bracket");
            return false;
        }
    } else {
        // byte address buffers have no explicit type.
        TType uintType(EbtUint, storage);
        templateType->shallowCopy(uintType);
    }

    // Create an unsized array out of that type.
    // TODO: does this work if it's already an array type?
    TArraySizes unsizedArray;
    unsizedArray.addInnerSize(UnsizedArraySize);
    templateType->newArraySizes(unsizedArray);
    templateType->getQualifier().storage = storage;

    // field name is canonical for all structbuffers
    templateType->setFieldName("@data");

    TTypeList* blockStruct = new TTypeList;
    TTypeLoc  member = { templateType, token.loc };
    blockStruct->push_back(member);

    // This is the type of the buffer block (SSBO)
    TType blockType(blockStruct, "", templateType->getQualifier());

    blockType.getQualifier().storage = storage;
    blockType.getQualifier().readonly = readonly;
    blockType.getQualifier().builtIn = builtinType;

    // We may have created an equivalent type before, in which case we should use its
    // deep structure.
    parseContext.shareStructBufferType(blockType);

    type.shallowCopy(blockType);

    return true;
}

// struct_declaration_list
//      : struct_declaration SEMI_COLON struct_declaration SEMI_COLON ...
//
// struct_declaration
//      : fully_specified_type struct_declarator COMMA struct_declarator ...
//      | fully_specified_type IDENTIFIER function_parameters post_decls compound_statement // member-function definition
//
// struct_declarator
//      : IDENTIFIER post_decls
//      | IDENTIFIER array_specifier post_decls
//      | IDENTIFIER function_parameters post_decls                                         // member-function prototype
//
bool HlslGrammar::acceptStructDeclarationList(TTypeList*& typeList, TIntermNode*& nodeList,
                                              TVector<TFunctionDeclarator>& declarators)
{
    typeList = new TTypeList();
    HlslToken idToken;

    do {
        // success on seeing the RIGHT_BRACE coming up
        if (peekTokenClass(EHTokRightBrace))
            break;

        // struct_declaration
    
        bool declarator_list = false;

        // xksl extensions: struct members can have attributes
        TVector<TShaderMemberAttribute> memberAttributes;
        if (!checkForXkslStructMemberAttribute(memberAttributes)) {
            error("failed to check the struct member attribute");
            return false;
        }

        // fully_specified_type
        TType memberType;
        if (!acceptFullySpecifiedType(memberType, nodeList)) {
            if (this->xkslShaderParsingOperation == XkslShaderParsingOperationEnum::ParseXkslShaderNewTypesDefinition)
                return false; //return false but it's not necessary an error (maybe the struct is using a custom type)
            expected("member type");
            return false;
        }

        if (memberAttributes.size() > 0)
        {
            memberType.SetMemberAttributes(&memberAttributes);
        }

        // struct_declarator COMMA struct_declarator ...
        bool functionDefinitionAccepted = false;
        do {
            if (! acceptIdentifier(idToken)) {
                expected("member name");
                return false;
            }

            if (peekTokenClass(EHTokLeftParen)) {
                // function_parameters
                if (!declarator_list) {
                    declarators.resize(declarators.size() + 1);
                    // request a token stream for deferred processing
                    functionDefinitionAccepted = acceptMemberFunctionDefinition(nodeList, memberType, *idToken.string,
                                                                                declarators.back());
                    if (functionDefinitionAccepted)
                        break;
                }
                expected("member-function definition");
                return false;
            } else {
                // add it to the list of members
                TTypeLoc member = { new TType(EbtVoid), token.loc };
                member.type->shallowCopy(memberType);
                member.type->setFieldName(*idToken.string);
                typeList->push_back(member);

                // array_specifier
                TArraySizes* arraySizes = nullptr;
                acceptArraySpecifier(arraySizes);
                if (arraySizes)
                    typeList->back().type->newArraySizes(*arraySizes);

                // XKSL extensions: samplers accept immediate sampler state (or default one)
                if (member.type->getBasicType() == EbtSampler && member.type->getSampler().sampler)
                {
                    TSamplerStateDefinition* samplerDef = nullptr;
                    if (!acceptSamplerState(samplerDef))
                        return false;

                    if (samplerDef != nullptr)
                    {
                        member.type->SetSamplerStateDef(samplerDef);
                    }
                }

                acceptPostDecls(member.type->getQualifier());

                // EQUAL assignment_expression
                if (acceptTokenClass(EHTokAssign)) {
                    parseContext.warn(idToken.loc, "struct-member initializers ignored", "typedef", "");
                    TIntermTyped* expressionNode = nullptr;
                    if (! acceptAssignmentExpression(expressionNode)) {
                        expected("initializer");
                        return false;
                    }
                }
            }
            // success on seeing the SEMICOLON coming up
            if (peekTokenClass(EHTokSemicolon))
                break;

            // COMMA
            if (acceptTokenClass(EHTokComma))
                declarator_list = true;
            else {
                expected(",");
                return false;
            }

        } while (true);

        // SEMI_COLON
        if (! functionDefinitionAccepted && ! acceptTokenClass(EHTokSemicolon)) {
            expected(";");
            return false;
        }

    } while (true);

    return true;
}

// member_function_definition
//    | function_parameters post_decls compound_statement
//
// Expects type to have EvqGlobal for a static member and
// EvqTemporary for non-static member.
bool HlslGrammar::acceptMemberFunctionDefinition(TIntermNode*& nodeList, const TType& type, TString& memberName,
                                                 TFunctionDeclarator& declarator)
{
    bool accepted = false;

    TString* functionName = &memberName;
    parseContext.getFullNamespaceName(functionName);
    declarator.function = new TFunction(functionName, type);
    if (type.getQualifier().storage == EvqTemporary)
        declarator.function->setImplicitThis();
    else
        declarator.function->setIllegalImplicitThis();

    // function_parameters
    if (acceptFunctionParameters(*declarator.function)) {
        // post_decls
        acceptPostDecls(declarator.function->getWritableType().getQualifier());

        // compound_statement (function body definition)
        if (peekTokenClass(EHTokLeftBrace)) {
            declarator.loc = token.loc;
            declarator.body = new TVector<HlslToken>;
            accepted = acceptFunctionDefinition(declarator, nodeList, declarator.body);
        }
    } else
        expected("function parameter list");

    return accepted;
}

// function_parameters
//      : LEFT_PAREN parameter_declaration COMMA parameter_declaration ... RIGHT_PAREN
//      | LEFT_PAREN VOID RIGHT_PAREN
//
bool HlslGrammar::acceptFunctionParameters(TFunction& function)
{
    // LEFT_PAREN
    if (! acceptTokenClass(EHTokLeftParen))
        return false;

    // VOID RIGHT_PAREN
    if (! acceptTokenClass(EHTokVoid)) {
        do {
            // parameter_declaration
            if (! acceptParameterDeclaration(function))
                break;

            // COMMA
            if (! acceptTokenClass(EHTokComma))
                break;
        } while (true);
    }

    // RIGHT_PAREN
    if (! acceptTokenClass(EHTokRightParen)) {
        expected(")");
        return false;
    }

    return true;
}

// default_parameter_declaration
//      : EQUAL conditional_expression
//      : EQUAL initializer
bool HlslGrammar::acceptDefaultParameterDeclaration(const TType& type, TIntermTyped*& node)
{
    node = nullptr;

    // Valid not to have a default_parameter_declaration
    if (!acceptTokenClass(EHTokAssign))
        return true;

    if (!acceptConditionalExpression(node)) {
        if (!acceptInitializer(node))
            return false;

        // For initializer lists, we have to const-fold into a constructor for the type, so build
        // that.
        TFunction* constructor = parseContext.makeConstructorCall(token.loc, type);
        if (constructor == nullptr)  // cannot construct
            return false;

        TIntermTyped* arguments = nullptr;
        for (int i = 0; i < int(node->getAsAggregate()->getSequence().size()); i++)
            parseContext.handleFunctionArgument(constructor, arguments, node->getAsAggregate()->getSequence()[i]->getAsTyped());

        node = parseContext.handleFunctionCall(token.loc, constructor, node);
    }

    if (node == nullptr)
        return false;

    // If this is simply a constant, we can use it directly.
    if (node->getAsConstantUnion())
        return true;

    // Otherwise, it has to be const-foldable.
    TIntermTyped* origNode = node;

    node = intermediate.fold(node->getAsAggregate());

    if (node != nullptr && origNode != node)
        return true;

    parseContext.error(token.loc, "invalid default parameter value", "", "");

    return false;
}

// parameter_declaration
//      : attributes attributed_declaration
//
// attributed_declaration
//      : fully_specified_type post_decls [ = default_parameter_declaration ]
//      | fully_specified_type identifier array_specifier post_decls [ = default_parameter_declaration ]
//
bool HlslGrammar::acceptParameterDeclaration(TFunction& function)
{
    // attributes
    TAttributeMap attributes;
    acceptAttributes(attributes);

    // fully_specified_type
    TType* type = new TType;
    if (! acceptFullySpecifiedType(*type))
        return false;

    parseContext.transferTypeAttributes(attributes, *type);
        
    if (type->getBasicType() == EbtUndefinedVar)
    {
        parseContext.error(token.loc, (TString("A function cannot define a parameter as a \"var\" type. Function: ") + function.getDeclaredMangledName()).c_str(), "", "");
        return false;
    }

    // identifier
    HlslToken idToken;
    acceptIdentifier(idToken);

    // array_specifier
    TArraySizes* arraySizes = nullptr;
    acceptArraySpecifier(arraySizes);
    if (arraySizes) {
        if (arraySizes->isImplicit()) {
            parseContext.error(token.loc, "function parameter array cannot be implicitly sized", "", "");
            return false;
        }

        type->newArraySizes(*arraySizes);
    }

    // post_decls
    acceptPostDecls(type->getQualifier());

    TIntermTyped* defaultValue;
    if (!acceptDefaultParameterDeclaration(*type, defaultValue))
        return false;

    parseContext.paramFix(*type);

    // If any prior parameters have default values, all the parameters after that must as well.
    if (defaultValue == nullptr && function.getDefaultParamCount() > 0) {
        parseContext.error(idToken.loc, "invalid parameter after default value parameters", idToken.string->c_str(), "");
        return false;
    }

    TParameter param = { idToken.string, type, defaultValue };
    function.addParameter(param);

    return true;
}

// Do the work to create the function definition in addition to
// parsing the body (compound_statement).
//
// If 'deferredTokens' are passed in, just get the token stream,
// don't process.
//
bool HlslGrammar::acceptFunctionDefinition(TFunctionDeclarator& declarator, TIntermNode*& nodeList,
                                           TVector<HlslToken>* deferredTokens)
{
    parseContext.handleFunctionDeclarator(declarator.loc, *declarator.function, false /* not prototype */);

    if (deferredTokens)
        return captureBlockTokens(*deferredTokens);
    else
        return acceptFunctionBody(declarator, nodeList);
}

bool HlslGrammar::acceptFunctionBody(TFunctionDeclarator& declarator, TIntermNode*& nodeList)
{
    // we might get back an entry-point
    TIntermNode* entryPointNode = nullptr;

    // This does a pushScope()
    TIntermNode* functionNode = parseContext.handleFunctionDefinition(declarator.loc, *declarator.function,
                                                                      declarator.attributes, entryPointNode);

    // compound_statement
    TIntermNode* functionBody = nullptr;
    if (! acceptCompoundStatement(functionBody))
    {
        parseContext.popScope();
        return false;
    }

    // this does a popScope()
    parseContext.handleFunctionBody(declarator.loc, *declarator.function, functionBody, functionNode);

    // Hook up the 1 or 2 function definitions.
    nodeList = intermediate.growAggregate(nodeList, functionNode);
    nodeList = intermediate.growAggregate(nodeList, entryPointNode);

    return true;
}

// Accept an expression with parenthesis around it, where
// the parenthesis ARE NOT expression parenthesis, but the
// syntactically required ones like in "if ( expression )".
//
// Also accepts a declaration expression; "if (int a = expression)".
//
// Note this one is not set up to be speculative; as it gives
// errors if not found.
//
bool HlslGrammar::acceptParenExpression(TIntermTyped*& expression)
{
    // LEFT_PAREN
    if (! acceptTokenClass(EHTokLeftParen))
        expected("(");

    bool decl = false;
    TIntermNode* declNode = nullptr;
    decl = acceptControlDeclaration(declNode);
    if (decl) {
        if (declNode == nullptr || declNode->getAsTyped() == nullptr) {
            expected("initialized declaration");
            return false;
        } else
            expression = declNode->getAsTyped();
    } else {
        // no declaration
        if (! acceptExpression(expression)) {
            expected("expression");
            return false;
        }
    }

    // RIGHT_PAREN
    if (! acceptTokenClass(EHTokRightParen))
        expected(")");

    return true;
}

// The top-level full expression recognizer.
//
// expression
//      : assignment_expression COMMA assignment_expression COMMA assignment_expression ...
//
bool HlslGrammar::acceptExpression(TIntermTyped*& node)
{
    node = nullptr;

    // assignment_expression
    if (! acceptAssignmentExpression(node))
        return false;

    if (! peekTokenClass(EHTokComma))
        return true;

    do {
        // ... COMMA
        TSourceLoc loc = token.loc;
        advanceToken();

        // ... assignment_expression
        TIntermTyped* rightNode = nullptr;
        if (! acceptAssignmentExpression(rightNode)) {
            expected("assignment expression");
            return false;
        }

        node = intermediate.addComma(node, rightNode, loc);

        if (! peekTokenClass(EHTokComma))
            return true;
    } while (true);
}

// initializer
//      : LEFT_BRACE RIGHT_BRACE
//      | LEFT_BRACE initializer_list RIGHT_BRACE
//
// initializer_list
//      : assignment_expression COMMA assignment_expression COMMA ...
//
bool HlslGrammar::acceptInitializer(TIntermTyped*& node)
{
    // LEFT_BRACE
    if (! acceptTokenClass(EHTokLeftBrace))
        return false;

    // RIGHT_BRACE
    TSourceLoc loc = token.loc;
    if (acceptTokenClass(EHTokRightBrace)) {
        // a zero-length initializer list
        node = intermediate.makeAggregate(loc);
        return true;
    }

    // initializer_list
    node = nullptr;
    do {
        // assignment_expression
        TIntermTyped* expr;
        if (! acceptAssignmentExpression(expr)) {
            
            if (this->xkslShaderParsingOperation == XkslShaderParsingOperationEnum::ParseXkslShaderNewTypesDefinition ||
                this->xkslShaderParsingOperation == XkslShaderParsingOperationEnum::ParseXkslShaderConstVariables ||
                this->xkslShaderParsingOperation == XkslShaderParsingOperationEnum::ParseXkslShaderMembersAndMethodsDeclarations ||
                this->xkslShaderParsingOperation == XkslShaderParsingOperationEnum::ParseXkslShaderConstStatements)
                return false; //return false but it's not necessary an error: the expression can be resolved later

            expected("assignment expression in initializer list");
            return false;
        }

        const bool firstNode = (node == nullptr);

        node = intermediate.growAggregate(node, expr, loc);

        // If every sub-node in the list has qualifier EvqConst, the returned node becomes
        // EvqConst.  Otherwise, it becomes EvqTemporary. That doesn't happen with e.g.
        // EvqIn or EvqPosition, since the collection isn't EvqPosition if all the members are.
        if (firstNode && expr->getQualifier().storage == EvqConst)
            node->getQualifier().storage = EvqConst;
        else if (expr->getQualifier().storage != EvqConst)
            node->getQualifier().storage = EvqTemporary;

        // COMMA
        if (acceptTokenClass(EHTokComma)) {
            if (acceptTokenClass(EHTokRightBrace))  // allow trailing comma
                return true;
            continue;
        }

        // RIGHT_BRACE
        if (acceptTokenClass(EHTokRightBrace))
            return true;

        expected(", or }");
        return false;
    } while (true);
}

// Accept an assignment expression, where assignment operations
// associate right-to-left.  That is, it is implicit, for example
//
//    a op (b op (c op d))
//
// assigment_expression
//      : initializer
//      | conditional_expression
//      | conditional_expression assign_op conditional_expression assign_op conditional_expression ...
//
bool HlslGrammar::acceptAssignmentExpression(TIntermTyped*& node)
{
    // initializer
    if (peekTokenClass(EHTokLeftBrace)) {
        if (acceptInitializer(node))
            return true;

        if (this->xkslShaderParsingOperation == XkslShaderParsingOperationEnum::ParseXkslShaderNewTypesDefinition ||
            this->xkslShaderParsingOperation == XkslShaderParsingOperationEnum::ParseXkslShaderConstVariables ||
            this->xkslShaderParsingOperation == XkslShaderParsingOperationEnum::ParseXkslShaderMembersAndMethodsDeclarations ||
            this->xkslShaderParsingOperation == XkslShaderParsingOperationEnum::ParseXkslShaderConstStatements)
            return false; //return false but it's not necessary an error: the expression can be resolved later

        expected("initializer");
        return false;
    }

    // conditional_expression
    if (! acceptConditionalExpression(node))
        return false;

    // assignment operation?
    TOperator assignOp = HlslOpMap::assignment(peek());
    if (assignOp == EOpNull)
        return true;

    // assign_op
    TSourceLoc loc = token.loc;
    advanceToken();

    // conditional_expression assign_op conditional_expression ...
    // Done by recursing this function, which automatically
    // gets the right-to-left associativity.
    TIntermTyped* rightNode = nullptr;
    if (! acceptAssignmentExpression(rightNode)) {
        expected("assignment expression");
        return false;
    }

    //=======================================================================
    //XKSL extensions: if the left-value variable was defined with the "var" keyword, we assign its type now, by copying the type from the right-value expression
    if (rightNode != nullptr && node != nullptr && node->getType().getBasicType() == EbtUndefinedVar)
    {
        const TType& variableNewType = rightNode->getType();
        TIntermSymbol* variableSymbolNode = node->getAsSymbolNode();
        if (variableSymbolNode == nullptr) {
            parseContext.error(loc, "An unknown var type has been parsed without generating a symbol node", "", "");
            return false;
        }
        TString variableName = variableSymbolNode->getName();

        //Update the type of the variable recorded in the symbol table
        TSymbol* variableSymbol = parseContext.lookIfSymbolExistInSymbolTable(&variableName);
        if (variableSymbol == nullptr){
            parseContext.error(loc, "An unknown var type has been parsed but we cannot retrieve its symbol from the symbol table", "", "");
            return false;
        }

        TVariable* variable = variableSymbol->getAsVariable();
        if (variableSymbol == nullptr) {
            parseContext.error(loc, "An unknown var type has been parsed but we cannot retrieve its symbol from the symbol table", "", "");
            return false;
        }

        TQualifier variableQualifier = variable->getType().getQualifier();
        variable->setType(variableNewType);
        variable->getWritableType().setQualifier(variableQualifier);

        //update the current node type
        node->setType(rightNode->getType());
        node->getWritableType().setQualifier(variableQualifier);
    }
    //=======================================================================

    //=======================================================================================================
    //=======================================================================================================
    //XKSL extensions:
    //if we have an assignment expression involving "Streams" types, we replace the types by their actual meaning
    if (this->xkslShaderParsingOperation == XkslShaderParsingOperationEnum::ParseXkslShaderMethodsDefinition)
    {
        TType* leftStreamType = nullptr;
        TType* rightStreamType = nullptr;
        TIntermSymbol* leftIntermSymbol = node == nullptr ? nullptr : node->getAsSymbolNode();
        TIntermSymbol* rightIntermSymbol = rightNode == nullptr ? nullptr : rightNode->getAsSymbolNode();
        if (leftIntermSymbol != nullptr && leftIntermSymbol->getBasicType() == EbtStreams) leftStreamType = &(leftIntermSymbol->getWritableType());
        if (rightIntermSymbol != nullptr && rightIntermSymbol->getBasicType() == EbtStreams) rightStreamType = &(rightIntermSymbol->getWritableType());

        bool anyStreamsTypeAreInvolved = (leftStreamType != nullptr || rightStreamType != nullptr);
        if (anyStreamsTypeAreInvolved)
        {
            XkslShaderDefinition* currentShader = getShaderCurrentlyParsed();
            if (currentShader == nullptr) { error("Failed to get the current shader"); return false; }
            TString replacementExpression = "";

            if (!peekTokenClass(EHTokSemicolon))
            {
                error("; expected after a stream assignment");
                return false;
            }

            if (leftStreamType == nullptr && rightStreamType != nullptr)
            {
                if (rightStreamType->GetStreamsTypeProperties() == nullptr || rightStreamType->GetStreamsTypeProperties()->IsFromStreamsKeyword)
                {
                    //we process here the assignment "aStreamVar = streams" (where aStreamVar has normally been declared with a Streams type)
                    //We replace it by: "StreamStruct _tmpStreamX = {streams values}; symbolName = _tmpStreamX;"
                    if (leftIntermSymbol == nullptr || leftIntermSymbol->getBasicType() != EbtStruct)
                    {
                        error("Invalid Streams assignment expression: Left symbol is missing or has an invalid type");
                        return false;
                    }

                    //Limitation: multiple assignment expression such like: "s5 = s2 = streams;" will not work correctly
                    //The assignments would be done from left to right instead of right to left
                    //Due to the fact that we're skipping the current expression and rediffer it to the next instructions.
                    TString tmpStreamVariableName = TString("_tmpStreamsVar_") + TString(std::to_string(GetUniqueIndex()).c_str());
                    replacementExpression =
                        currentShader->streamsTypeInfo.StreamStructDeclarationName + " " + tmpStreamVariableName    //StreamStruct _tmpStreamX
                        + " = " + currentShader->streamsTypeInfo.StreamStructAssignmentExpression + "; "            // = {streams values};
                        + leftIntermSymbol->getName() + " = " + tmpStreamVariableName + ";";                        //symbolName = _tmpStreamX;
                }
                else
                {
                    error("Unprocessed Streams assignment expression (1)");
                    return false;
                }
            }
            else if (leftStreamType != nullptr && rightStreamType == nullptr)
            {
                //we process here the assignment "streams = aStreamVar" (where aStreamVar has normally been declared with a Streams type)
                if (rightIntermSymbol == nullptr || rightIntermSymbol->getBasicType() != EbtStruct)
                {
                    error("Invalid \"streams = symbol\" assignment expression: right symbol is missing or has an invalid type");
                    return false;
                }
                if (leftStreamType->GetStreamsTypeProperties() == nullptr || leftStreamType->GetStreamsTypeProperties()->IsFromStreamsKeyword == false)
                {
                    error("Invalid \"streams = symbol\" assignment expression: left Stream variable has missing or invalid properties");
                    return false;
                }

                TString structNamePlusDot = rightIntermSymbol->getName() + ".";
                int countStreamVariables = currentShader->streamsTypeInfo.GetCountStreamVariables();
                for (int m = 0; m < countStreamVariables; ++m)
                {
                    const TString& memberName = currentShader->streamsTypeInfo.GetStreamVariableName(m);
                    replacementExpression += "streams." + memberName + " = " + structNamePlusDot + memberName + ";";
                }
            }
            else
            {
                //The only case this can happens is when the left variable was defined with "var" keyword, then assign from a "streams"
                //We don't process this case for now
                error("Unprocessed Streams assignment expression (2)");
                return false;
            }

            if (replacementExpression.size() > 0)
            {
                //Precompute the list of tokens for our new expression
                TVector<HlslToken> listNewTokensToAddAfterDeclarationIsCompleted;
                if (!getListTokensForExpression(replacementExpression, listNewTokensToAddAfterDeclarationIsCompleted)) {
                    error("Failed to create the list of tokens for the Streams assignment expression"); return false;
                }

                //We remove the termination token from the list
                while (listNewTokensToAddAfterDeclarationIsCompleted.size() > 0 && listNewTokensToAddAfterDeclarationIsCompleted.back().tokenClass == EHTokNone)
                    listNewTokensToAddAfterDeclarationIsCompleted.pop_back();

                if (listNewTokensToAddAfterDeclarationIsCompleted.size() > 0)
                {
                    //New tokens are to be inserted BEFORE accepting the SemiColon token
                    if (!peekTokenClass(EHTokSemicolon)) {
                        error("An end of instruction token (;) is expected before we insert some new expression tokens");
                        return false;
                    }

                    if (!insertListOfTokensAtCurrentPosition(listNewTokensToAddAfterDeclarationIsCompleted)) {
                        error("Failed to insert the list of tokens for the Streams expression");
                        return false;
                    }
                }
            }

            //We return directly, without processing the initial assignment expression
            return true;
        }
    }
    //=======================================================================================================
    //=======================================================================================================

    node = parseContext.handleAssign(loc, assignOp, node, rightNode);
    node = parseContext.handleLvalue(loc, "assign", node);

    if (node == nullptr) {
        parseContext.error(loc, "could not create assignment", "", "");
        return false;
    }

    if (! peekTokenClass(EHTokComma))
        return true;

    return true;
}

// Accept a conditional expression, which associates right-to-left,
// accomplished by the "true" expression calling down to lower
// precedence levels than this level.
//
// conditional_expression
//      : binary_expression
//      | binary_expression QUESTION expression COLON assignment_expression
//
bool HlslGrammar::acceptConditionalExpression(TIntermTyped*& node)
{
    // binary_expression
    if (! acceptBinaryExpression(node, PlLogicalOr))
        return false;

    if (! acceptTokenClass(EHTokQuestion))
        return true;

    node = parseContext.convertConditionalExpression(token.loc, node, false);
    if (node == nullptr)
        return false;

    ++parseContext.controlFlowNestingLevel;  // this only needs to work right if no errors

    TIntermTyped* trueNode = nullptr;
    if (! acceptExpression(trueNode)) {
        expected("expression after ?");
        return false;
    }
    TSourceLoc loc = token.loc;

    if (! acceptTokenClass(EHTokColon)) {
        expected(":");
        return false;
    }

    TIntermTyped* falseNode = nullptr;
    if (! acceptAssignmentExpression(falseNode)) {
        expected("expression after :");
        return false;
    }

    --parseContext.controlFlowNestingLevel;

    node = intermediate.addSelection(node, trueNode, falseNode, loc);

    return true;
}

// Accept a binary expression, for binary operations that
// associate left-to-right.  This is, it is implicit, for example
//
//    ((a op b) op c) op d
//
// binary_expression
//      : expression op expression op expression ...
//
// where 'expression' is the next higher level in precedence.
//
bool HlslGrammar::acceptBinaryExpression(TIntermTyped*& node, PrecedenceLevel precedenceLevel)
{
    if (precedenceLevel > PlMul)
        return acceptUnaryExpression(node);

    // assignment_expression
    if (! acceptBinaryExpression(node, (PrecedenceLevel)(precedenceLevel + 1)))
        return false;

    do {
        TOperator op = HlslOpMap::binary(peek());
        PrecedenceLevel tokenLevel = HlslOpMap::precedenceLevel(op);
        if (tokenLevel < precedenceLevel)
            return true;

        // ... op
        TSourceLoc loc = token.loc;
        advanceToken();

        // ... expression
        TIntermTyped* rightNode = nullptr;
        if (! acceptBinaryExpression(rightNode, (PrecedenceLevel)(precedenceLevel + 1))) {

            if (this->xkslShaderParsingOperation == XkslShaderParsingOperationEnum::ParseXkslShaderNewTypesDefinition ||
                this->xkslShaderParsingOperation == XkslShaderParsingOperationEnum::ParseXkslShaderConstVariables ||
                this->xkslShaderParsingOperation == XkslShaderParsingOperationEnum::ParseXkslShaderMembersAndMethodsDeclarations ||
                this->xkslShaderParsingOperation == XkslShaderParsingOperationEnum::ParseXkslShaderConstStatements)
                return false; //return false but it's not necessary an error: the expression can be resolved later

            expected("expression");
            return false;
        }

        node = intermediate.addBinaryMath(op, node, rightNode, loc);
        if (node == nullptr) {
            parseContext.error(loc, "Could not perform requested binary operation", "", "");
            return false;
        }
    } while (true);
}

// unary_expression
//      : (type) unary_expression
//      | + unary_expression
//      | - unary_expression
//      | ! unary_expression
//      | ~ unary_expression
//      | ++ unary_expression
//      | -- unary_expression
//      | postfix_expression
//
bool HlslGrammar::acceptUnaryExpression(TIntermTyped*& node)
{
    // (type) unary_expression
    // Have to look two steps ahead, because this could be, e.g., a
    // postfix_expression instead, since that also starts with at "(".
    if (acceptTokenClass(EHTokLeftParen)) {
        TType castType;
        if (acceptType(castType)) {
            // recognize any array_specifier as part of the type
            TArraySizes* arraySizes = nullptr;
            acceptArraySpecifier(arraySizes);
            if (arraySizes != nullptr)
                castType.newArraySizes(*arraySizes);
            TSourceLoc loc = token.loc;
            if (acceptTokenClass(EHTokRightParen)) {
                // We've matched "(type)" now, get the expression to cast
                if (! acceptUnaryExpression(node))
                    return false;

                // Hook it up like a constructor
                TFunction* constructorFunction = parseContext.makeConstructorCall(loc, castType);
                if (constructorFunction == nullptr) {
                    expected("type that can be constructed");
                    return false;
                }
                TIntermTyped* arguments = nullptr;
                parseContext.handleFunctionArgument(constructorFunction, arguments, node);
                node = parseContext.handleFunctionCall(loc, constructorFunction, arguments);

                return node != nullptr;
            } else {
                // This could be a parenthesized constructor, ala (int(3)), and we just accepted
                // the '(int' part.  We must back up twice.
                recedeToken();
                recedeToken();

                // Note, there are no array constructors like
                //   (float[2](...))
                if (arraySizes != nullptr)
                    parseContext.error(loc, "parenthesized array constructor not allowed", "([]())", "", "");
            }
        } else {
            // This isn't a type cast, but it still started "(", so if it is a
            // unary expression, it can only be a postfix_expression, so try that.
            // Back it up first.
            recedeToken();
            return acceptPostfixExpression(node);
        }
    }

    // peek for "op unary_expression"
    TOperator unaryOp = HlslOpMap::preUnary(peek());

    // postfix_expression (if no unary operator)
    if (unaryOp == EOpNull)
        return acceptPostfixExpression(node);

    // op unary_expression
    TSourceLoc loc = token.loc;
    advanceToken();
    if (! acceptUnaryExpression(node))
        return false;

    // + is a no-op
    if (unaryOp == EOpAdd)
        return true;

    node = intermediate.addUnaryMath(unaryOp, node, loc);

    // These unary ops require lvalues
    if (unaryOp == EOpPreIncrement || unaryOp == EOpPreDecrement)
        node = parseContext.handleLvalue(loc, "unary operator", node);

    return node != nullptr;
}

XkslShaderDefinition* HlslGrammar::getShaderClassDefinition(const TString& shaderClassName)
{
    if (this->xkslShaderLibrary == nullptr)
    {
        error("shaderLibrary has not been set");
        return nullptr;
    }

    XkslShaderDefinition* shader = nullptr;

    //find the shader declaration
    int countShaders = (int)(this->xkslShaderLibrary->listShaders.size());
    for (int i = 0; i < countShaders; ++i)
    {
        if (this->xkslShaderLibrary->listShaders.at(i)->shaderFullName.compare(shaderClassName) == 0)
        {
            shader = this->xkslShaderLibrary->listShaders.at(i);
            break;
        }
    }

    return shader;
}

TType* HlslGrammar::getTypeDefinedByTheShaderOrItsParents(const TString& shaderName, const TString& typeName, int uniqueId)
{
    if (uniqueId <= 0)
    {
        //used to avoid infinite loop if there are cyclic dependencies among shaders
        dependencyUniqueCounter++;
        if (dependencyUniqueCounter <= 0) ResetShaderLibraryFlag();
        uniqueId = dependencyUniqueCounter;
    }

    XkslShaderDefinition* shader = getShaderClassDefinition(shaderName);
    if (shader == nullptr) {
        error("undeclared shader:" + shaderName);
        return nullptr;
    }
    if (shader->isValid == false) {
        error("invalid shader:" + shader->shaderFullName);
        return nullptr;
    }
    if (shader->tmpFlag == uniqueId) return false; //the shader was already investigated
    shader->tmpFlag = uniqueId;

    //look if the shader defined the type
    int countTypes = (int)(shader->listCustomTypes.size());
    for (int i = 0; i < countTypes; ++i)
    {
        if (shader->listCustomTypes[i].type->getUserIdentifierName()->compare(typeName) == 0)
        {
            return shader->listCustomTypes[i].type;
        }
    }

    //type not found: we look in the parent classes
    int countParents = (int)(shader->listParents.size());
    for (int p = 0; p < countParents; p++)
    {
        if (shader->listParents[p].parentShader == nullptr) {
            if (this->xkslShaderParsingOperation == XkslShaderParsingOperationEnum::ParseXkslShaderConstStatements)
                return nullptr; //not an error: the links between shaders are only done after this operation

            error("missing link to parent shader for:" + shaderName);
            return nullptr;
        }
        if (shader->listParents[p].parentShader->isValid == false) continue;

        TString& parentName = shader->listParents[p].parentShader->shaderFullName;
        TType* type = getTypeDefinedByTheShaderOrItsParents(parentName, typeName, uniqueId);
        if (type != nullptr) return type;
    }

    return nullptr;
}

bool HlslGrammar::getListShaderClassMethodsWithGivenName(XkslShaderDefinition* shader, const TString& methodName, TVector<const TShaderClassFunction*>& shaderMethodsList, bool onlyLookInParentClasses, bool recursivelyLookInParents)
{
    if (shader == nullptr || shader->isValid == false) {
        error("invalid or null shader");
        return false;
    }

    if (!onlyLookInParentClasses)
    {
        //look if the shader declared some method with 
        unsigned int countMethods = (unsigned int)(shader->listMethods.size());
        for (unsigned int i = 0; i < countMethods; ++i)
        {
            const TShaderClassFunction* aShaderMethod = &(shader->listMethods[i]);
            if (aShaderMethod->function->getName().compare(methodName) == 0)
            {
                shaderMethodsList.push_back(aShaderMethod);
            }
        }
    }

    //look within the class parents
    if (recursivelyLookInParents)
    {
        //method not found: we look in the parent classes
        unsigned int countParents = (unsigned int)(shader->listParents.size());
        for (unsigned int p = 0; p < countParents; p++)
        {
            XkslShaderDefinition* parentShader = shader->listParents[p].parentShader;
            if (parentShader == nullptr) {
                if (this->xkslShaderParsingOperation == XkslShaderParsingOperationEnum::ParseXkslShaderConstStatements)
                    return false; //not an error: the links between shaders are only done after this operation

                error("missing link to parent shader for:" + shader->shaderBaseName);
                return false;
            }
            if (parentShader->isValid == false) continue;

            if ( !getListShaderClassMethodsWithGivenName(parentShader, methodName, shaderMethodsList, false, recursivelyLookInParents))
                return false;
        }
    }

    return true;
}

//Look for a method best-matching the function call (mangled name is not identical but we check if we can convert / cast some parameters)
XkslShaderDefinition::ShaderIdentifierLocation HlslGrammar::findShaderClassBestMatchingMethod(const TString& shaderClassName, TFunction* functionCall, bool onlyLookInParentClasses)
{
    XkslShaderDefinition::ShaderIdentifierLocation identifierLocation;

    XkslShaderDefinition* shader = getShaderClassDefinition(shaderClassName);
    if (shader == nullptr) {
        error(TString("undeclared class:") + shaderClassName);
        return identifierLocation;
    }
    if (shader->isValid == false) {
        error("invalid shader:" + shader->shaderFullName);
        return identifierLocation;
    }

    //Get the list of all potential candidates
    const TString& methodName = functionCall->getName();
    TVector<const TShaderClassFunction*> shaderMethodsList;
    bool recursivelyLookInParents = false; //if we take all methods from current shader + its parents: it can lead to ambigeous choices if some signature are similar
    if (!getListShaderClassMethodsWithGivenName(shader, methodName, shaderMethodsList, onlyLookInParentClasses, recursivelyLookInParents))
    {
        error("Failed to build the list of all candidate method for the given name: " + methodName);
        return identifierLocation;
    }

    unsigned int countCandidates = (unsigned int)(shaderMethodsList.size());
    if (countCandidates == 0) return identifierLocation;
    TVector<const TFunction*> candidateList;
    for (unsigned int k = 0; k < countCandidates; k++) candidateList.push_back(shaderMethodsList[k]->function);

    //Find the best match
    TIntermTyped* args = nullptr;
    const TFunction* bestMatch = parseContext.findBestMatchingFunctionFromCandidateList(token.loc, *functionCall, candidateList, false, args, false);

    if (bestMatch != nullptr)
    {
        //Got a matching function, find back the shader owning the function
        const TShaderClassFunction* shaderClassFunction = nullptr;
        for (unsigned int k = 0; k < countCandidates; k++)
        {
            if (shaderMethodsList[k]->function == bestMatch)
            {
                shaderClassFunction = shaderMethodsList[k];
                break;
            }
        }
        if (shaderClassFunction == nullptr) {
            error("Failed to find back the shader class method");
            return identifierLocation;
        }
        
        identifierLocation.SetMethodLocation(shaderClassFunction->shader, shaderClassFunction->function);
        return identifierLocation;
    }

    //Look into the parents
    {
        //method not found: we look in the parent classes
        unsigned int countParents = (unsigned int)(shader->listParents.size());
        for (unsigned int p = 0; p < countParents; p++)
        {
            XkslShaderDefinition* parentShader = shader->listParents[p].parentShader;
            if (parentShader == nullptr) {
                if (this->xkslShaderParsingOperation == XkslShaderParsingOperationEnum::ParseXkslShaderConstStatements)
                    return identifierLocation; //not an error: the links between shaders are only done after this operation

                error("missing link to parent shader for:" + shaderClassName);
                return identifierLocation;
            }
            if (parentShader->isValid == false) continue;

            const TString& parentName = parentShader->shaderFullName;
            identifierLocation = findShaderClassBestMatchingMethod(parentName, functionCall, false);
            if (identifierLocation.isMethod()) return identifierLocation;
        }
    }

    return identifierLocation;
}

XkslShaderDefinition::ShaderIdentifierLocation HlslGrammar::findShaderClassMethod(const TString& shaderClassName, const TString& methodName, bool onlyLookInParentClasses)
{
    XkslShaderDefinition::ShaderIdentifierLocation identifierLocation;

    XkslShaderDefinition* shader = getShaderClassDefinition(shaderClassName);
    if (shader == nullptr) {
        error(TString("undeclared class:") + shaderClassName);
        return identifierLocation;
    }
    if (shader->isValid == false) {
        error("invalid shader:" + shader->shaderFullName);
        return identifierLocation;
    }

    if (!onlyLookInParentClasses)
    {
        //look if the shader did declare the method
        unsigned int countMethods = (unsigned int)(shader->listMethods.size());
        for (unsigned int i = 0; i < countMethods; ++i)
        {
            if (shader->listMethods[i].function->getDeclaredMangledName().compare(methodName) == 0)
            {
                identifierLocation.SetMethodLocation(shader, shader->listMethods[i].function);
                break;
            }
        }
    }

    if (identifierLocation.isUnknown())
    {
        //method not found: we look in the parent classes
        unsigned int countParents = (unsigned int)(shader->listParents.size());
        for (unsigned int p = 0; p < countParents; p++)
        {
            if (shader->listParents[p].parentShader == nullptr) {
                if (this->xkslShaderParsingOperation == XkslShaderParsingOperationEnum::ParseXkslShaderConstStatements)
                    return identifierLocation; //not an error: the links between shaders are only done after this operation

                error("missing link to parent shader for:" + shaderClassName);
                return identifierLocation;
            }
            if (shader->listParents[p].parentShader->isValid == false) continue;
            
            TString& parentName = shader->listParents[p].parentShader->shaderFullName;
            identifierLocation = findShaderClassMethod(parentName, methodName, false);
            if (identifierLocation.isMethod()) return identifierLocation;
        }
    }

    return identifierLocation;
}

bool HlslGrammar::isIdentifierRecordedAsACompositionVariableName(TString* accessorClassName, const TString& identifierName, bool lookForArraycomposition, TShaderCompositionVariable& compositionTargeted, int uniqueId)
{
    if (uniqueId <= 0)
    {
        //used to avoid infinite loop if there are cyclic dependencies among shaders
        dependencyUniqueCounter++;
        if (dependencyUniqueCounter <= 0) ResetShaderLibraryFlag();
        uniqueId = dependencyUniqueCounter;
    }

    TString* className = accessorClassName == nullptr? getCurrentShaderName() : accessorClassName;
    if (className == nullptr) return false;

    //we first look if a foreach loop declare a composition variable
    int countForEachLoopVariables = (int)(this->listForeachArrayCompositionVariable.size());
    for (int i = countForEachLoopVariables - 1; i >= 0; i--)
    {
        const TShaderVariableTargetingACompositionVariable& variableTargetingACompositionVariable = this->listForeachArrayCompositionVariable[i];
        if (identifierName == variableTargetingACompositionVariable.variableName)
        {
            compositionTargeted = variableTargetingACompositionVariable.compositionTargeted;
            return true;
        }
    }

    XkslShaderDefinition* shader = getShaderClassDefinition(*className);
    if (shader == nullptr) {
        if (this->xkslShaderParsingOperation == XkslShaderParsingOperationEnum::ParseXkslShaderNewTypesDefinition ||
            this->xkslShaderParsingOperation == XkslShaderParsingOperationEnum::ParseXkslShaderConstVariables ||
            this->xkslShaderParsingOperation == XkslShaderParsingOperationEnum::ParseXkslShaderMembersAndMethodsDeclarations ||
            this->xkslShaderParsingOperation == XkslShaderParsingOperationEnum::ParseXkslShaderConstStatements)
        {
            //return false but it's not necessary an error: the expression can be resolved later
            return false;
        }

        error("undeclared shader:" + (*className));
        return false;
    }
    if (shader->isValid == false) {
        error("invalid shader:" + shader->shaderFullName);
        return false;
    }
    if (shader->tmpFlag == uniqueId) return false; //the shader was already investigated
    shader->tmpFlag = uniqueId;

    //look if the shader declare the composition variable
    int countCompositions = (int)(shader->listCompositions.size());
    for (int i = 0; i < countCompositions; ++i)
    {
        if (shader->listCompositions[i].isArray == lookForArraycomposition)
        {
            if (shader->listCompositions[i].variableName.compare(identifierName) == 0)
            {
                //compositionShaderIdTargeted = shader->listCompositions[i].shaderCompositionId;
                //compositionShaderClassOwner = shader->shaderName;
                compositionTargeted = shader->listCompositions[i];
                return true;
            }
        }
    }

    //look for the composition name is declared in the shader parents
    int countParents = (int)(shader->listParents.size());
    for (int p = 0; p < countParents; p++)
    {
        if (shader->listParents[p].parentShader == nullptr) {
            if (this->xkslShaderParsingOperation == XkslShaderParsingOperationEnum::ParseXkslShaderConstStatements)
                return false; //not an error: the links between shaders are only done after this operation

            error("missing link to parent shader for:" + shader->shaderFullName);
            return false;
        }
        if (shader->listParents[p].parentShader->isValid == false) continue;

        TString& parentName = shader->listParents[p].parentShader->shaderFullName;
        if (isIdentifierRecordedAsACompositionVariableName(&parentName, identifierName, lookForArraycomposition, compositionTargeted, uniqueId)) return true;
    }

    return false;
}

bool HlslGrammar::IsShaderEqualOrSubClassOf(XkslShaderDefinition* shader, XkslShaderDefinition* maybeParent)
{
    if (shader == nullptr || maybeParent == nullptr) {
        error("null parameters");
        return false;
    }
    if (shader->isValid == false) {
        error("invalid shader:" + shader->shaderFullName);
        return false;
    }

    if (shader == maybeParent) return true;

    int countParents = (int)(shader->listParents.size());
    for (int p = 0; p < countParents; p++)
    {
        if (shader->listParents[p].parentShader == nullptr) {
            if (this->xkslShaderParsingOperation == XkslShaderParsingOperationEnum::ParseXkslShaderConstStatements)
                return false; //not an error: the links between shaders are only done after this operation

            error("missing link to parent shader for:" + shader->shaderFullName);
            return false;
        }
        if (shader->listParents[p].parentShader->isValid == false) continue;

        if (IsShaderEqualOrSubClassOf(shader->listParents[p].parentShader, maybeParent)) return true;
    }

    return false;
}

void HlslGrammar::ResetShaderLibraryFlag()
{
    dependencyUniqueCounter = 1;
    if (xkslShaderLibrary == nullptr) return;
    for (unsigned int s = 0; s < xkslShaderLibrary->listShaders.size(); s++)
        xkslShaderLibrary->listShaders[s]->tmpFlag = 0;
}

XkslShaderDefinition::ShaderIdentifierLocation HlslGrammar::findShaderClassMember(const TString& shaderClassName, bool hasStreamAccessor, const TString& memberName, bool onlyLookInParentClasses, int uniqueId)
{
    if (uniqueId <= 0)
    {
        //used to avoid infinite loop if there are cyclic dependencies among shaders
        dependencyUniqueCounter++;
        if (dependencyUniqueCounter <= 0) ResetShaderLibraryFlag();
        uniqueId = dependencyUniqueCounter;
    }

    XkslShaderDefinition::ShaderIdentifierLocation identifierLocation;

    XkslShaderDefinition* shader = getShaderClassDefinition(shaderClassName);
    if (shader == nullptr){
        error(TString("undeclared class:") + shaderClassName);
        return identifierLocation;
    }
    if (shader->isValid == false) {
        error("invalid shader:" + shader->shaderFullName);
        return identifierLocation;
    }
    if (shader->tmpFlag == uniqueId) return identifierLocation; //the shader was already investigated
    shader->tmpFlag = uniqueId;

    if (!onlyLookInParentClasses)
    {
        //look if the shader did declare the identifier
        int countMembers = (int)(shader->listAllDeclaredMembers.size());
        for (int i = 0; i < countMembers; ++i)
        {
            if (shader->listAllDeclaredMembers[i].type->getUserIdentifierName()->compare(memberName) == 0)
            {
                //to avoid name conflict, in the case of a shader declare a stream and a non-stream variables using the same name
                if (hasStreamAccessor)
                {
                    if (shader->listAllDeclaredMembers[i].memberLocation.memberLocationType != XkslShaderDefinition::MemberLocationTypeEnum::StreamBuffer) continue;
                }
                else
                {
                    if (shader->listAllDeclaredMembers[i].memberLocation.memberLocationType == XkslShaderDefinition::MemberLocationTypeEnum::StreamBuffer) continue;
                }

                identifierLocation = shader->listAllDeclaredMembers[i].memberLocation;
                break;
            }
        }
    }

    if (identifierLocation.isUnknown())
    {
        if (this->shaderWhereMembersCanBeFound != nullptr)
        {
            if (this->shaderWhereMembersCanBeFound->tmpFlag != uniqueId)
            {
                identifierLocation = findShaderClassMember(this->shaderWhereMembersCanBeFound->shaderFullName, hasStreamAccessor, memberName, false, uniqueId);
                if (identifierLocation.isMember()) return identifierLocation;
            }
        }
    }

    if (identifierLocation.isUnknown())
    {
        //member not found: we look in the parent classes
        int countParents = (int)(shader->listParents.size());
        for (int p = 0; p < countParents; p++)
        {
            if (shader->listParents[p].parentShader == nullptr) {
                if (this->xkslShaderParsingOperation == XkslShaderParsingOperationEnum::ParseXkslShaderConstStatements)
                    return identifierLocation; //not an error: the links between shaders are only done after this operation

                error("missing link to parent shader for:" + shader->shaderFullName);
                return identifierLocation;
            }
            if (shader->listParents[p].parentShader->isValid == false) continue;

            TString& parentName = shader->listParents[p].parentShader->shaderFullName;
            identifierLocation = findShaderClassMember(parentName, hasStreamAccessor, memberName, false, uniqueId);
            if (identifierLocation.isMember()) return identifierLocation;
        }
    }

    return identifierLocation;
}

TFunction* HlslGrammar::getFunctionCurrentlyParsed()
{
    return functionCurrentlyParsed;
}

XkslShaderDefinition* HlslGrammar::getShaderCurrentlyParsed()
{
    return xkslShaderCurrentlyParsed;
}

TString* HlslGrammar::getCurrentShaderName()
{
    if (xkslShaderCurrentlyParsed == nullptr) return nullptr;
    return &(xkslShaderCurrentlyParsed->shaderFullName);
}

int HlslGrammar::getCurrentShaderCountParents()
{
    if (xkslShaderCurrentlyParsed == nullptr) return 0;
    return (int)(xkslShaderCurrentlyParsed->listParents.size());
}

TString* HlslGrammar::getCurrentShaderParentName(int index)
{
    if (xkslShaderCurrentlyParsed == nullptr) return nullptr;
    assert(index >= 0 && index < (int)xkslShaderCurrentlyParsed->listParents.size());

    if (xkslShaderCurrentlyParsed->listParents[index].parentShader == nullptr) {
        if (this->xkslShaderParsingOperation == XkslShaderParsingOperationEnum::ParseXkslShaderConstStatements)
            return nullptr; //not an error: the links between shaders are only done after this operation

        error("missing link to parent shader for:" + xkslShaderCurrentlyParsed->shaderFullName);
        return nullptr;
    }

    return &(xkslShaderCurrentlyParsed->listParents[index].parentShader->shaderFullName);
}

bool HlslGrammar::isRecordedAsAShaderBaseName(const TString& baseName)
{
    int countShaders = (int)(this->xkslShaderLibrary->listShaders.size());
    for (int i = 0; i < countShaders; ++i)
    {
        if (this->xkslShaderLibrary->listShaders[i]->shaderBaseName.compare(baseName) == 0) return true;
    }
    return false;
}

bool HlslGrammar::isRecordedAsAShaderName(const TString& name)
{
    int countShaders = (int)(this->xkslShaderLibrary->listShaders.size());
    for (int i = 0; i < countShaders; ++i)
    {
        if (this->xkslShaderLibrary->listShaders[i]->shaderFullName.compare(name) == 0) return true;
    }
    return false;
}

bool HlslGrammar::acceptPostfixExpression(TIntermTyped*& node)
{
    TShaderCompositionVariable composition;
    composition.shaderCompositionId = -1;
    bool isStreamsUsedAsAType = false;
    return acceptPostfixExpression(node, false, false, false, nullptr, composition, isStreamsUsedAsAType);
}

int HlslGrammar::uniqueIndex = 0;
int HlslGrammar::GetUniqueIndex()
{
    return uniqueIndex++;
}

// postfix_expression
//      : LEFT_PAREN expression RIGHT_PAREN
//      | literal
//      | constructor
//      | IDENTIFIER [ COLONCOLON IDENTIFIER [ COLONCOLON IDENTIFIER ... ] ]
//      | function_call
//      | postfix_expression LEFT_BRACKET integer_expression RIGHT_BRACKET
//      | postfix_expression DOT IDENTIFIER
//      | postfix_expression DOT IDENTIFIER arguments
//      | postfix_expression arguments
//      | postfix_expression INC_OP
//      | postfix_expression DEC_OP
//
bool HlslGrammar::acceptPostfixExpression(TIntermTyped*& node, bool hasBaseAccessor, bool callThroughStaticShaderClassName, bool hasStreamAccessor, TString* classAccessorName,
    TShaderCompositionVariable& compositionTargeted, bool& isStreamsUsedAsAType)
{
    // Not implemented as self-recursive:
    // The logical "right recursion" is done with a loop at the end

    // idToken will pick up either a variable or a function name in a function call
    HlslToken idToken;

    bool hasComposition = compositionTargeted.shaderCompositionId >= 0? true: false;
    int countAccessor = 0;
    if (hasBaseAccessor) countAccessor++;
    if (callThroughStaticShaderClassName) countAccessor++;
    if (countAccessor > 1) {
        error("Found too many class accessors");
        return false;
    }

    // Find something before the postfix operations, as they can't operate
    // on nothing.  So, no "return true", they fall through, only "return false".
    if (acceptTokenClass(EHTokLeftParen)) {
        // LEFT_PAREN expression RIGHT_PAREN
        if (! acceptExpression(node)) {
            expected("expression");
            return false;
        }
        if (! acceptTokenClass(EHTokRightParen)) {
            expected(")");
            return false;
        }
    } else if (acceptLiteral(node)) {
        // literal (nothing else to do yet)
    } else if (acceptConstructor(node)) {
        // constructor (nothing else to do yet)
    }
    else if (acceptClassReferenceAccessor(classAccessorName, hasBaseAccessor, callThroughStaticShaderClassName, hasStreamAccessor, compositionTargeted, isStreamsUsedAsAType))
    {
        return acceptPostfixExpression(node, hasBaseAccessor, callThroughStaticShaderClassName, hasStreamAccessor, classAccessorName, compositionTargeted, isStreamsUsedAsAType);
    }
    else if (isStreamsUsedAsAType || acceptIdentifier(idToken))
    {
        TString* memberFullName = nullptr;
        TString* memberName = nullptr;
        if (isStreamsUsedAsAType)
        {
            if (!peekTokenClass(EHTokStreams)) {
                expected("streams keyword"); return false;
            }
            advanceToken();

            //We create the corresponding Streams type
        }
        else
        {
            memberFullName = idToken.string;
            if (memberFullName == nullptr) {
                error("member name is missing"); return false;
            }

            //The identifier name can be composed
            while (acceptTokenClass(EHTokColonColon)) {
                // user-type or namespace name
                memberFullName = NewPoolTString(memberFullName->c_str());
                memberFullName->append(parseContext.scopeMangler);
                if (acceptIdentifier(idToken))
                    memberFullName->append(*idToken.string);
                else {
                    expected("identifier after ::");
                    return false;
                }
            }

            memberName = idToken.string;
        }

        if (! peekTokenClass(EHTokLeftParen))
        {
            //we're parsing a variable
            TString* referenceShaderName = getCurrentShaderName();
            if (referenceShaderName == nullptr)
            {
                //we're not parsing a shader, normal hlsl procedure
                node = parseContext.handleVariable(idToken.loc, memberFullName);
            }
            else
            {
                //look if the symbol already exists in our symbol table
                TSymbol* symbol = nullptr;
                if (!isStreamsUsedAsAType && classAccessorName == nullptr && !hasStreamAccessor && !hasComposition && !hasBaseAccessor && !callThroughStaticShaderClassName)
                {
                    if (memberName != nullptr)
                    {
                        symbol = parseContext.lookIfSymbolExistInSymbolTable(memberName);
                    }
                }
                
                if (isStreamsUsedAsAType)
                {
                    if (hasBaseAccessor) {
                        error("base.streams is not permitted");
                        return false;
                    }

                    //We create a temporary Streams type and will have the "streams" keyword refers to it
                    TString* temporaryStreamVariableName = NewPoolTString( (TString("_tmpStreamsVar_") + std::to_string(GetUniqueIndex()).c_str()).c_str() );
                    symbol = parseContext.lookIfSymbolExistInSymbolTable(temporaryStreamVariableName);
                    if (symbol != nullptr) {
                        error("The temporary variable already exists in the symbol table");
                        return false;
                    }

                    TString accessorClassName = classAccessorName == nullptr ? *referenceShaderName : *classAccessorName;

                    //Create and setup the "Streams" type and variable
                    TType* temporaryStreamType = new TType(EbtStreams);
                    temporaryStreamType->getQualifier().storage = EvqTemporary;
                    temporaryStreamType->SetStreamsTypeProperties( new TStreamsTypeProperties(accessorClassName, true) );

                    parseContext.declareVariable(idToken.loc, *temporaryStreamVariableName, *temporaryStreamType, nullptr);

                    memberName = temporaryStreamVariableName;
                }

                if (!isStreamsUsedAsAType && (symbol == nullptr || classAccessorName != nullptr || hasStreamAccessor || hasComposition || hasBaseAccessor || callThroughStaticShaderClassName))
                {
                    if (hasComposition) {
                        //maybe we can authorize this later?
                        error("a function call is expected after refering a composition");
                        return false;
                    }

                    TString accessorClassName = classAccessorName == nullptr ? *referenceShaderName : *classAccessorName;

                    if (this->xkslShaderParsingOperation == XkslShaderParsingOperationEnum::ParseXkslShaderConstVariables
                        || this->xkslShaderParsingOperation == XkslShaderParsingOperationEnum::ParseXkslShaderMembersAndMethodsDeclarations)
                    {
                        //at these stages we're only parsing members and methods declaration
                        //If we meet an unknown expression while parsing members / methods declaration, then we're likely pasring a const type. example: static const int vb = va * 2;
                        //We skip the expression in this case and keep it for a later resolution
                        if (this->functionCurrentlyParsed == nullptr && this->shaderMethodOrMemberTypeCurrentlyParsed != nullptr)
                        {
                            if (this->shaderMethodOrMemberTypeCurrentlyParsed->getQualifier().storage == EvqConst)
                                return false;
                        }
                    }

                    {
                        //XKSL extensions: a shader can override the memberName with a "MemberName" generic
                        if (this->xkslShaderCurrentlyParsed != nullptr)
                        {
                            if (memberName != nullptr)
                            {
                                unsigned int countGenerics = (unsigned int)(this->xkslShaderCurrentlyParsed->listGenerics.size());
                                for (unsigned int c = 0; c < countGenerics; c++)
                                {
                                    const ShaderGenericAttribute& aGeneric = this->xkslShaderCurrentlyParsed->listGenerics[c];
                                    if (aGeneric.type->getBasicType() == EbtMemberNameType)
                                    {
                                        const TString& genericName = *(aGeneric.type->getUserIdentifierName());
                                        if (genericName == *memberName)
                                        {
                                            memberName = NewPoolTString( aGeneric.expressionConstValue.c_str() );
                                            break;
                                        }
                                    }
                                }
                            }
                        }

                        //we look if the identifier is a shader's member
                        XkslShaderDefinition::ShaderIdentifierLocation identifierLocation = findShaderClassMember(accessorClassName, hasStreamAccessor, *memberName, hasBaseAccessor);

                        if (!identifierLocation.isMember())
                        {
                            bool throwAnError = MustThrowAnErrorWhenParsingUnidentifiedSymbol();
                            if (!throwAnError){
                                if (classAccessorName != nullptr || hasStreamAccessor || hasComposition){
                                    throwAnError = true;
                                }
                            }

                            if (throwAnError)
                            {
                                error( (TString("Member: \"") + (*memberName) + TString("\" not found in the class (or its parents): \"") + accessorClassName + TString("\"")).c_str() );
                            }
                            else
                            {
                                //unknown identifier, but in some cases we can resolve it (for example if the identifier is an unknown class and we have the possibility to recursively query them)
                                if (!hasAnyErrorToBeProcessedAtTheTop())
                                {
                                    setUnknownIdentifierToProcessAtTheTop(NewPoolTString(memberName->c_str()));
                                }
                            }
                            return false;
                        }

                        if (identifierLocation.symbolName == nullptr)
                        {
                            error("identifierLocation.symbolName undefined");
                            return false;
                        }

                        //============================================================================
                        //XKSL RULES: check the validity of accessing the variable
                        TFunction* currentFunctionBeingParsed = getFunctionCurrentlyParsed();
                        XkslShaderDefinition* shaderBeingParsed = getShaderCurrentlyParsed();
                        if (shaderBeingParsed == nullptr) {
                            if (shaderBeingParsed == nullptr) error(TString("We should be parsing a shader"));
                            return false;
                        }

                        //currentFunctionBeingParsed can be null, when parsing a const assignment expression for example
                        if (currentFunctionBeingParsed != nullptr)
                        {
                            if (currentFunctionBeingParsed->getType().getQualifier().isStatic)
                            {
                                //a static function can only access a const variables
                                if (identifierLocation.memberLocationType != XkslShaderDefinition::MemberLocationTypeEnum::Const)
                                {
                                    error(TString("A static method: " + currentFunctionBeingParsed->getName() + TString(" is accessing a non-const variable: ") + (*identifierLocation.memberName)));
                                    return false;
                                }
                            }
                            else
                            {
                                //Rules obsolete: ComputeColorWave shader accesses a cbuffer variable from Global shader (Global.Time)

                                /////a non-static function can only access a variables if the classes are related, or if the variables is a const
                                ///if (identifierLocation.memberLocationType != XkslShaderDefinition::MemberLocationTypeEnum::Const)
                                ///{
                                ///    XkslShaderDefinition* shaderOwningTheVariable = identifierLocation.shader;
                                ///    if (shaderOwningTheVariable == nullptr) {
                                ///        error(TString("no shader found for the variable: ") + (*identifierLocation.memberName));
                                ///        return false;
                                ///    }
                                ///
                                ///    if (!IsShaderEqualOrSubClassOf(shaderBeingParsed, shaderOwningTheVariable)) {
                                ///        error(TString("Shader: ") + shaderBeingParsed->shaderFullName + TString(" cannot access the variable ")
                                ///            + shaderOwningTheVariable->shaderFullName + TString(".") + (*identifierLocation.memberName) );
                                ///        return false;
                                ///    }
                                ///}

                            }
                        }
                        //============================================================================

                        TString* blockSymbolName = nullptr;
                        TString* variableSymbolName = nullptr;
                        switch (identifierLocation.memberLocationType)
                        {
                            case XkslShaderDefinition::MemberLocationTypeEnum::StreamBuffer:
                                if (!hasStreamAccessor)
                                {
                                    //to access a stream variable, we need to use the "streams" keyword accessor
                                    error("The \"streams\" keyword is required to access a stream variable");
                                    return false;
                                }
                                blockSymbolName = identifierLocation.symbolName;
                                break;

                            case XkslShaderDefinition::MemberLocationTypeEnum::CBuffer:
                                blockSymbolName = identifierLocation.symbolName;
                                break;

                            case XkslShaderDefinition::MemberLocationTypeEnum::Const:
                                variableSymbolName = identifierLocation.symbolName;
                                break;

                            case XkslShaderDefinition::MemberLocationTypeEnum::UnresolvedConst:
                                if (this->xkslShaderParsingOperation == XkslShaderParsingOperationEnum::ParseXkslShaderConstStatements)
                                    return false; //return false but it's not necessary an error: the const can be resolved later
                                else
                                {
                                    error("The const member is unresolved");
                                    return false;
                                }
                                break;

                            default:
                                error("Unknown member location type");
                                return false;
                        }

                        if (blockSymbolName != nullptr)
                        {
                            //Add accessor to the member struct
                            glslang::TSymbol* blockSymbol = parseContext.symbolTable.find(*blockSymbolName);
                            if (blockSymbol == nullptr || blockSymbol->getAsVariable() == nullptr) {
                                error((TString("Cannot find valid struct symbol for Member: \"") + *memberName + TString("\" in shader (or its parents): \"") + accessorClassName + TString("\"")).c_str());
                                return false;
                            }

                            //handle access to the struct
                            //node = parseContext.handleVariable(idToken.loc, blockSymbol, identifierLocation.symbolName);

                            //handle access to the struct member
                            // handleDotDereference will not be able to find the correct member index for stream variable (fieldName != declarationName)
                            //node = parseContext.handleDotDereference(idToken.loc, node, *memberName);

                            {
                                //Add access to the variable, by first accessing to the struct (stream) or block (cbuffer)
                                TVariable* blockVariable = blockSymbol->getAsVariable();
                                TIntermTyped* container = intermediate.addSymbol(*blockVariable, idToken.loc);

                                int member = identifierLocation.memberIndex;
                                TIntermTyped* index = intermediate.addConstantUnion(member, idToken.loc);
                                node = intermediate.addIndex(EOpIndexDirectStruct, container, index, idToken.loc);
                                node->setType(*(*blockVariable->getType().getStruct())[member].type);
                            }
                        }
                        else if (variableSymbolName != nullptr)
                        {
                            glslang::TSymbol* variableSymbol = parseContext.symbolTable.find(*variableSymbolName);
                            if (variableSymbol == nullptr) {
                                error((TString("Cannot find variable symbol: \"") + *memberName + TString("\" in shader (or its parents): \"") + accessorClassName + TString("\"")).c_str());
                                return false;
                            }

                            node = parseContext.handleVariable(idToken.loc, identifierLocation.symbolName);
                        }
                        else {
                            error("invalid class identifier");
                            return false;
                        }
                    }
                }
                else
                {
                    node = parseContext.handleVariable(idToken.loc, memberName);
                }
            }
        }
        else 
        {
            //we're parsing a method
            if (isStreamsUsedAsAType) {
                error("We cannot have a funtion call after streams identifier"); return false;
            }

            if (memberFullName == nullptr || memberName == nullptr) {
                error("Function identifier name is missing"); return false;
            }

            TString* referenceShaderName = getCurrentShaderName();
            if (referenceShaderName == nullptr)
            {
                //we're not parsing a shader: normal hlsl procedure
                if (acceptFunctionCall(idToken.loc, *memberFullName, node, nullptr)) {
                    // function_call (nothing else to do yet)
                }
                else {
                    expected("function call arguments");
                    return false;
                }
            }
            else
            {
                TSymbol* symbol = nullptr;
                if (classAccessorName == nullptr && !hasStreamAccessor && !hasComposition && !hasBaseAccessor && !callThroughStaticShaderClassName)
                {
                    symbol = parseContext.lookIfSymbolExistInSymbolTable(token.string);
                }

                if (symbol == nullptr || classAccessorName != nullptr || hasStreamAccessor || hasComposition || hasBaseAccessor || callThroughStaticShaderClassName)
                {
                    //No known symbol or a class accessor: we look for the appropriate method in our xksl shader library

                    if (hasStreamAccessor)
                    {
                        error("streams accessor cannot be used in front of a method call");
                        return false;
                    }

                    TString accessorClassName = classAccessorName == nullptr ? *referenceShaderName : *classAccessorName;
                    TShaderCompositionVariable* pCompositionTargeted = hasComposition? &compositionTargeted: nullptr;
                    if (acceptXkslFunctionCall(accessorClassName, hasBaseAccessor, callThroughStaticShaderClassName, pCompositionTargeted, idToken, node, nullptr)) {
                        // function_call (nothing else to do yet)
                    }
                    else {
                        expected("Xksl function call arguments");
                        return false;
                    }
                }
                else
                {
                    //the symbol is known and there is no class accessor, this is a call to a normal function (not belonging to a class)
                    if (acceptFunctionCall(idToken.loc, *memberFullName, node, nullptr)) {
                        // function_call (nothing else to do yet)
                    }
                    else {
                        expected("function call arguments");
                        return false;
                    }
                }
            }
        }
    } else {
        // nothing found, can't post operate
        return false;
    }

    // Something was found, chain as many postfix operations as exist.
    do {
        TSourceLoc loc = token.loc;
        TOperator postOp = HlslOpMap::postUnary(peek());

        // Consume only a valid post-unary operator, otherwise we are done.
        switch (postOp) {
        case EOpIndexDirectStruct:
        case EOpIndexIndirect:
        case EOpPostIncrement:
        case EOpPostDecrement:
        case EOpScoping:
            advanceToken();
            break;
        default:
            return true;
        }

        // We have a valid post-unary operator, process it.
        switch (postOp) {
        case EOpScoping:
        case EOpIndexDirectStruct:
        {
            // DOT IDENTIFIER
            // includes swizzles, member variables, and member functions
            HlslToken field;
            if (! acceptIdentifier(field)) {
                expected("swizzle or member");
                return false;
            }

            if (peekTokenClass(EHTokLeftParen)) {
                // member function
                TIntermTyped* thisNode = node;

                // arguments
                if (! acceptFunctionCall(field.loc, *field.string, node, thisNode)) {
                    expected("function parameters");
                    return false;
                }
            }
            else
            {
                TString dotFieldName = *field.string;

                //XKSL extensiosn: a shader can have MemberName generics overriding the `field` name
                if (this->xkslShaderCurrentlyParsed != nullptr)
                {
                    unsigned int countGenerics = (unsigned int)(this->xkslShaderCurrentlyParsed->listGenerics.size());
                    for (unsigned int c = 0; c < countGenerics; c++)
                    {
                        const ShaderGenericAttribute& aGeneric = this->xkslShaderCurrentlyParsed->listGenerics[c];
                        if (aGeneric.type->getBasicType() == EbtMemberNameType)
                        {
                            const TString& genericName = *(aGeneric.type->getUserIdentifierName());
                            if (genericName == dotFieldName)
                            {
                                dotFieldName = aGeneric.expressionConstValue;
                                break;
                            }
                        }
                    }
                }

                node = parseContext.handleDotDereference(field.loc, node, dotFieldName);
            }

            break;
        }
        case EOpIndexIndirect:
        {
            // LEFT_BRACKET integer_expression RIGHT_BRACKET
            TIntermTyped* indexNode = nullptr;
            if (! acceptExpression(indexNode) ||
                ! peekTokenClass(EHTokRightBracket)) {
                expected("expression followed by ']'");
                return false;
            }
            advanceToken();
            node = parseContext.handleBracketDereference(indexNode->getLoc(), node, indexNode);
            if (node == nullptr)
                return false;
            break;
        }
        case EOpPostIncrement:
            // INC_OP
            // fall through
        case EOpPostDecrement:
            // DEC_OP
            node = intermediate.addUnaryMath(postOp, node, loc);
            node = parseContext.handleLvalue(loc, "unary operator", node);
            break;
        default:
            assert(0);
            break;
        }
    } while (true);
}

// constructor
//      : type argument_list
//
bool HlslGrammar::acceptConstructor(TIntermTyped*& node)
{
    // type
    TType type;
    if (acceptType(type)) {
        TFunction* constructorFunction = parseContext.makeConstructorCall(token.loc, type);
        if (constructorFunction == nullptr)
            return false;

        // arguments
        TIntermTyped* arguments = nullptr;
        if (! acceptArguments(constructorFunction, arguments)) {
        
            if (this->xkslShaderParsingOperation == XkslShaderParsingOperationEnum::ParseXkslShaderNewTypesDefinition ||
                this->xkslShaderParsingOperation == XkslShaderParsingOperationEnum::ParseXkslShaderConstVariables ||
                this->xkslShaderParsingOperation == XkslShaderParsingOperationEnum::ParseXkslShaderMembersAndMethodsDeclarations ||
                this->xkslShaderParsingOperation == XkslShaderParsingOperationEnum::ParseXkslShaderConstStatements)
                return false; //return false but it's not necessary an error: the expression can be resolved later

            // It's possible this is a type keyword used as an identifier.  Put the token back
            // for later use.
            recedeToken();
            return false;
        }

        // hook it up
        node = parseContext.handleFunctionCall(arguments->getLoc(), constructorFunction, arguments);

        return node != nullptr;
    }

    return false;
}

bool HlslGrammar::acceptXkslShaderComposition(TShaderCompositionVariable& composition)
{
    HlslToken idToken = token;
    if (!acceptIdentifier(idToken))
    {
        error("Cannot parse composition: shaderName expected");
        return false;
    }
    composition.shaderTypeName = *idToken.string;

    if (!acceptIdentifier(idToken))
    {
        error("Cannot parse composition: variableName expected");
        return false;
    }
    composition.variableName = *idToken.string;

    if (acceptTokenClass(EHTokLeftBracket))
    {
        if (!acceptTokenClass(EHTokRightBracket)) {
            error("cannot parse composition: expected ']'");
            return false;
        }
        composition.isArray = true;
    }
    else composition.isArray = false;

    return true;
}

bool HlslGrammar::acceptXkslFunctionCall(TString& functionClassAccessorName, bool callToFunctionThroughBaseAccessor, bool isACallThroughStaticShaderClassName,
    TShaderCompositionVariable* compositionTargeted, HlslToken idToken, TIntermTyped*& node, TIntermTyped* base)
{
    // arguments
    TFunction* functionCall = new TFunction(idToken.string, TType(EbtVoid));
    TIntermTyped* arguments = nullptr;

    // methods have an implicit first argument of the calling object.
    if (base != nullptr)
        parseContext.handleFunctionArgument(functionCall, arguments, base);

    if (!acceptArguments(functionCall, arguments))
        return false;
    
    TString nameOfShaderOwningTheFunction = functionClassAccessorName;
    if (compositionTargeted != nullptr)
    {
        //we call a function through a composition
        nameOfShaderOwningTheFunction = compositionTargeted->shaderTypeName;
    }

    // We now have the method mangled name, find the corresponding method in the shader library
    const TString& methodMangledName = functionCall->getDeclaredMangledName();
    bool onlyLookInParentClasses = (callToFunctionThroughBaseAccessor == true);
    XkslShaderDefinition::ShaderIdentifierLocation identifierLocation = findShaderClassMethod(nameOfShaderOwningTheFunction, methodMangledName, onlyLookInParentClasses);

    if (!identifierLocation.isMethod())
    {
        //we keep looking if we can match another method by casting some of the function parameters
        identifierLocation = findShaderClassBestMatchingMethod(nameOfShaderOwningTheFunction, functionCall, onlyLookInParentClasses);
    }

    if (identifierLocation.isMethod())
    {
        //check if the function call is permitted!
        TFunction* currentFunctionBeingParsed = getFunctionCurrentlyParsed();
        XkslShaderDefinition* shaderBeingParsed = getShaderCurrentlyParsed();
        if (currentFunctionBeingParsed == nullptr || shaderBeingParsed == nullptr){
            //Maybe we can have a method call while we're not parsing a function, to initialize consts variables?
            if (currentFunctionBeingParsed == nullptr) error(TString("We should be parsing a method"));
            if (shaderBeingParsed == nullptr) error(TString("We should be parsing a shader"));
            return false;
        }

        //============================================================================
        //XKSL RULES: check the validity of the function call
        if (compositionTargeted != nullptr)
        {
            //we're calling a function through a composition, can we access the composition variable?
            if (currentFunctionBeingParsed->getType().getQualifier().isStatic)
            {
                //static method can only access consts
                error(TString("A static method: " + currentFunctionBeingParsed->getName() + TString(" is accessing a non-const variable: ") + compositionTargeted->variableName));
                return false;
            }
            else
            {
                //the function can access the composition only if the classes are related
                XkslShaderDefinition* shaderOwningTheComposition = getShaderClassDefinition(functionClassAccessorName);
                if (shaderOwningTheComposition == nullptr) {
                    error(TString("undeclared class: ") + functionClassAccessorName);
                    return false;
                }

                if (!IsShaderEqualOrSubClassOf(shaderBeingParsed, shaderOwningTheComposition)) {
                    error(TString("Shader: ") + shaderBeingParsed->shaderFullName + TString(" cannot access the composition: ") +
                        shaderOwningTheComposition->shaderFullName + "." + compositionTargeted->variableName);
                    return false;
                }
            }
        }

        if (currentFunctionBeingParsed->getType().getQualifier().isStatic)
        {
            //we're currently parsing a static method, we can only call another static method
            if (!identifierLocation.method->getType().getQualifier().isStatic)
            {
                error(TString("A static method: " + currentFunctionBeingParsed->getName() + TString(" is calling a non-static method: ") + identifierLocation.method->getName()));
                return false;
            }
        }
        else
        {
            //otherwise: if we're calling a non-static method: the caller and called shaders have to be related (the child calling a parent method)
            //unless we call a method through a composition of course...
            if (!identifierLocation.method->getType().getQualifier().isStatic)
            {
                if (compositionTargeted == nullptr)
                {
                    XkslShaderDefinition* shaderOwningTheFunction = getShaderClassDefinition(nameOfShaderOwningTheFunction);
                    if (shaderOwningTheFunction == nullptr) {
                        error(TString("undeclared class: ") + nameOfShaderOwningTheFunction);
                        return false;
                    }

                    if (!IsShaderEqualOrSubClassOf(shaderBeingParsed, shaderOwningTheFunction)) {
#ifdef XKSLANG_ENFORCE_NEW_XKSL_RULES
                        error(TString("invalid non-static function call. Shader: ") + shaderBeingParsed->shaderFullName + TString(" is not related to shader: ") + shaderOwningTheFunction->shaderFullName);
                        return false;
#else
                        //set the method as static
                        warning(TString("invalid non-static function call. Shader: ") + shaderBeingParsed->shaderFullName + TString(" is not related to shader: ") + shaderOwningTheFunction->shaderFullName);
                        identifierLocation.method->getWritableType().getQualifier().isStatic = true;
#endif
                    }
                }
            }
        }
        //============================================================================

        node = parseContext.handleFunctionCall(idToken.loc, identifierLocation.method, arguments, callToFunctionThroughBaseAccessor, isACallThroughStaticShaderClassName, compositionTargeted);
    }
    else
    {
        //function not found as a method from our shader library, so we look in the global list of method
        //could be a call to instrisic functions
        node = parseContext.handleFunctionCall(idToken.loc, functionCall, arguments, false, nullptr);
    }

    return true;
}

// The function_call identifier was already recognized, and passed in as idToken.
//
// function_call
//      : [idToken] arguments
//
bool HlslGrammar::acceptFunctionCall(const TSourceLoc& loc, TString& name, TIntermTyped*& node, TIntermTyped* baseObject)
{
    // name
    TString* functionName = nullptr;
    if (baseObject == nullptr) {
        functionName = &name;
    } else if (parseContext.isBuiltInMethod(loc, baseObject, name)) {
        // Built-in methods are not in the symbol table as methods, but as global functions
        // taking an explicit 'this' as the first argument.
        functionName = NewPoolTString(BUILTIN_PREFIX);
        functionName->append(name);
    } else {
        if (! baseObject->getType().isStruct()) {
            expected("structure");
            return false;
        }
        functionName = NewPoolTString("");
        functionName->append(baseObject->getType().getTypeName());
        parseContext.addScopeMangler(*functionName);
        functionName->append(name);
    }

    // function
    TFunction* function = new TFunction(functionName, TType(EbtVoid));

    // arguments
    TIntermTyped* arguments = nullptr;
    if (baseObject != nullptr) {
        // Non-static member functions have an implicit first argument of the base object.
        parseContext.handleFunctionArgument(function, arguments, baseObject);
    }
    if (! acceptArguments(function, arguments))
        return false;

    // call
    node = parseContext.handleFunctionCall(loc, function, arguments);

    return node != nullptr;
}

// arguments
//      : LEFT_PAREN expression COMMA expression COMMA ... RIGHT_PAREN
//
// The arguments are pushed onto the 'function' argument list and
// onto the 'arguments' aggregate.
//
bool HlslGrammar::acceptArguments(TFunction* function, TIntermTyped*& arguments)
{
    // LEFT_PAREN
    if (! acceptTokenClass(EHTokLeftParen))
        return false;

    // RIGHT_PAREN
    if (acceptTokenClass(EHTokRightParen))
        return true;

    // must now be at least one expression...
    do {
        // expression
        TIntermTyped* arg;
        if (! acceptAssignmentExpression(arg))
            return false;

        // hook it up
        parseContext.handleFunctionArgument(function, arguments, arg);

        // COMMA
        if (! acceptTokenClass(EHTokComma))
            break;
    } while (true);

    // RIGHT_PAREN
    if (! acceptTokenClass(EHTokRightParen)) {

        if (this->xkslShaderParsingOperation == XkslShaderParsingOperationEnum::ParseXkslShaderNewTypesDefinition ||
            this->xkslShaderParsingOperation == XkslShaderParsingOperationEnum::ParseXkslShaderConstVariables ||
            this->xkslShaderParsingOperation == XkslShaderParsingOperationEnum::ParseXkslShaderMembersAndMethodsDeclarations ||
            this->xkslShaderParsingOperation == XkslShaderParsingOperationEnum::ParseXkslShaderConstStatements)
            return false; //return false but it's not necessary an error: the expression can be resolved later

        expected(")");
        return false;
    }

    return true;
}

bool HlslGrammar::acceptLiteralFloatValue(float& value)
{
    bool negativeSign = false;
    if (acceptTokenClass(EHTokDash)) negativeSign = true;

    switch (token.tokenClass) {
        case EHTokIntConstant:
            value = (float)token.i;
            break;
        case EHTokUintConstant:
            value = (float)token.u;
            break;
        case EHTokFloatConstant:
            value = (float)token.d;
            break;
        case EHTokDoubleConstant:
            value = (float)token.d;
            break;

        default:
            if (negativeSign) recedeToken();
            return false;
    }

    if (negativeSign) value = -value;
    advanceToken();

    return true;
}

bool HlslGrammar::acceptLiteralIntValue(int& value)
{
    bool negativeSign = false;
    if (acceptTokenClass(EHTokDash)) negativeSign = true;

    switch (token.tokenClass) {
        case EHTokIntConstant:
            value = (int)token.i;
            break;
        case EHTokUintConstant:
            value = (int)token.u;
            break;

        default:
            if (negativeSign) recedeToken();
            return false;
    }

    if (negativeSign) value = -value;
    advanceToken();

    return true;
}

bool HlslGrammar::acceptLiteral(TIntermTyped*& node)
{
    switch (token.tokenClass) {
    case EHTokIntConstant:
        node = intermediate.addConstantUnion(token.i, token.loc, true);
        break;
    case EHTokUintConstant:
        node = intermediate.addConstantUnion(token.u, token.loc, true);
        break;
    case EHTokFloatConstant:
        node = intermediate.addConstantUnion(token.d, EbtFloat, token.loc, true);
        break;
    case EHTokDoubleConstant:
        node = intermediate.addConstantUnion(token.d, EbtDouble, token.loc, true);
        break;
    case EHTokBoolConstant:
        node = intermediate.addConstantUnion(token.b, token.loc, true);
        break;
    case EHTokStringConstant:
        node = intermediate.addConstantUnion(token.string, token.loc, true);
        break;

    default:
        return false;
    }

    advanceToken();

    return true;
}

// simple_statement
//      : SEMICOLON
//      | declaration_statement
//      | expression SEMICOLON
//
bool HlslGrammar::acceptSimpleStatement(TIntermNode*& statement)
{
    // SEMICOLON
    if (acceptTokenClass(EHTokSemicolon))
        return true;

    // declaration
    if (acceptDeclaration(statement))
        return true;

    // expression
    TIntermTyped* node;
    if (acceptExpression(node))
        statement = node;
    else
        return false;

    // SEMICOLON (following an expression)
    if (acceptTokenClass(EHTokSemicolon))
        return true;
    else {
        expected(";");
        return false;
    }
}

// compound_statement
//      : LEFT_CURLY statement statement ... RIGHT_CURLY
//
bool HlslGrammar::acceptCompoundStatement(TIntermNode*& retStatement)
{
    TIntermAggregate* compoundStatement = nullptr;

    // LEFT_CURLY
    if (! acceptTokenClass(EHTokLeftBrace))
        return false;

    // statement statement ...
    TIntermNode* statement = nullptr;
    while (acceptStatement(statement)) {
        TIntermBranch* branch = statement ? statement->getAsBranchNode() : nullptr;
        if (branch != nullptr && (branch->getFlowOp() == EOpCase ||
                                  branch->getFlowOp() == EOpDefault)) {
            // hook up individual subsequences within a switch statement
            parseContext.wrapupSwitchSubsequence(compoundStatement, statement);
            compoundStatement = nullptr;
        } else {
            // hook it up to the growing compound statement
            compoundStatement = intermediate.growAggregate(compoundStatement, statement);
        }
    }
    if (compoundStatement)
        compoundStatement->setOperator(EOpSequence);

    retStatement = compoundStatement;

    // RIGHT_CURLY
    return acceptTokenClass(EHTokRightBrace);
}

bool HlslGrammar::acceptScopedStatement(TIntermNode*& statement)
{
    parseContext.pushScope();
    bool result = acceptStatement(statement);
    parseContext.popScope();

    return result;
}

bool HlslGrammar::acceptScopedCompoundStatement(TIntermNode*& statement)
{
    parseContext.pushScope();
    bool result = acceptCompoundStatement(statement);
    parseContext.popScope();

    return result;
}

// statement
//      : attributes attributed_statement
//
// attributed_statement
//      : compound_statement
//      | simple_statement
//      | selection_statement
//      | switch_statement
//      | case_label
//      | default_label
//      | iteration_statement
//      | jump_statement
//
bool HlslGrammar::acceptStatement(TIntermNode*& statement)
{
    statement = nullptr;

    // attributes
    TAttributeMap attributes;
    acceptAttributes(attributes);

    // attributed_statement
    switch (peek()) {
    case EHTokLeftBrace:
        return acceptScopedCompoundStatement(statement);

    case EHTokIf:
        return acceptSelectionStatement(statement, attributes);

    case EHTokSwitch:
        return acceptSwitchStatement(statement, attributes);

    case EHTokFor:
    case EHTokDo:
    case EHTokWhile:
    case EHTokForEach:
        return acceptIterationStatement(statement, attributes);

    case EHTokContinue:
    case EHTokBreak:
    case EHTokDiscard:
    case EHTokReturn:
        return acceptJumpStatement(statement);

    case EHTokCase:
        return acceptCaseLabel(statement);
    case EHTokDefault:
        return acceptDefaultLabel(statement);

    case EHTokRightBrace:
        // Performance: not strictly necessary, but stops a bunch of hunting early,
        // and is how sequences of statements end.
        return false;

    default:
        return acceptSimpleStatement(statement);
    }

    return true;
}

bool HlslGrammar::checkForXkslStructMemberAttribute(TVector<TShaderMemberAttribute>& memberAttributes)
{
    // accept the [ XXX ] syntax, or [ XXX("value") ] syntaxes

    while (true)
    {
        TString attributeName;
        TString attributeValue;

        // LEFT_BRACKET?
        if (!acceptTokenClass(EHTokLeftBracket))
            return true;

        // attribute
        HlslToken idTokenName;
        if (!acceptIdentifier(idTokenName)) {
            expected("identifier");
            return false;
        }
        attributeName = *idTokenName.string;

        if (acceptTokenClass(EHTokLeftParen))
        {
            HlslToken tokenValue = token;
            if (acceptTokenClass(EHTokStringConstant)) attributeValue = *tokenValue.string;
            else {
                if (!acceptTokenClass(EHTokIdentifier))
                {
                    expected("identifier");
                    return false;
                }
                attributeValue = *tokenValue.string;
            }

            if (!acceptTokenClass(EHTokRightParen)) {
                expected(")");
                return false;
            }
        }

        // RIGHT_BRACKET
        if (!acceptTokenClass(EHTokRightBracket)) {
            expected("]");
            return false;
        }

        if (attributeName == "Link")
        {
            //For "Link" attributes: we check if the shader has a "LinkType" generic overriding the value
            if (this->xkslShaderCurrentlyParsed == nullptr) {
                error("No shader currently parsed"); return false;
            }

            unsigned int countGenerics = (unsigned int)(this->xkslShaderCurrentlyParsed->listGenerics.size());
            for (unsigned int c = 0; c < countGenerics; c++)
            {
                const ShaderGenericAttribute& aGeneric = this->xkslShaderCurrentlyParsed->listGenerics[c];
                if (aGeneric.type->getBasicType() == EbtLinkType)
                {
                    const TString& genericName = *(aGeneric.type->getUserIdentifierName());
                    if (genericName == attributeValue)
                    {
                        attributeValue = aGeneric.expressionConstValue;
                        break;
                    }
                }
            }
        }

        memberAttributes.push_back(TShaderMemberAttribute(attributeName, attributeValue));
    }

    return true;
}

// attributes
//      : [zero or more:] bracketed-attribute
//
// bracketed-attribute:
//      : LEFT_BRACKET scoped-attribute RIGHT_BRACKET
//      : LEFT_BRACKET LEFT_BRACKET scoped-attribute RIGHT_BRACKET RIGHT_BRACKET
//
// scoped-attribute:
//      : attribute
//      | namespace COLON COLON attribute
//
// attribute:
//      : UNROLL
//      | UNROLL LEFT_PAREN literal RIGHT_PAREN
//      | FASTOPT
//      | ALLOW_UAV_CONDITION
//      | BRANCH
//      | FLATTEN
//      | FORCECASE
//      | CALL
//      | DOMAIN
//      | EARLYDEPTHSTENCIL
//      | INSTANCE
//      | MAXTESSFACTOR
//      | OUTPUTCONTROLPOINTS
//      | OUTPUTTOPOLOGY
//      | PARTITIONING
//      | PATCHCONSTANTFUNC
//      | NUMTHREADS LEFT_PAREN x_size, y_size,z z_size RIGHT_PAREN
//
void HlslGrammar::acceptAttributes(TAttributeMap& attributes)
{
    // For now, accept the [ XXX(X) ] syntax, but drop all but
    // numthreads, which is used to set the CS local size.
    // TODO: subset to correct set?  Pass on?
    do {
        HlslToken attributeToken;

        // LEFT_BRACKET?
        if (! acceptTokenClass(EHTokLeftBracket))
            return;
        // another LEFT_BRACKET?
        bool doubleBrackets = false;
        if (acceptTokenClass(EHTokLeftBracket))
            doubleBrackets = true;

        // attribute? (could be namespace; will adjust later)
        if (!acceptIdentifier(attributeToken)) {
            if (!peekTokenClass(EHTokRightBracket)) {
                expected("namespace or attribute identifier");
                advanceToken();
            }
        }

        TString nameSpace;
        if (acceptTokenClass(EHTokColonColon)) {
            // namespace COLON COLON
            nameSpace = *attributeToken.string;
            // attribute
            if (!acceptIdentifier(attributeToken)) {
                expected("attribute identifier");
                return;
            }
        }

        TIntermAggregate* expressions = nullptr;

        // (x, ...)
        if (acceptTokenClass(EHTokLeftParen)) {
            expressions = new TIntermAggregate;

            TIntermTyped* node;
            bool expectingExpression = false;

            while (acceptAssignmentExpression(node)) {
                expectingExpression = false;
                expressions->getSequence().push_back(node);
                if (acceptTokenClass(EHTokComma))
                    expectingExpression = true;
            }

            // 'expressions' is an aggregate with the expressions in it
            if (! acceptTokenClass(EHTokRightParen))
                expected(")");

            // Error for partial or missing expression
            if (expectingExpression || expressions->getSequence().empty())
                expected("expression");
        }

        // RIGHT_BRACKET
        if (!acceptTokenClass(EHTokRightBracket)) {
            expected("]");
            return;
        }
        // another RIGHT_BRACKET?
        if (doubleBrackets && !acceptTokenClass(EHTokRightBracket)) {
            expected("]]");
            return;
        }

        // Add any values we found into the attribute map.  This accepts
        // (and ignores) values not mapping to a known TAttributeType;
        attributes.setAttribute(nameSpace, attributeToken.string, expressions);
    } while (true);
}

// selection_statement
//      : IF LEFT_PAREN expression RIGHT_PAREN statement
//      : IF LEFT_PAREN expression RIGHT_PAREN statement ELSE statement
//
bool HlslGrammar::acceptSelectionStatement(TIntermNode*& statement, const TAttributeMap& attributes)
{
    TSourceLoc loc = token.loc;

    const TSelectionControl control = parseContext.handleSelectionControl(attributes);

    // IF
    if (! acceptTokenClass(EHTokIf))
        return false;

    // so that something declared in the condition is scoped to the lifetimes
    // of the then-else statements
    parseContext.pushScope();

    // LEFT_PAREN expression RIGHT_PAREN
    TIntermTyped* condition;
    if (! acceptParenExpression(condition))
        return false;
    condition = parseContext.convertConditionalExpression(loc, condition);
    if (condition == nullptr)
        return false;

    // create the child statements
    TIntermNodePair thenElse = { nullptr, nullptr };

    ++parseContext.controlFlowNestingLevel;  // this only needs to work right if no errors

    // then statement
    if (! acceptScopedStatement(thenElse.node1)) {
        expected("then statement");
        return false;
    }

    // ELSE
    if (acceptTokenClass(EHTokElse)) {
        // else statement
        if (! acceptScopedStatement(thenElse.node2)) {
            expected("else statement");
            return false;
        }
    }

    // Put the pieces together
    statement = intermediate.addSelection(condition, thenElse, loc, control);
    parseContext.popScope();
    --parseContext.controlFlowNestingLevel;

    return true;
}

// switch_statement
//      : SWITCH LEFT_PAREN expression RIGHT_PAREN compound_statement
//
bool HlslGrammar::acceptSwitchStatement(TIntermNode*& statement, const TAttributeMap& attributes)
{
    // SWITCH
    TSourceLoc loc = token.loc;

    const TSelectionControl control = parseContext.handleSelectionControl(attributes);

    if (! acceptTokenClass(EHTokSwitch))
        return false;

    // LEFT_PAREN expression RIGHT_PAREN
    parseContext.pushScope();
    TIntermTyped* switchExpression;
    if (! acceptParenExpression(switchExpression)) {
        parseContext.popScope();
        return false;
    }

    // compound_statement
    parseContext.pushSwitchSequence(new TIntermSequence);

    ++parseContext.controlFlowNestingLevel;
    bool statementOkay = acceptCompoundStatement(statement);
    --parseContext.controlFlowNestingLevel;

    if (statementOkay)
        statement = parseContext.addSwitch(loc, switchExpression, statement ? statement->getAsAggregate() : nullptr, control);

    parseContext.popSwitchSequence();
    parseContext.popScope();

    return statementOkay;
}

// iteration_statement
//      : WHILE LEFT_PAREN condition RIGHT_PAREN statement
//      | DO LEFT_BRACE statement RIGHT_BRACE WHILE LEFT_PAREN expression RIGHT_PAREN SEMICOLON
//      | FOR LEFT_PAREN for_init_statement for_rest_statement RIGHT_PAREN statement
//
// Non-speculative, only call if it needs to be found; WHILE or DO or FOR already seen.
bool HlslGrammar::acceptIterationStatement(TIntermNode*& statement, const TAttributeMap& attributes)
{
    TSourceLoc loc = token.loc;
    TIntermTyped* condition = nullptr;

    EHlslTokenClass loop = peek();
    assert(loop == EHTokDo || loop == EHTokFor || loop == EHTokWhile || loop == EHTokForEach);

    //  WHILE or DO or FOR
    advanceToken();
    
    const TLoopControl control = parseContext.handleLoopControl(attributes);

    switch (loop) {

    case EHTokForEach:
    {
        TString *currentShadercName = getCurrentShaderName();
        if (currentShadercName == nullptr) {
            error("We can only parse a foreach expression within a shader class");
            return false;
        }

        if (!acceptTokenClass(EHTokLeftParen)){
            expected("foreach: (");
            return false;
        }

        ///variable type: the foreach variable type can either be a known type ("var"), or an identifier (a class name)
        TString foreachVariableTypeString;
        {
            TType foreachVariableType;
            if (acceptIdentifierTokenClass(foreachVariableTypeString)) {
                //fine for now: we will check later the type validity
            }
            else if (acceptType(foreachVariableType)) {
                if (foreachVariableType.getBasicType() != EbtUndefinedVar) {
                    error("foreach: invalid loop variable type. \"var\" or shader className expected");
                    return false;
                }
            }
            else {
                error("foreach: invalid loop variable type. \"var\" or shader className expected");
                return false;
            }
        }

        TString variableNameTargetingTheComposition;
        if (!acceptIdentifierTokenClass(variableNameTargetingTheComposition)) {
            expected("foreach: variable name");
            return false;
        }

        //in
        if (!acceptTokenClass(EHTokIn))
        {
            expected("foreach: in");
            return false;
        }

        //name of the composition targeted
        TString compositionArrayVariableName;
        if (!acceptIdentifierTokenClass(compositionArrayVariableName)) {
            expected("foreach: composition variable name");
            return false;
        }

        //Check if we can find the composition targeted
        TShaderCompositionVariable compositionTargeted;
        bool findCompositionVariable = isIdentifierRecordedAsACompositionVariableName(currentShadercName, compositionArrayVariableName, true, compositionTargeted);
        if (!findCompositionVariable)
        {
            error(TString("foreach: Cannot find the array composition named: ") + compositionArrayVariableName + TString(" within the shader: ") + (*currentShadercName));
            return false;
        }

        //double check the foreach variable type (in the case of the type was explicit)
        if (foreachVariableTypeString.size() > 0) {
            if (foreachVariableTypeString.compare(compositionTargeted.shaderTypeName) != 0) {
                error(TString("foreach: the variable type: ") + foreachVariableTypeString + TString(" is not matching the composition type: ") + compositionTargeted.shaderTypeName);
                return false;
            }
        }

        //pile the variable
        TShaderVariableTargetingACompositionVariable variableTargetingACompositionVariable(compositionTargeted, variableNameTargetingTheComposition);
        this->listForeachArrayCompositionVariable.push_back(variableTargetingACompositionVariable);

        if (!acceptTokenClass(EHTokRightParen)){
            expected("foreach: )");
            return false;
        }

        //parseContext.pushScope();
        parseContext.nestLooping();

        // foreach statement
        if (!acceptScopedStatement(statement)) {
            expected("foreach sub-statement");
            return false;
        }

        parseContext.unnestLooping();
        //parseContext.popScope();

        //we define the statement as a forEach loop, so that we can add xksl extended info in the bytecode
        statement = intermediate.growAggregate(statement, nullptr);
        statement->getAsAggregate()->setOperator(EOpSequence);
        statement->getAsAggregate()->SetIsAForEachLoopBlockStatement(true, compositionTargeted);

        //unpile the variable
        this->listForeachArrayCompositionVariable.pop_back();

        return true;
    }

    case EHTokWhile:
        // so that something declared in the condition is scoped to the lifetime
        // of the while sub-statement
        parseContext.pushScope();  // this only needs to work right if no errors
        parseContext.nestLooping();
        ++parseContext.controlFlowNestingLevel;

        // LEFT_PAREN condition RIGHT_PAREN
        if (! acceptParenExpression(condition))
            return false;
        condition = parseContext.convertConditionalExpression(loc, condition);
        if (condition == nullptr)
            return false;

        // statement
        if (! acceptScopedStatement(statement)) {
            expected("while sub-statement");
            return false;
        }

        parseContext.unnestLooping();
        parseContext.popScope();
        --parseContext.controlFlowNestingLevel;

        statement = intermediate.addLoop(statement, condition, nullptr, true, loc, control);

        return true;

    case EHTokDo:
        parseContext.nestLooping();  // this only needs to work right if no errors
        ++parseContext.controlFlowNestingLevel;

        // statement
        if (! acceptScopedStatement(statement)) {
            expected("do sub-statement");
            return false;
        }

        // WHILE
        if (! acceptTokenClass(EHTokWhile)) {
            expected("while");
            return false;
        }

        // LEFT_PAREN condition RIGHT_PAREN
        TIntermTyped* condition;
        if (! acceptParenExpression(condition))
            return false;
        condition = parseContext.convertConditionalExpression(loc, condition);
        if (condition == nullptr)
            return false;

        if (! acceptTokenClass(EHTokSemicolon))
            expected(";");

        parseContext.unnestLooping();
        --parseContext.controlFlowNestingLevel;

        statement = intermediate.addLoop(statement, condition, 0, false, loc, control);

        return true;

    case EHTokFor:
    {
        // LEFT_PAREN
        if (! acceptTokenClass(EHTokLeftParen))
            expected("(");

        // so that something declared in the condition is scoped to the lifetime
        // of the for sub-statement
        parseContext.pushScope();

        // initializer
        TIntermNode* initNode = nullptr;
        if (! acceptSimpleStatement(initNode))
            expected("for-loop initializer statement");

        parseContext.nestLooping();  // this only needs to work right if no errors
        ++parseContext.controlFlowNestingLevel;

        // condition SEMI_COLON
        acceptExpression(condition);
        if (! acceptTokenClass(EHTokSemicolon))
            expected(";");
        if (condition != nullptr) {
            condition = parseContext.convertConditionalExpression(loc, condition);
            if (condition == nullptr)
                return false;
        }

        // iterator SEMI_COLON
        TIntermTyped* iterator = nullptr;
        acceptExpression(iterator);
        if (! acceptTokenClass(EHTokRightParen))
            expected(")");

        if (this->hasAnyErrorToBeProcessedAtTheTop())
        {
            parseContext.popScope();
            parseContext.unnestLooping();
            return false;
        }

        // statement
        if (! acceptScopedStatement(statement)) {
            expected("for sub-statement");
            return false;
        }

        statement = intermediate.addForLoop(statement, initNode, condition, iterator, true, loc, control);

        parseContext.popScope();
        parseContext.unnestLooping();
        --parseContext.controlFlowNestingLevel;

        return true;
    }

    default:
        return false;
    }
}

// jump_statement
//      : CONTINUE SEMICOLON
//      | BREAK SEMICOLON
//      | DISCARD SEMICOLON
//      | RETURN SEMICOLON
//      | RETURN expression SEMICOLON
//
bool HlslGrammar::acceptJumpStatement(TIntermNode*& statement)
{
    EHlslTokenClass jump = peek();
    switch (jump) {
    case EHTokContinue:
    case EHTokBreak:
    case EHTokDiscard:
    case EHTokReturn:
        advanceToken();
        break;
    default:
        // not something we handle in this function
        return false;
    }

    switch (jump) {
    case EHTokContinue:
        statement = intermediate.addBranch(EOpContinue, token.loc);
        break;
    case EHTokBreak:
        statement = intermediate.addBranch(EOpBreak, token.loc);
        break;
    case EHTokDiscard:
        statement = intermediate.addBranch(EOpKill, token.loc);
        break;

    case EHTokReturn:
    {
        // expression
        TIntermTyped* node;
        if (acceptExpression(node)) {
            // hook it up
            statement = parseContext.handleReturnValue(token.loc, node);
        }
        else
        {
            if (this->hasAnyErrorToBeProcessedAtTheTop()) return false; //The error will be processed later.
            statement = intermediate.addBranch(EOpReturn, token.loc);
        }
        break;
    }

    default:
        assert(0);
        return false;
    }

    // SEMICOLON
    if (! acceptTokenClass(EHTokSemicolon))
        expected(";");

    return true;
}

// case_label
//      : CASE expression COLON
//
bool HlslGrammar::acceptCaseLabel(TIntermNode*& statement)
{
    TSourceLoc loc = token.loc;
    if (! acceptTokenClass(EHTokCase))
        return false;

    TIntermTyped* expression;
    if (! acceptExpression(expression)) {
        expected("case expression");
        return false;
    }

    if (! acceptTokenClass(EHTokColon)) {
        expected(":");
        return false;
    }

    statement = parseContext.intermediate.addBranch(EOpCase, expression, loc);

    return true;
}

// default_label
//      : DEFAULT COLON
//
bool HlslGrammar::acceptDefaultLabel(TIntermNode*& statement)
{
    TSourceLoc loc = token.loc;
    if (! acceptTokenClass(EHTokDefault))
        return false;

    if (! acceptTokenClass(EHTokColon)) {
        expected(":");
        return false;
    }

    statement = parseContext.intermediate.addBranch(EOpDefault, loc);

    return true;
}

// array_specifier
//      : LEFT_BRACKET integer_expression RGHT_BRACKET ... // optional
//      : LEFT_BRACKET RGHT_BRACKET // optional
//
void HlslGrammar::acceptArraySpecifier(TArraySizes*& arraySizes)
{
    arraySizes = nullptr;

    // Early-out if there aren't any array dimensions
    if (!peekTokenClass(EHTokLeftBracket))
        return;

    // If we get here, we have at least one array dimension.  This will track the sizes we find.
    arraySizes = new TArraySizes;

    // Collect each array dimension.
    while (acceptTokenClass(EHTokLeftBracket)) {
        TSourceLoc loc = token.loc;
        TIntermTyped* sizeExpr = nullptr;

        // Array sizing expression is optional.  If omitted, array will be later sized by initializer list.
        const bool hasArraySize = acceptAssignmentExpression(sizeExpr);

        if (! acceptTokenClass(EHTokRightBracket)) {
            expected("]");
            return;
        }

        if (hasArraySize) {
            TArraySize arraySize;
            parseContext.arraySizeCheck(loc, sizeExpr, arraySize);
            arraySizes->addInnerSize(arraySize);
        } else {
            arraySizes->addInnerSize(0);  // sized by initializers.
        }
    }
}

//XKSL extensions
//Parse the post declaration following a shader declaration
// post_decls
//		: parent1, parent2, ...
bool HlslGrammar::acceptShaderClassParentsInheritance(TVector<ShaderInheritedParentDefinition>& listParents)
{
    do {
        // COLON 
        if (acceptTokenClass(EHTokColon))
        {
            do
            {
                //We accept either identifier (unknown class name), or a known class name
                HlslToken idToken;
                if (!acceptIdentifier(idToken))
                {
                    expected("shader parent name");
                    return false;
                }

                //the parent identifier can set some generic values
                TVector<TString*> listGenericValuesExpression;
                if (peekTokenClass(EHTokLeftAngle))
                {
                    if (!checkShaderGenericValuesExpression(listGenericValuesExpression)) {
                        error("failed to check for shader generic values");
                        return false;
                    }
                }

                ShaderInheritedParentDefinition parentDefinition;
                parentDefinition.parentName = NewPoolTString(idToken.string->c_str());
                if (listGenericValuesExpression.size() > 0)
                {
                    parentDefinition.listGenericsValue = new TVector<TString*>;
                    *(parentDefinition.listGenericsValue) = listGenericValuesExpression;
                }
                else parentDefinition.listGenericsValue = nullptr;

                listParents.push_back(parentDefinition);

                if (acceptTokenClass(EHTokComma)) continue;
                else break;

            } while (true);
        }

        break;

    } while (true);

    return true;
}

// post_decls
//      : COLON semantic // optional
//        COLON PACKOFFSET LEFT_PAREN c[Subcomponent][.component] RIGHT_PAREN // optional
//        COLON REGISTER LEFT_PAREN [shader_profile,] Type#[subcomp]opt (COMMA SPACEN)opt RIGHT_PAREN // optional
//        COLON LAYOUT layout_qualifier_list
//        annotations // optional
//
// Return true if any tokens were accepted. That is,
// false can be returned on successfully recognizing nothing,
// not necessarily meaning bad syntax.
//
// XKSL extensions: if userDefinedSemantic is not null, we assign it with the semantic string value
// (if more than one semantic is set, return an error)
bool HlslGrammar::acceptPostDecls(TQualifier& qualifier, TString* userDefinedSemantic)
{
    bool found = false;

    do {
        // COLON
        if (acceptTokenClass(EHTokColon)) {
            found = true;
            HlslToken idToken;
            if (peekTokenClass(EHTokLayout))
                acceptLayoutQualifierList(qualifier);
            else if (acceptTokenClass(EHTokPackOffset)) {
                // PACKOFFSET LEFT_PAREN c[Subcomponent][.component] RIGHT_PAREN
                if (! acceptTokenClass(EHTokLeftParen)) {
                    expected("(");
                    return false;
                }
                HlslToken locationToken;
                if (! acceptIdentifier(locationToken)) {
                    expected("c[subcomponent][.component]");
                    return false;
                }
                HlslToken componentToken;
                if (acceptTokenClass(EHTokDot)) {
                    if (! acceptIdentifier(componentToken)) {
                        expected("component");
                        return false;
                    }
                }
                if (! acceptTokenClass(EHTokRightParen)) {
                    expected(")");
                    break;
                }
                parseContext.handlePackOffset(locationToken.loc, qualifier, *locationToken.string, componentToken.string);
            } else if (! acceptIdentifier(idToken)) {
                expected("layout, semantic, packoffset, or register");
                return false;
            } else if (*idToken.string == "register") {
                // REGISTER LEFT_PAREN [shader_profile,] Type#[subcomp]opt (COMMA SPACEN)opt RIGHT_PAREN
                // LEFT_PAREN
                if (! acceptTokenClass(EHTokLeftParen)) {
                    expected("(");
                    return false;
                }
                HlslToken registerDesc;  // for Type#
                HlslToken profile;
                if (! acceptIdentifier(registerDesc)) {
                    expected("register number description");
                    return false;
                }
                if (registerDesc.string->size() > 1 && !isdigit((*registerDesc.string)[1]) &&
                                                       acceptTokenClass(EHTokComma)) {
                    // Then we didn't really see the registerDesc yet, it was
                    // actually the profile.  Adjust...
                    profile = registerDesc;
                    if (! acceptIdentifier(registerDesc)) {
                        expected("register number description");
                        return false;
                    }
                }
                int subComponent = 0;
                if (acceptTokenClass(EHTokLeftBracket)) {
                    // LEFT_BRACKET subcomponent RIGHT_BRACKET
                    if (! peekTokenClass(EHTokIntConstant)) {
                        expected("literal integer");
                        return false;
                    }
                    subComponent = token.i;
                    advanceToken();
                    if (! acceptTokenClass(EHTokRightBracket)) {
                        expected("]");
                        break;
                    }
                }
                // (COMMA SPACEN)opt
                HlslToken spaceDesc;
                if (acceptTokenClass(EHTokComma)) {
                    if (! acceptIdentifier(spaceDesc)) {
                        expected ("space identifier");
                        return false;
                    }
                }
                // RIGHT_PAREN
                if (! acceptTokenClass(EHTokRightParen)) {
                    expected(")");
                    break;
                }
                parseContext.handleRegister(registerDesc.loc, qualifier, profile.string, *registerDesc.string, subComponent, spaceDesc.string);
            } else {
                // semantic, in idToken.string
                TString semantic = *idToken.string;

                //XKSL extensiosn: a shader can have a Semantic generics overriding the semantic name
                if (this->xkslShaderCurrentlyParsed != nullptr)
                {
                    unsigned int countGenerics = (unsigned int)(this->xkslShaderCurrentlyParsed->listGenerics.size());
                    for (unsigned int c = 0; c < countGenerics; c++)
                    {
                        const ShaderGenericAttribute& aGeneric = this->xkslShaderCurrentlyParsed->listGenerics[c];
                        if (aGeneric.type->getBasicType() == EbtSemanticType)
                        {
                            const TString& genericName = *(aGeneric.type->getUserIdentifierName());
                            if (genericName == semantic)
                            {
                                semantic = aGeneric.expressionConstValue;
                                break;
                            }
                        }
                    }
                }

                TString semanticUpperCase = semantic;
                std::transform(semanticUpperCase.begin(), semanticUpperCase.end(), semanticUpperCase.begin(), ::toupper);
                parseContext.handleSemantic(idToken.loc, qualifier, mapSemantic(semanticUpperCase.c_str()), semanticUpperCase);

                if (userDefinedSemantic != nullptr)
                {
                    if (userDefinedSemantic->length() > 0){
                        error("can only accept one user-defined semantic");
                        return false;
                    }
                    *userDefinedSemantic = semantic;
                }
            }
        } else if (peekTokenClass(EHTokLeftAngle)) {
            found = true;
            acceptAnnotations(qualifier);
        } else
            break;

    } while (true);

    return found;
}

//
// Get the stream of tokens from the scanner, but skip all syntactic/semantic
// processing.
//
bool HlslGrammar::captureBlockTokens(TVector<HlslToken>& tokens)
{
    if (! peekTokenClass(EHTokLeftBrace))
        return false;

    int braceCount = 0;

    do {
        switch (peek()) {
        case EHTokLeftBrace:
            ++braceCount;
            break;
        case EHTokRightBrace:
            --braceCount;
            break;
        case EHTokNone:
            // End of input before balance { } is bad...
            return false;
        default:
            break;
        }

        tokens.push_back(token);
        advanceToken();
    } while (braceCount > 0);

    return true;
}

// Return a string for just the types that can also be declared as an identifier.
const char* HlslGrammar::getTypeString(EHlslTokenClass tokenClass) const
{
    switch (tokenClass) {
    case EHTokSample:     return "sample";
    case EHTokHalf:       return "half";
    case EHTokHalf1x1:    return "half1x1";
    case EHTokHalf1x2:    return "half1x2";
    case EHTokHalf1x3:    return "half1x3";
    case EHTokHalf1x4:    return "half1x4";
    case EHTokHalf2x1:    return "half2x1";
    case EHTokHalf2x2:    return "half2x2";
    case EHTokHalf2x3:    return "half2x3";
    case EHTokHalf2x4:    return "half2x4";
    case EHTokHalf3x1:    return "half3x1";
    case EHTokHalf3x2:    return "half3x2";
    case EHTokHalf3x3:    return "half3x3";
    case EHTokHalf3x4:    return "half3x4";
    case EHTokHalf4x1:    return "half4x1";
    case EHTokHalf4x2:    return "half4x2";
    case EHTokHalf4x3:    return "half4x3";
    case EHTokHalf4x4:    return "half4x4";
    case EHTokBool:       return "bool";
    case EHTokFloat:      return "float";
    case EHTokDouble:     return "double";
    case EHTokInt:        return "int";
    case EHTokUint:       return "uint";
    case EHTokMin16float: return "min16float";
    case EHTokMin10float: return "min10float";
    case EHTokMin16int:   return "min16int";
    case EHTokMin12int:   return "min12int";
    default:
        return nullptr;
    }
}

} // end namespace glslang

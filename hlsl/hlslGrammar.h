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

#ifndef HLSLGRAMMAR_H_
#define HLSLGRAMMAR_H_

#include "hlslParseHelper.h"
#include "hlslOpMap.h"
#include "hlslTokenStream.h"

namespace glslang {

    class TAttributeMap;
    class TFunctionDeclarator;
    
    enum class XkslShaderParsingOperationEnum {
        Undefined,
        ParseXkslShaderDeclarations,
        ParseXkslShaderNewTypesDefinition,
        ParseXkslShaderMembersAndMethodsDeclarations,
        ParseXkslShaderConstStatements,
        ParseXkslShaderMethodsDefinition,
    };

    // Should just be the grammar aspect of HLSL.
    // Described in more detail in hlslGrammar.cpp.

    class HlslGrammar : public HlslTokenStream {
    public:
        HlslGrammar(HlslScanContext& scanner, HlslParseContext& parseContext)
            : HlslTokenStream(scanner), parseContext(parseContext), intermediate(parseContext.intermediate),
            xkslShaderParsingOperation(XkslShaderParsingOperationEnum::Undefined),
            xkslShaderToParse(nullptr), xkslShaderCurrentlyParsed(nullptr), xkslShaderLibrary(nullptr), functionCurrentlyParsed(nullptr), shaderMethodOrMemberTypeCurrentlyParsed(nullptr),
            dependencyUniqueCounter(0), unknownIdentifierToProcessAtTheTop(nullptr), throwErrorWhenParsingUnidentifiedSymbol(true), shaderWhereMembersCanBeFound(nullptr),
            typeIdentifiers(false) { }
        virtual ~HlslGrammar() { }

        bool parse();

        bool parseXKslShaderDeclaration(XkslShaderLibrary* shaderLibrary);
        bool parseXKslShaderNewTypesDefinition(XkslShaderLibrary* shaderLibrary, XkslShaderDefinition* shaderToParse);
        bool parseXKslShaderMembersAndMethodsDeclaration(XkslShaderLibrary* shaderLibrary, XkslShaderDefinition* shaderToParse);
        bool parseXKslShaderMethodsDefinition(XkslShaderLibrary* shaderLibrary, XkslShaderDefinition* shaderToParse);
        TIntermTyped* parseXkslShaderAssignmentExpression(XkslShaderLibrary* shaderLibrary, XkslShaderDefinition* currentShader, bool errorWhenParsingUnidentifiedSymbol, XkslShaderDefinition* shaderWhereSomeMembersCanBeFound);

        void setUnknownIdentifierToProcessAtTheTop(TString* unknownIdentifier) { unknownIdentifierToProcessAtTheTop = unknownIdentifier; }
        const char* getUnknownIdentifier() { return unknownIdentifierToProcessAtTheTop == nullptr ? nullptr : unknownIdentifierToProcessAtTheTop->c_str(); }
        bool hasAnyErrorToBeProcessedAtTheTop() { return unknownIdentifierToProcessAtTheTop != nullptr; }
        void resetErrorsToBeProcessedAtTheTop() { unknownIdentifierToProcessAtTheTop = nullptr; }
        bool MustThrowAnErrorWhenParsingUnidentifiedSymbol() { return throwErrorWhenParsingUnidentifiedSymbol; }

    protected:
        HlslGrammar();
        HlslGrammar& operator=(const HlslGrammar&);

        void expected(const char*);
        void unimplemented(const char*);
        void error(const char*);
        void error(const TString&);
        void warning(const char*);
        void warning(const TString&);
        bool acceptIdentifier(HlslToken&);
        bool acceptClassReferenceAccessor(TString*& className, bool& isBase, bool& isACallThroughStaticShaderClassName, bool& isStreams, TShaderCompositionVariable& compositionTargeted);
        bool acceptShaderCustomType(const TString& shaderName, TType& type);
        bool acceptCompilationUnit();
        bool acceptDeclarationList(TIntermNode*&);
        bool acceptDeclaration(TIntermNode*&);
        bool acceptControlDeclaration(TIntermNode*& node);
        bool acceptSamplerDeclarationDX9(TType&);
        bool acceptSamplerState();
        bool acceptFullySpecifiedType(TType&);
        bool acceptFullySpecifiedType(TType&, TIntermNode*& nodeList);
        bool acceptQualifier(TQualifier&);
        bool acceptLayoutQualifierList(TQualifier&);
        bool acceptType(TType&);
        bool acceptType(TType&, TIntermNode*& nodeList);
        bool acceptTemplateVecMatBasicType(TBasicType&);
        bool acceptVectorTemplateType(TType&);
        bool acceptMatrixTemplateType(TType&);
        bool acceptTessellationDeclType(TBuiltInVariable&);
        bool acceptTessellationPatchTemplateType(TType&);
        bool acceptStreamOutTemplateType(TType&, TLayoutGeometry&);
        bool acceptOutputPrimitiveGeometry(TLayoutGeometry&);
        bool acceptAnnotations(TQualifier&);
        bool acceptSamplerType(TType&);
        bool acceptTextureType(TType&);
        bool acceptStructBufferType(TType&);
        bool acceptConstantBufferType(TType&);
        bool acceptStruct(TType&, TIntermNode*& nodeList);
        bool acceptStructDeclarationList(TTypeList*&, TIntermNode*& nodeList, TVector<TFunctionDeclarator>&);
        bool acceptMemberFunctionDefinition(TIntermNode*& nodeList, const TType&, const TString& memberName,
                                            TFunctionDeclarator&);
        bool acceptShaderClass(TType&);
        bool checkShaderGenericsList(TVector<TType*>& listGenericTypes);
        bool checkShaderGenericValuesExpression(TVector<TString*>& listGenericValues);
        bool parseShaderMembersAndMethods(XkslShaderDefinition* shader, TVector<TShaderClassFunction>* listMethodDeclaration);
        bool validateShaderDeclaredType(const TType& type);
        bool addShaderClassFunctionDeclaration(const TString& shaderName, TFunction& function, TVector<TShaderClassFunction>& functionList);
        bool acceptFunctionParameters(TFunction&);
        bool acceptParameterDeclaration(TFunction&);
        bool acceptFunctionDefinition(TFunctionDeclarator&, TIntermNode*& nodeList, TVector<HlslToken>* deferredTokens);
        bool acceptFunctionBody(TFunctionDeclarator& declarator, TIntermNode*& nodeList);
        bool acceptParenExpression(TIntermTyped*&);
        bool acceptExpression(TIntermTyped*&);
        bool acceptInitializer(TIntermTyped*&);
        bool acceptAssignmentExpression(TIntermTyped*&);
        bool acceptConditionalExpression(TIntermTyped*&);
        bool acceptBinaryExpression(TIntermTyped*&, PrecedenceLevel);
        bool acceptUnaryExpression(TIntermTyped*&);
        bool acceptPostfixExpression(TIntermTyped*&);
        bool acceptPostfixExpression(TIntermTyped*&, bool hasBaseAccessor, bool callThroughStaticShaderClassName, bool hasStreamAccessor, TString* classAccessor, TShaderCompositionVariable& compositionTargeted);
        bool acceptConstructor(TIntermTyped*&);
        bool acceptFunctionCall(const TSourceLoc&, TString& name, TIntermTyped*&, TIntermTyped* objectBase);
        bool acceptXkslFunctionCall(TString& functionClassAccessorName, bool callToFunctionThroughBaseAccessor, bool isACallThroughStaticShaderClassName, TShaderCompositionVariable* compositionTargeted, HlslToken, TIntermTyped*&, TIntermTyped* base);
        bool acceptXkslShaderComposition(TShaderCompositionVariable&);
        bool acceptArguments(TFunction*, TIntermTyped*&);
        bool acceptLiteral(TIntermTyped*&);
        bool acceptSimpleStatement(TIntermNode*&);
        bool acceptCompoundStatement(TIntermNode*&);
        bool acceptScopedStatement(TIntermNode*&);
        bool acceptScopedCompoundStatement(TIntermNode*&);
        bool acceptStatement(TIntermNode*&);
        bool acceptNestedStatement(TIntermNode*&);
        void acceptAttributes(TAttributeMap&);
        bool checkForXkslStructMemberAttribute(TString& attributeName);
        bool acceptSelectionStatement(TIntermNode*&);
        bool acceptSwitchStatement(TIntermNode*&);
        bool acceptIterationStatement(TIntermNode*&, const TAttributeMap&);
        bool acceptJumpStatement(TIntermNode*&);
        bool acceptCaseLabel(TIntermNode*&);
        bool acceptDefaultLabel(TIntermNode*&);
        void acceptArraySpecifier(TArraySizes*&);
        bool acceptPostDecls(TQualifier&, TString* userDefinedSemantic = nullptr);
        bool acceptDefaultParameterDeclaration(const TType&, TIntermTyped*&);

        bool captureBlockTokens(TVector<HlslToken>& tokens);
        const char* getTypeString(EHlslTokenClass tokenClass) const;

        //XKSL extensions
        bool acceptShaderClassParentsInheritance(TVector<ShaderInheritedParentDefinition>& listParents);
        TString* getCurrentShaderName();
        TFunction* getFunctionCurrentlyParsed();
        XkslShaderDefinition* getShaderCurrentlyParsed();
        int getCurrentShaderCountParents();
        TString* getCurrentShaderParentName(int index);
        XkslShaderDefinition* getShaderClassDefinition(const TString& shaderClassName);
        XkslShaderDefinition::ShaderIdentifierLocation findShaderClassMember(const TString& shaderClassName, bool hasStreamAccessor, const TString& memberName, bool onlyLookInParentClasses, int uniqueId = 0);
        XkslShaderDefinition::ShaderIdentifierLocation findShaderClassMethod(const TString& shaderClassName, const TString& methodName, bool onlyLookInParentClasses);
        TType* getTypeDefinedByTheShaderOrItsParents(const TString& shaderName, const TString& typeName, int uniqueId = 0);
        bool IsShaderEqualOrSubClassOf(XkslShaderDefinition* shader, XkslShaderDefinition* maybeParent);
        bool isRecordedAsAShaderName(const TString& name);
        bool isIdentifierRecordedAsACompositionVariableName(TString* accessorClassName, const TString& identifierName, bool lookForArraycomposition, TShaderCompositionVariable& compositionTargeted, int uniqueId = 0);
        void ResetShaderLibraryFlag();

        HlslParseContext& parseContext;  // state of parsing and helper functions for building the intermediate
        TIntermediate& intermediate;     // the final product, the intermediate representation, includes the AST
        bool typeIdentifiers;            // shader uses some types as identifiers
        
        //XKSL extensions
        TString getLabelForTokenType(EHlslTokenClass tokenType);
        XkslShaderParsingOperationEnum xkslShaderParsingOperation;
        XkslShaderDefinition* xkslShaderToParse;
        XkslShaderDefinition* xkslShaderCurrentlyParsed;
        TType* shaderMethodOrMemberTypeCurrentlyParsed;
        TFunction* functionCurrentlyParsed;
        XkslShaderLibrary* xkslShaderLibrary;
        TVector<TShaderVariableTargetingACompositionVariable> listForeachArrayCompositionVariable;  //if we parse a foreach loop: we pile (we can have nested foreach) the temporary variable composition name
        int dependencyUniqueCounter;

        XkslShaderDefinition* shaderWhereMembersCanBeFound;   //in some very special cases (resolving generic expressions for example) we can look into children class when parsing an unidentified symbol
        bool throwErrorWhenParsingUnidentifiedSymbol;         //if false, an unknown identifier won't throw an error, but will be pass to higher level
        TString* unknownIdentifierToProcessAtTheTop;          //when the parser meets an unknown identifier, in some case we want to stop parsing withouth throwing an error, and check if we can process it at the top level
    };

} // end namespace glslang

#endif // HLSLGRAMMAR_H_

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

    enum class XkslShaderParsingOperationEnum {
        Undefined,
        ParseXkslDeclarations,
        ParseXkslDefinitions,
        ParseXkslConstStatements,
    };

    class TAttributeMap; // forward declare

    // Should just be the grammar aspect of HLSL.
    // Described in more detail in hlslGrammar.cpp.

    class HlslGrammar : public HlslTokenStream {
    public:
        HlslGrammar(HlslScanContext& scanner, HlslParseContext& parseContext)
            : HlslTokenStream(scanner), parseContext(parseContext), intermediate(parseContext.intermediate),
            xkslShaderParsingOperation(XkslShaderParsingOperationEnum::Undefined), xkslShaderCurrentlyParsed(nullptr), xkslShaderLibrary(nullptr), functionCurrentlyParsed(nullptr){ }
        virtual ~HlslGrammar() { }

        bool parse();

        bool parseXKslShaderDeclaration(XkslShaderLibrary* shaderLibrary);
        bool parseXKslShaderDefinition(XkslShaderLibrary* shaderLibrary);
        TIntermTyped* parseXkslShaderAssignmentExpression(XkslShaderLibrary* shaderLibrary, XkslShaderDefinition* currentShader);

    protected:
        HlslGrammar();
        HlslGrammar& operator=(const HlslGrammar&);

        void expected(const char*);
        void unimplemented(const char*);
        void error(const char*);
        void error(const TString&);
        bool acceptIdentifier(HlslToken&);
        bool acceptClassReferenceAccessor(TString*& className, bool& isBase, bool& isStreams, TShaderCompositionVariable& compositionTargeted);
        bool acceptCompilationUnit();
        bool acceptDeclaration(TIntermNode*&);
        bool acceptDeclaration(TIntermNode*& node1, TIntermNode*& node2);
        bool acceptControlDeclaration(TIntermNode*& node);
        bool acceptSamplerDeclarationDX9(TType&);
        bool acceptSamplerState();
        bool acceptFullySpecifiedType(TIntermNode** node, TType&);
        bool acceptQualifier(TQualifier&);
        bool acceptLayoutQualifierList(TQualifier&);
        bool acceptType(TIntermNode** node, TType&);
        bool acceptTemplateVecMatBasicType(TBasicType&);
        bool acceptVectorTemplateType(TType&);
        bool acceptMatrixTemplateType(TType&);
        bool acceptTessellationDeclType();
        bool acceptTessellationPatchTemplateType(TType&);
        bool acceptStreamOutTemplateType(TType&, TLayoutGeometry&);
        bool acceptOutputPrimitiveGeometry(TLayoutGeometry&);
        bool acceptAnnotations(TQualifier&);
        bool acceptSamplerType(TType&);
        bool acceptTextureType(TType&);
        bool acceptStructBufferType(TType&);
        bool acceptStruct(TType&);
        bool acceptShaderClass(TIntermNode** node, TType&);
        bool acceptShaderAllVariablesAndFunctionsDeclaration(XkslShaderDefinition* shader, TVector<TShaderClassFunction>& functionList);
        bool acceptShaderClassFunctionsDefinition(const TString& shaderName, XkslShaderDefinition* shader);
        bool addShaderClassFunctionDeclaration(const TString& shaderName, TFunction& function, TVector<TShaderClassFunction>& functionList);
        bool acceptStructDeclarationList(TTypeList*&);
        bool acceptFunctionParameters(TFunction&);
        bool acceptParameterDeclaration(TFunction&);
        bool acceptFunctionDefinition(TFunction&, TIntermNode*& node1, TIntermNode*& node2, const TAttributeMap&);
        bool acceptParenExpression(TIntermTyped*&);
        bool acceptExpression(TIntermTyped*&);
        bool acceptInitializer(TIntermTyped*&);
        bool acceptAssignmentExpression(TIntermTyped*&);
        bool acceptConditionalExpression(TIntermTyped*&);
        bool acceptBinaryExpression(TIntermTyped*&, PrecedenceLevel);
        bool acceptUnaryExpression(TIntermTyped*&);
        bool acceptPostfixExpression(TIntermTyped*&);
        bool acceptPostfixExpression(TIntermTyped*&, bool hasBaseAccessor, bool hasStreamAccessor, TString* classAccessor, TShaderCompositionVariable& compositionTargeted);
        bool acceptConstructor(TIntermTyped*&);
        bool acceptFunctionCall(HlslToken, TIntermTyped*&, TIntermTyped* base = nullptr);
        bool acceptXkslFunctionCall(TString& functionClassAccessorName, bool callToFunctionFromBaseShaderClass, TShaderCompositionVariable* compositionTargeted, HlslToken, TIntermTyped*&, TIntermTyped* base);
        bool acceptXkslShaderComposition(TShaderCompositionVariable&);
        bool acceptArguments(TFunction*, TIntermTyped*&);
        bool acceptLiteral(TIntermTyped*&);
        bool acceptCompoundStatement(TIntermNode*&);
        bool acceptStatement(TIntermNode*&);
        bool acceptScopedStatement(TIntermNode*&);
        bool acceptScopedCompoundStatement(TIntermNode*&);
        bool acceptNestedStatement(TIntermNode*&);
        void acceptAttributes(TAttributeMap&);
        bool acceptSelectionStatement(TIntermNode*&);
        bool acceptSwitchStatement(TIntermNode*&);
        bool acceptIterationStatement(TIntermNode*&);
        bool acceptJumpStatement(TIntermNode*&);
        bool acceptCaseLabel(TIntermNode*&);
        bool acceptDefaultLabel(TIntermNode*&);
        void acceptArraySpecifier(TArraySizes*&);
        bool acceptPostDecls(TQualifier&, TString* userDefinedSemantic = nullptr);
        bool acceptDefaultParameterDeclaration(const TType&, TIntermTyped*&);

        //XKSL extensions
        void acceptShaderClassPostDecls(TIdentifierList*& parents);
        TString* getCurrentShaderName();
        TFunction* getFunctionCurrentlyParsed();
        XkslShaderDefinition* getShaderCurrentlyParsed();
        int getCurrentShaderCountParents();
        TString* getCurrentShaderParentName(int index);
        XkslShaderDefinition* getShaderClassDefinition(const TString& shaderClassName);
        XkslShaderDefinition::ShaderIdentifierLocation findShaderClassMember(const TString& shaderClassName, bool hasStreamAccessor, const TString& memberName);
        XkslShaderDefinition::ShaderIdentifierLocation findShaderClassMethod(const TString& shaderClassName, const TString& methodName);
        bool IsShaderEqualOrSubClassOf(XkslShaderDefinition* shader, XkslShaderDefinition* maybeParent);
        bool isRecordedAsAShaderName(const TString& name);
        bool isIdentifierRecordedAsACompositionVariableName(TString* accessorClassName, const TString& identifierName, bool lookForArraycomposition, TShaderCompositionVariable& compositionTargeted);

        HlslParseContext& parseContext;  // state of parsing and helper functions for building the intermediate
        TIntermediate& intermediate;     // the final product, the intermediate representation, includes the AST

        //XKSL extensions
        XkslShaderParsingOperationEnum xkslShaderParsingOperation;
        XkslShaderDefinition* xkslShaderCurrentlyParsed;
        TFunction* functionCurrentlyParsed;
        XkslShaderLibrary* xkslShaderLibrary;
        TVector<TShaderVariableTargetingACompositionVariable> listForeachArrayCompositionVariable;  //if we parse a foreach loop: we pile (we can have nested foreach) the temporary variable composition name
    };

} // end namespace glslang

#endif // HLSLGRAMMAR_H_

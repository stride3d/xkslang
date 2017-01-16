//
// Copyright (C) 

#ifndef XKSLANG_XKSL_MIXER_H__
#define XKSLANG_XKSL_MIXER_H__

#include <string>
#include <vector>

#include "define.h"
#include "SpxBytecode.h"
#include "SpxStreamParser.h"

namespace xkslang
{

enum class SPVObjectTypeEnum
{
    SPVTypeUndefined,
    SPVTypeFunction,
    SPVTypeShader
};

//=====================================================================================================================//
//=======================================  SPVObject, SPVShader, SPVFunction   ========================================//
//=====================================================================================================================//
class SPVFunction;
class SPVShader;
class SPVObject
{
public:
    SPVObject(SpxStreamParser* stream, int streamMixinOrder, SPVObjectTypeEnum objectType) : stream(stream), streamMixinOrder(streamMixinOrder), objectType(objectType) {}
    virtual ~SPVObject() {}

    SPVFunction* GetAsSPVFunction();
    SPVShader* GetAsSPVShader();

public:
    const SPVObjectTypeEnum objectType;

    SpxStreamParser* stream;  //stream that created the object
    int streamMixinOrder;     //Order of the stream that declared the object (if mixin("A", "B", "C") --> their order would be A=0, B=1, C=2)
    std::string declarationName;
};

class SPVFunction : public SPVObject
{
public:
    uint32_t opStart;       //index of OpFunction
    uint32_t opEnd;         //index of OpFunctionEnd
    bool isAbstract;
    bool isOverride;
    bool isStage;
    bool isClone;
    SPVShader* shaderOwner;
    SPVFunction* isOverriddenBy;

    SPVFunction(SpxStreamParser* stream, int streamMixinOrder)
        : SPVObject(stream, streamMixinOrder, SPVObjectTypeEnum::SPVTypeFunction),
        opStart(0), opEnd(0), isAbstract(false), isOverride(false), isStage(false), isClone(false),
        shaderOwner(nullptr), isOverriddenBy(nullptr){}

    void SetOverridingFunction(SPVFunction* f){ isOverriddenBy = f; }

private:
    void SetShaderOwner(SPVShader* p) {
        shaderOwner = p;
    }

friend class SPVShader;
};

class SPVShader : public SPVObject
{
public:
    uint32_t opDeclaration;       //index of type declaration (OpTypeXlslShaderClass)
    std::vector<SPVShader*> parents;
    std::vector<SPVFunction*> methods;

    SPVShader(SpxStreamParser* stream, int streamMixinOrder)
        : SPVObject(stream, streamMixinOrder, SPVObjectTypeEnum::SPVTypeShader), opDeclaration(0) {}

    void AddParent(SPVShader* p){
        parents.push_back(p);
    }

    void AddMethod(SPVFunction* m) {
        methods.push_back(m);
        m->SetShaderOwner(this);
    }
};


//=============================================================================================================//
//==============================================  XkslMixer  =================================================//
//============================================================================================================//
class XkslMixer
{
public:

    XkslMixer();
    virtual ~XkslMixer();

    void AddMixin(SpxBytecode* spirXBytecode);
    bool CreateMixinAST(std::vector<std::string>& messages);
    bool GenerateStageBytecode(SpvBytecode& bytecode, ShadingStage stage, std::string entryPoint, std::vector<std::string>& messages);

    bool AddSPVFunction(SPVFunction*);
    bool AddSPVShader(SPVShader*);
    SPVShader* GetSpvShaderByName(const std::string& name);

    SPVFunction* GetFunctionCalledByName(const std::string& name);

private:
    SpxStreamParser* DisassembleSpxBytecode(SpxBytecode* spirXBytecode, int streamMixinNum, std::vector<std::string>& msgs);
    void ProcessOverrideMethods();

    std::vector<SpxBytecode*> listMixins;
    std::vector<SpxStreamParser*> listSpxStream;

    std::vector<SPVFunction*> listAllFunctions; //list all functions defined by the mixins
    std::vector<SPVShader*> listAllShaders;     //list all shaders defined by the mixins

    bool m_astGenerated;
};

}  // namespace xkslang

#endif  // XKSLANG_XKSL_MIXER_H__

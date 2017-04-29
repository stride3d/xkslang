
#include "XkslangDLL.h"
#include "../XkslParser/XkslParser.h"
#include "../SpxMixer/SpxMixer.h"

using namespace std;

#pragma warning(disable:4996)  //disable annoying deprecation warnings

namespace xkslang
{
    static vector<string> errorMessages;
    static XkslParser* xkslParser = nullptr;

    void GetErrorMessages(char *buffer, int maxBufferLength)
    {
        maxBufferLength--;
        if (maxBufferLength <= 0) return;

        int offset = 0;
        int remainingLen = maxBufferLength;
        for (unsigned int n = 0; n < errorMessages.size(); ++n)
        {
            string& errorMsg = errorMessages[n];
            int len = (int)errorMsg.size();

            if (len > remainingLen) len = remainingLen;
            strncpy(buffer + offset, errorMsg.c_str(), len);

            remainingLen -= len;
            offset += len;
            if (remainingLen <= 0) break;

            buffer[offset] = '\n';
            remainingLen--;
            offset++;
            if (remainingLen <= 1) break;
        }

        if (offset > maxBufferLength) offset = maxBufferLength;
        buffer[offset] = 0;
    }

    //=====================================================================================================================
    //=====================================================================================================================
    // Parsing and conversion functions

    static bool error(const string& errorMsg)
    {
        errorMessages.push_back(errorMsg);
        return false;
    }

    static bool error(const char* errorMsg)
    {
        //cout << "XkslangDLL Error: " << errorMsg << endl;
        if (errorMsg == 0 || strlen(errorMsg) == 0) return false;
        errorMessages.push_back(string(errorMsg));

        return false;
    }

    bool InitializeParser()
    {
        if (xkslParser != nullptr) return error("Xkslang Parser has already been initialized");

        errorMessages.clear();

        xkslParser = new XkslParser();
        if (!xkslParser->InitialiseXkslang())
        {
            return error("Failed to initialize the Xkslang Parser");
        }

        return true;
    }

    void ReleaseParser()
    {
        if (xkslParser != nullptr)
        {
            xkslParser->Finalize();

            delete xkslParser;
            xkslParser = nullptr;
        }

        errorMessages.clear();
    }

    char* ConvertBytecodeToAscii(uint32_t* bytecode, int bytecodeSize, int* asciiBufferSize)
    {
        errorMessages.clear();
        *asciiBufferSize = -1;

        if (bytecode == nullptr || bytecodeSize <= 0) { error("bytecode is empty"); return nullptr; }
        if (xkslParser == nullptr) { error("Xkslang parser has not been initialized"); return nullptr; }

        string bytecodeText;
        std::vector<uint32_t> vecBytecode;
        for (int k = 0; k < bytecodeSize; k++) vecBytecode.push_back(bytecode[k]);
        
        if (!xkslParser->ConvertBytecodeToText(vecBytecode, bytecodeText)){
            error("Failed to convert the bytecode to Ascii");
            return nullptr;
        }

        int asciiBufferLen = bytecodeText.size();
        if (asciiBufferLen == 0) { error("The Ascii buffer is empty"); return nullptr; }

        //allocate a byte buffer using LocalAlloc, so we can return to the calling framework and let it delete it
        char* asciiBuffer = (char*)LocalAlloc(0, asciiBufferLen * sizeof(char));
        strncpy(asciiBuffer, bytecodeText.c_str(), asciiBufferLen);
        *asciiBufferSize = asciiBufferLen;
        return asciiBuffer;
    }

    static ShaderSourceLoaderCallback externalShaderDataCallback = nullptr;
    static bool callbackRequestDataForShader(const string& shaderName, string& shaderData)
    {
        if (externalShaderDataCallback == nullptr) return error("No callback function has been set");

        int dataLen;
        char* data = externalShaderDataCallback(shaderName.c_str(), &dataLen);
        if (data == nullptr || dataLen < 0) return false;

        shaderData.append(data);

        return true;
    }

    uint32_t* ConvertXkslShaderToSPX(char* shaderName, ShaderSourceLoaderCallback shaderDependencyCallback, int* bytecodeSize)
    {
        errorMessages.clear();
        *bytecodeSize = -1;

        if (xkslParser == nullptr) {error("Xkslang parser has not been initialized"); return nullptr;}
        if (shaderName == nullptr) {error("shaderName is null"); return nullptr;}
        if (shaderDependencyCallback == nullptr) {error("The callback function is null"); return nullptr;}
        externalShaderDataCallback = shaderDependencyCallback;

        SpxBytecode spirXBytecode;
        vector<ShaderGenericValues> listGenericsValue;
        ostringstream errorMessages;
        bool success = xkslParser->ConvertShaderToSpx(shaderName, callbackRequestDataForShader, listGenericsValue, spirXBytecode, &errorMessages);

        if (!success) {
            error(errorMessages.str().c_str());
            return nullptr;
        }

        const vector<uint32_t> bytecode = spirXBytecode.getBytecodeStream();
        int bytecodeLen = (int)bytecode.size();
        if (bytecodeLen <= 0) { error("Resulting bytecode is empty"); return nullptr; }

        //allocate a byte buffer using LocalAlloc, so we can return to the calling framework and let it delete it
        uint32_t* byteBuffer = (uint32_t*)LocalAlloc(0, bytecodeLen * sizeof(uint32_t));
        //for (int i = 0; i < bytecodeLen; ++i) byteBuffer[i] = bytecode[i];
        *bytecodeSize = bytecodeLen;
        uint32_t* pDest = byteBuffer;
        const uint32_t* pSrc = &bytecode[0];
        while (bytecodeLen-- > 0) *pDest++ = *pSrc++;
        return byteBuffer;
    }

    //=====================================================================================================================
    //=====================================================================================================================
    // Mixin functions

    //A mixer and its outputs bytecode
    class MixerData
    {
    public:
        SpxMixer* mixer;
        vector<OutputStageBytecode> stagesCompiledData;

        MixerData(SpxMixer* mixer) : mixer(mixer) {}
        ~MixerData() { if (mixer != nullptr) delete mixer; }
    };

    static std::vector<MixerData*> listMixerData;

    static MixerData* GetMixerForHandleId(uint32_t mixerHandleId)
    {
        uint32_t index = mixerHandleId - 1;
        if (mixerHandleId == 0 || index >= listMixerData.size()) return nullptr;

        MixerData* mixerData = listMixerData[index];
        if (mixerData == nullptr) return nullptr;

        return mixerData;
    }

    uint32_t CreateSpxShaderMixer()
    {
        SpxMixer* mixer = new SpxMixer();
        MixerData* mixerData = new MixerData(mixer);

        uint32_t handleId = listMixerData.size();
        listMixerData.push_back(mixerData);

        return handleId + 1;
    }

    bool ReleaseSpxShaderMixer(uint32_t mixerHandleId)
    {
        errorMessages.clear();

        MixerData* mixerData = GetMixerForHandleId(mixerHandleId);
        if (mixerData == nullptr)
            return error("Invalid mixer handle");

        uint32_t index = mixerHandleId - 1;
        if (mixerHandleId == 0 || index >= listMixerData.size())
            return error("Invalid mixer handle");

        delete mixerData;
        listMixerData[index] = nullptr;

        int indexLastValidElement = listMixerData.size() - 1;
        while (indexLastValidElement >= 0 && listMixerData[indexLastValidElement] == nullptr) indexLastValidElement--;

        if (indexLastValidElement != listMixerData.size() - 1)
        {
            if (indexLastValidElement < 0) listMixerData.clear();
            else listMixerData.resize(indexLastValidElement + 1);
        }

        return true;
    }

    bool MixinShader(uint32_t mixerHandleId, const char* shaderName, uint32_t* shaderSpxBytecode, int bytecodeSize)
    {
        errorMessages.clear();
        if (shaderSpxBytecode == nullptr || bytecodeSize <= 0) { error("bytecode is empty"); return false; }

        MixerData* mixerData = GetMixerForHandleId(mixerHandleId);
        if (mixerData == nullptr) return error("Invalid mixer handle");
        SpxMixer* mixer = mixerData->mixer;

        SpxBytecode spxBytecode(shaderName);
        std::vector<uint32_t>& bytecode = spxBytecode.getWritableBytecodeStream();
        bytecode.insert(bytecode.end(), shaderSpxBytecode, shaderSpxBytecode + bytecodeSize);

        vector<string> listShaderToMix;
        listShaderToMix.push_back(shaderName);

        vector<string> errorMsgs;
        bool success = mixer->Mixin(spxBytecode, listShaderToMix, errorMsgs);

        if (!success)
        {
            for (unsigned int k = 0; k < errorMsgs.size(); ++k) error(errorMsgs[k]);
        }

        return success;
    }

    bool CompileMixer(uint32_t mixerHandleId, OutputStageEntryPoint* stageEntryPointArray, int countStages)
    {
        errorMessages.clear();

        if (countStages <= 0) return error("Invalid number of output stages");

        MixerData* mixerData = GetMixerForHandleId(mixerHandleId);
        if (mixerData == nullptr) return error("Invalid mixer handle");
        SpxMixer* mixer = mixerData->mixer;

        for (int s = 0; s < countStages; s++)
        {
            if (!IsAValidOutputStage(stageEntryPointArray[s].stage)) return error("Output stage is not valid: " + to_string((int)(stageEntryPointArray[s].stage)));
            mixerData->stagesCompiledData.push_back(OutputStageBytecode(stageEntryPointArray[s].stage, string(stageEntryPointArray[s].entryPointName)));
        }

        SpvBytecode finalSpv;
        vector<string> errorMsgs;
        mixerData->stagesCompiledData.clear();

        asdfasdf;
        //for (int s = 0; s < countStages; s++)
        //{
        //    if (!IsAValidOutputStage(stageEntryPointArray[s].stage)) return error("Output stage is not valid: " + to_string((int)(stageEntryPointArray[s].stage)));
        //    outputStages.push_back(OutputStageBytecode(stageEntryPointArray[s].stage, string(stageEntryPointArray[s].entryPointName)));
        //}

        bool success = mixer->Compile(mixerData->stagesCompiledData, errorMsgs, nullptr, nullptr, nullptr, nullptr, &finalSpv, nullptr);
        if (!success)
        {
            for (unsigned int k = 0; k < errorMsgs.size(); ++k)
                error(errorMsgs[k]);
            return false;
        }

        return true;
    }

    uint32_t* GetMixerCompiledBytecodeForStage(uint32_t mixerHandleId, ShadingStageEnum stage, int* bytecodeSize)
    {
        errorMessages.clear();

        MixerData* mixerData = GetMixerForHandleId(mixerHandleId);
        if (mixerData == nullptr) {error("Invalid mixer handle"); return nullptr;}
        
        OutputStageBytecode* outputStageBytecode = nullptr;
        for (unsigned int k = 0; k < mixerData->stagesCompiledData.size(); k++)
        {
            if (mixerData->stagesCompiledData[k].stage == stage)
            {
                outputStageBytecode = &(mixerData->stagesCompiledData[k]);
                break;
            }
        }
        if (outputStageBytecode == nullptr || outputStageBytecode->resultingBytecode.GetBytecodeSize() == 0) {
            error("The mixer has not been compiled for the given stage");
            return nullptr;
        }

        /// copy the bytecode into the output buffer: allocate a byte buffer using LocalAlloc, so we can return to the calling framework and let it delete it
        const std::vector<uint32_t>& bytecode = outputStageBytecode->resultingBytecode.getBytecodeStream();
        unsigned int bytecodeLen = bytecode.size();
        *bytecodeSize = bytecodeLen;
        uint32_t* byteBuffer = (uint32_t*)LocalAlloc(0, bytecodeLen * sizeof(uint32_t));
        uint32_t* pDest = byteBuffer;
        const uint32_t* pSrc = &bytecode[0];
        while (bytecodeLen-- > 0) *pDest++ = *pSrc++;
        return byteBuffer;
    }
}


#include "XkslangDLL.h"
#include "../XkslParser.h"

using namespace std;

namespace xkslang
{
    static vector<string> xkslangErrorMessages;
    static XkslParser* xkslParser = nullptr;

    void GetErrorMessages(char *buffer, int maxBufferLength)
    {
        maxBufferLength--;
        if (maxBufferLength <= 0) return;

        unsigned int offset = 0;
        int remainingLen = maxBufferLength;
        for (unsigned int n = 0; n < xkslangErrorMessages.size(); ++n)
        {
            string& errorMsg = xkslangErrorMessages[n];
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

    static bool error(const char* errorMsg)
    {
        //cout << "XkslangDLL Error: " << errorMsg << endl;
        if (errorMsg == 0 || strlen(errorMsg) == 0) return false;
        xkslangErrorMessages.push_back(string(errorMsg));

        return false;
    }

    char* ConvertBytecodeToAscii(uint32_t* bytecode, int bytecodeSize, int* asciiBufferSize)
    {
        *asciiBufferSize = -1;
        if (bytecode == nullptr || bytecodeSize <= 0) return nullptr;

        //allocate a byte buffer using LocalAlloc, so we can return to the calling framework and let it delete it
        int asciiBufferLen = bytecodeSize;
        char* asciiBuffer = (char*)LocalAlloc(0, asciiBufferLen * sizeof(char));
        for (int i = 0; i < asciiBufferLen; ++i) asciiBuffer[i] = i; //asciiBuffer[i] = (char)bytecode[i];
        *asciiBufferSize = asciiBufferLen;
        return asciiBuffer;
    }

    bool InitializeXkslang()
    {
        if (xkslParser != nullptr) return error("Xkslang has already been initialized");

        xkslParser = new XkslParser();
        if (!xkslParser->InitialiseXkslang())
        {
            return error("Failed to initialize the XkslParser");
        }

        return true;
    }

    void ReleaseXkslang()
    {
        if (xkslParser != nullptr)
        {
            xkslParser->Finalize();

            delete xkslParser;
            xkslParser = nullptr;
        }

        xkslangErrorMessages.clear();
    }

    static ShaderDataGetterCallback externalShaderDataCallback = nullptr;
    static bool callbackRequestDataForShader(const string& shaderName, string& shaderData)
    {
        if (externalShaderDataCallback == nullptr) return error("No callback function has been set");

        int dataLen;
        char* data = externalShaderDataCallback(shaderName.c_str(), &dataLen);
        if (data == nullptr || dataLen < 0) return false;

        shaderData.append(data);

        return true;
    }

    uint32_t* ConvertXkslShaderToSPX(char* shaderName, ShaderDataGetterCallback shaderDependencyCallback, int* bytecodeSize)
    {
        *bytecodeSize = -1;
        if (xkslParser == nullptr) {error("Xkslang has not been initialized"); return nullptr;}
        if (shaderName == nullptr) {error("shaderName is null"); return nullptr;}
        if (shaderDependencyCallback == nullptr) {error("The callback function is null"); return nullptr;}
        externalShaderDataCallback = shaderDependencyCallback;

        SpxBytecode spirXBytecode;
        vector<ShaderGenericsValue> listGenericsValue;
        ostringstream errorMessages;
        bool success = xkslParser->ConvertShaderToSpx(shaderName, callbackRequestDataForShader, listGenericsValue, spirXBytecode, &errorMessages, nullptr);

        if (!success) {
            error(errorMessages.str().c_str());
            return nullptr;
        }

        const vector<uint32_t> bytecode = spirXBytecode.getBytecodeStream();
        int bytecodeLen = (int)bytecode.size();
        if (bytecodeLen <= 0) { error("Resulting bytecode is empty"); return nullptr; }

        //allocate a byte buffer using LocalAlloc, so we can return to the calling framework and let it delete it
        uint32_t* byteBuffer = (uint32_t*)LocalAlloc(0, bytecodeLen * sizeof(uint32_t));
        for (int i = 0; i < bytecodeLen; ++i) byteBuffer[i] = bytecode[i];
        *bytecodeSize = bytecodeLen;
        return byteBuffer;
    }
}

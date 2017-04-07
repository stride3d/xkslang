
#include "XkslangDLL.h"
#include "../XkslParser.h"

using namespace std;

namespace xkslang
{
    uint32_t* Foo(int* length)
    {
        //https://social.msdn.microsoft.com/Forums/vstudio/en-US/49f97ef6-3917-473d-9464-421bb3a0c0b0/allocate-memory-in-a-c-dll-use-in-c-app?forum=vcgeneral
        //use LocalAlloc in your C++ code instead of new; this could be freed in C# using System::Runtime::InteropServices::FreeHGlobal. 

        //char* toto = "PROUT";
        //int len = strlen(toto) + 1;
        //char* buff = (char*)CoTaskMemAlloc(len);  //the buffer will be released on the client side
        //strcpy_s(buff, len, toto);

        int len = 10;
        uint32_t* byteBuffer = (uint32_t*)LocalAlloc(0, len * sizeof(uint32_t));
        for (int i = 0; i < len; ++i) byteBuffer[i] = i;
        
        *length = len;
        return byteBuffer;
    }

    bool ConvertXkslShaderToSPX(char* shaderName, ShaderDependencyCallback shaderDependencyCallback)
    {
        if (shaderName == nullptr) return false;

        //cout << "TOTO" << endl;
        shaderDependencyCallback("A");

        XkslParser parser;
        if (!parser.InitialiseXkslang())
        {
            cout << "Failed to initialize the XkslParser" << endl;
            return false;
        }

        shaderDependencyCallback("B");

        parser.Finalize();

        shaderDependencyCallback("C");

        return true;
    }
}

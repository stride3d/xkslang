#pragma once

#include <stdio.h>
#include <iostream>
#include <objbase.h>

namespace xkslang
{
    typedef char* (__stdcall *ShaderDataGetterCallback)(const char* shaderName, int* dataLen);

    extern "C" __declspec(dllexport) bool InitializeXkslang();

    extern "C" __declspec(dllexport) void ReleaseXkslang();

    extern "C" __declspec(dllexport) uint32_t* ConvertXkslShaderToSPX(char* shaderName, ShaderDataGetterCallback shaderDependencyCallback, int* bytecodeSize);

    //Utility function to help converting a bytecode to a human-readable ascii file
    extern "C" __declspec(dllexport) char* ConvertBytecodeToAscii(uint32_t* bytecode, int bytecodeSize, int* asciiBufferSize);

    //Return the error messages
    extern "C" __declspec(dllexport) void GetErrorMessages(char *buffer, int bufferLength);
}
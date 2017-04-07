#pragma once

#include <stdio.h>
#include <iostream>
#include <objbase.h>

namespace xkslang
{
    typedef void(__stdcall * ShaderDependencyCallback)(char* shaderName);

    extern "C" __declspec(dllexport) uint32_t* Foo(int* length);

    extern "C" __declspec(dllexport) bool ConvertXkslShaderToSPX(char* shaderName, ShaderDependencyCallback shaderDependencyCallback);
}
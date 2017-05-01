#pragma once

#include <stdio.h>
#include <iostream>
#include <objbase.h>

#include "../Common/define.h"

namespace xkslang
{
    struct OutputStageEntryPoint
    {
        ShadingStageEnum stage;
        const char* entryPointName;
    };

    //Return the error messages after an operation failed
    extern "C" __declspec(dllexport) void GetErrorMessages(char *buffer, int bufferLength);

    //=====================================================================================================================
    //=====================================================================================================================
    // Parsing and conversion functions: convert xksl shaders to SPX bytecode

    //callback function prototype
    typedef char* (__stdcall *ShaderSourceLoaderCallback)(const char* shaderName, int* dataLen);

    //Xkslang initialization: To be called before calling parsing functions
    extern "C" __declspec(dllexport) bool InitializeParser();

    //To be called when we finish parsing a file
    extern "C" __declspec(dllexport) void ReleaseParser();

    //Convert an xksl shader into SPX bytecode
    // shaderName: name of the shader to convert
    // shaderDependencyCallback: callback function, called by xkslang everytime the parser requests data for a shader (shaderName at first, then all its dependencies if any)
    // Return:
    //  null: if the conversion failed (user can call GetErrorMessages function to get more details)
    //  pointer to the bytecode if the operation succeeded
    //    The pointer is allocated on the dll side (using LocalAlloc function), and has to be deleted by the caller
    //    bytecodeSize parameter contains the length of the bytecode buffer
    extern "C" __declspec(dllexport) uint32_t* ConvertXkslShaderToSPX(char* shaderName, ShaderSourceLoaderCallback shaderDependencyCallback, int* bytecodeSize);

    //Utility function to help converting a bytecode to a human-readable ascii file
    extern "C" __declspec(dllexport) char* ConvertBytecodeToAscii(uint32_t* bytecode, int bytecodeSize, int* asciiBufferSize);

    //=====================================================================================================================
    //=====================================================================================================================
    // Mixin functions: Mix SPX shaders to generate SPV bytecode for specific output stages

    //Create a new mixin object
    // Return: the mixin object handle Id, or 0 if there is any error
    extern "C" __declspec(dllexport) uint32_t CreateSpxShaderMixer();

    extern "C" __declspec(dllexport) bool ReleaseSpxShaderMixer(uint32_t mixerHandleId);

    extern "C" __declspec(dllexport) bool MixinShader(uint32_t mixerHandleId, const char* shaderName, uint32_t* shaderSpxBytecode, int bytecodeSize);

    extern "C" __declspec(dllexport) bool CompileMixer(uint32_t mixerHandleId, OutputStageEntryPoint* stageEntryPointArray, int countStages);

    extern "C" __declspec(dllexport) uint32_t* GetMixerCompiledBytecodeForStage(uint32_t mixerHandleId, ShadingStageEnum stage, int* bytecodeSize);

    //extern "C" __declspec(dllexport) bool GetMixerCompiledBytecodeForStage(uint32_t mixerHandleId, ShadingStageEnum stage, uint32_t* bytecodeBuffer, int bufferSize);
}


/*
Reference code from C# app

public class BindingDllClass
{
    [UnmanagedFunctionPointer(CallingConvention.StdCall)]
    public delegate StringBuilder ShaderDataGetterCallbackDelegate(string shaderName, ref int len);
        
    [DllImport("Xkslang.dll", CallingConvention = CallingConvention.Cdecl)]
    [return: MarshalAs(UnmanagedType.I1)]
    public static extern bool InitializeXkslang();

    [DllImport("Xkslang.dll", CallingConvention = CallingConvention.Cdecl)]
    public static extern void ReleaseXkslang();

    [DllImport("Xkslang.dll", CallingConvention = CallingConvention.Cdecl)]
    public static extern IntPtr ConvertXkslShaderToSPX([MarshalAs(UnmanagedType.LPStr)] string shaderName, [MarshalAs(UnmanagedType.FunctionPtr)] ShaderDataGetterCallbackDelegate callbackPointer, ref int bytecodeLength);

    [DllImport("Xkslang.dll", CallingConvention = CallingConvention.Cdecl)]
    public static extern IntPtr ConvertBytecodeToAscii(Int32[] bytecode, int bytecodeSize, ref int bytecodeLength);
        
    [DllImport("Xkslang.dll", CallingConvention = CallingConvention.Cdecl)]
    public static extern void GetErrorMessages(StringBuilder str, int bufferMaxLen);

    public static StringBuilder ShaderSourceLoaderCallback(string shaderName, ref int len)
    {
        StringBuilder shaderData = new StringBuilder("shader ShaderA{int varA;}");
        len = shaderData.Length;
        return shaderData;
    }
}

    Int32[] bytecode = null;
    string asciiBytecode = null;

    try
    {
        string shaderName = "JojoEffect";
        bool success = true;

        success = BindingDllClass.InitializeXkslang();
        if (!success)
        {
            StringBuilder errorMsg = new StringBuilder(1024);
            BindingDllClass.GetErrorMessages(errorMsg, errorMsg.Capacity);
            throw new Exception("Error initializing Xkslang: " + errorMsg);
        }

        int bytecodeLength = 0;
        IntPtr pBytecodeBuffer = BindingDllClass.ConvertXkslShaderToSPX(shaderName, BindingDllClass.ShaderSourceLoaderCallback, ref bytecodeLength);
        if (pBytecodeBuffer == null || bytecodeLength < 0)
        {
            StringBuilder errorMsg = new StringBuilder(1024);
            BindingDllClass.GetErrorMessages(errorMsg, errorMsg.Capacity);
            throw new Exception("Failed to convert the Xksl shader \"" + shaderName + "\".\n" + errorMsg);
        }

        //copy the bytecode and free the object created by the dll
        bytecode = new Int32[bytecodeLength];
        Marshal.Copy(pBytecodeBuffer, bytecode, 0, bytecodeLength);
        Marshal.FreeHGlobal(pBytecodeBuffer);

        int asciiBufferLength = 0;
        IntPtr pAsciiBytecodeBuffer = BindingDllClass.ConvertBytecodeToAscii(bytecode, bytecodeLength, ref asciiBufferLength);
        if (pAsciiBytecodeBuffer == null || asciiBufferLength < 0)
        {
            throw new Exception("Failed to convert the Spx bytecode into Ascii");
        }

        Byte[] asciiByteArray = new Byte[asciiBufferLength];
        Marshal.Copy(pAsciiBytecodeBuffer, asciiByteArray, 0, asciiBufferLength);
        Marshal.FreeHGlobal(pAsciiBytecodeBuffer);
        asciiBytecode = System.Text.Encoding.UTF8.GetString(asciiByteArray);
    }
    catch (Exception e)
    {
        string[] messages = e.Message.Split('\n');
        foreach (string str in messages) Console.WriteLine(str);
        throw e;
    }
    finally
    {
        BindingDllClass.ReleaseXkslang();
    }

*/
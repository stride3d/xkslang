#pragma once

#include <stdio.h>
#include <iostream>
#include <objbase.h>

namespace xkslang
{
    typedef char* (__stdcall *ShaderSourceLoaderCallback)(const char* shaderName, int* dataLen);

    extern "C" __declspec(dllexport) bool InitializeXkslang();

    extern "C" __declspec(dllexport) void ReleaseXkslang();

    extern "C" __declspec(dllexport) uint32_t* ConvertXkslShaderToSPX(char* shaderName, ShaderSourceLoaderCallback shaderDependencyCallback, int* bytecodeSize);

    //Utility function to help converting a bytecode to a human-readable ascii file
    extern "C" __declspec(dllexport) char* ConvertBytecodeToAscii(uint32_t* bytecode, int bytecodeSize, int* asciiBufferSize);

    //Return the error messages
    extern "C" __declspec(dllexport) void GetErrorMessages(char *buffer, int bufferLength);
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
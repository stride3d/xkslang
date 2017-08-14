#pragma once

#include <stdio.h>
#include <iostream>
#include <objbase.h>

#include "../Common/xkslangDefine.h"
#include "../SpxMixer/EffectReflection.h"

namespace xkslangDll
{
	//=====================================================================================================================
	//Structs declaration
	//=====================================================================================================================
    struct OutputStageEntryPoint
    {
    public:
        xkslang::ShadingStageEnum stage;
        const char* entryPointName;

        OutputStageEntryPoint() : stage(xkslang::ShadingStageEnum::Undefined), entryPointName(nullptr) {}
        OutputStageEntryPoint(xkslang::ShadingStageEnum stage, const char* entryPointName) : stage(stage), entryPointName(entryPointName) {}
    };

    //We can query shader information from a SPX bytecode (only the shader name for now)
    struct BytecodeShaderInformation
    {
    public:
        char* ShaderName;

        BytecodeShaderInformation(char* shaderName): ShaderName(shaderName) {}
    };

	//Defines a constant buffer member data (to be easily exchanged between native and managed apps)
	struct ConstantBufferMemberReflectionDescriptionData
	{
	public:
		int32_t Offset;
		const char* KeyName;
        const char* RawName;
        const char* LogicalGroup;

        xkslang::EffectParameterReflectionClass Class;
        xkslang::EffectParameterReflectionType Type;
		int32_t RowCount;
		int32_t ColumnCount;
		int32_t ArrayElements;
		int32_t Size;
		int32_t Alignment;
		int32_t ArrayStride;
		int32_t MatrixStride;
		int32_t CountMembers;
        ConstantBufferMemberReflectionDescriptionData* StructMembers;

		ConstantBufferMemberReflectionDescriptionData(const int32_t offset, const char* keyName, const char* rawName, const char* logicalGroup, const xkslang::TypeReflectionDescription& t)
			: Offset(offset), KeyName(keyName), RawName(rawName), LogicalGroup(logicalGroup),
              Class(t.Class), Type(t.Type), RowCount(t.RowCount), ColumnCount(t.ColumnCount), ArrayElements(t.ArrayElements), Size(t.Size),
			  Alignment(t.Alignment), ArrayStride(t.ArrayStride), MatrixStride(t.MatrixStride), CountMembers(t.CountMembers), StructMembers(nullptr) {}

        void Set(const int32_t offset, const char* keyName, const char* rawName, const char* logicalGroup, xkslang::EffectParameterReflectionClass c, xkslang::EffectParameterReflectionType t,
            int countRow, int countColumn, int size, int alignment, int arrayStride, int matrixStride, int arrayElements, int countMembers)
        {
            this->Offset = offset;
            this->KeyName = keyName;
            this->RawName = rawName;
            this->LogicalGroup = logicalGroup;
            this->Class = c;
            this->Type = t;
            this->RowCount = countRow;
            this->ColumnCount = countColumn;
            this->Size = size;
            this->Alignment = alignment;
            this->ArrayStride = arrayStride;
            this->MatrixStride = matrixStride;
            this->ArrayElements = arrayElements;
            this->CountMembers = countMembers;
            this->StructMembers = nullptr;
        }

        void SetMembersDetails(ConstantBufferMemberReflectionDescriptionData* structMembers, int countMembers)
        {
            this->StructMembers = structMembers;
            this->CountMembers = countMembers;
        }
	};

	//struct containing a constant buffer data (to be easily exchanged between native and managed apps)
	struct ConstantBufferReflectionDescriptionData
	{
	public:
		int32_t Size;
		int32_t CountMembers;
		const char* CbufferName;
		ConstantBufferMemberReflectionDescriptionData* Members;

		ConstantBufferReflectionDescriptionData(const int32_t size, const int32_t countMembers, const char* name, ConstantBufferMemberReflectionDescriptionData* members)
			: Size(size), CountMembers(countMembers), CbufferName(name), Members(members) {}
	};

    class MethodData
    {
    public:
        const char* Name;
        const char* ShaderClassName;
        int32_t IsStage;

        MethodData() {}
        MethodData(const char* name, const char* shaderClassName, int32_t isStage)
            : Name(name), ShaderClassName(shaderClassName), IsStage(isStage) {}
    };

	//struct containing a resource binding data (to be easily exchanged between native and managed apps)
	struct EffectResourceBindingDescriptionData
	{
	public:
		xkslang::ShadingStageEnum Stage;
		xkslang::EffectParameterReflectionClass Class;
		xkslang::EffectParameterReflectionType Type;
		const char* KeyName;
        const char* RawName;
        const char* ResourceGroupName;

		EffectResourceBindingDescriptionData(const xkslang::EffectResourceBindingDescription& e, const char* keyName, const char* rawName, const char* resourceGroupName)
            : Stage(e.Stage), Class(e.Class), Type(e.Type), KeyName(keyName), RawName(rawName), ResourceGroupName(resourceGroupName) {}
	};

	//struct containing an input attribute data (to be easily exchanged between native and managed apps)
	struct ShaderInputAttributeDescriptionData
	{
	public:
		const char* SemanticName;
        int SemanticIndex;

		ShaderInputAttributeDescriptionData(const char* semanticName, int semanticIndex) : SemanticName(semanticName), SemanticIndex(semanticIndex) {}
	};

	//=====================================================================================================================
	//=====================================================================================================================
	//Return the error messages after an operation failed
	extern "C" __declspec(dllexport) char* GetErrorMessages();

    //=====================================================================================================================
    //=====================================================================================================================
    // Parsing and conversion functions: convert xksl shaders to SPX bytecode

    //callback function prototype
    typedef char* (__stdcall *ShaderSourceLoaderCallback)(const char* shaderName, int32_t* dataLen);

    //Xkslang initialization: To be called before calling parsing functions
    extern "C" __declspec(dllexport) bool InitializeParser();

    //To be called when we finish parsing a file
    extern "C" __declspec(dllexport) void ReleaseParser();

    //Convert an xksl shader into SPX bytecode
    // mainShaderName: name of the main shader to convert (this shader may be depending on other shaders, resulting in parsing a whole bynch of them)
    // stringShadersWithGenerics: let us specify generic values for the shaders we're going to parse (can be null)
    //   Syntax: "shaderA<g1, g2, ...> shaderB<g1, g2, ...> ..."
    // listMacroDefinition: let us define macro definition (can be null)
    //   Syntax: "MACRO_01 "value" MACRO_02 "value" ..."
    // shaderDependencyCallback: callback function, called by xkslang everytime the parser requests data for a shader (mainShaderName at first, then all its dependencies, if any)
    // resultingBytecodeSize: pointer returning the size of the generated bytecode
    // Return:
    //  null: if the conversion failed (user can call GetErrorMessages function to get more details)
    //   pointer to the bytecode if the operation succeeded
    //   The pointer is allocated on the dll side (using LocalAlloc function), and has to be deleted by the caller (resultingBytecodeSize parameter contains the length of the bytecode buffer)
    extern "C" __declspec(dllexport) uint32_t* ConvertXkslShaderToSPX(const char* mainShaderName, const char* stringShadersWithGenerics, const char* stringMacrosDefinition,
        ShaderSourceLoaderCallback shaderDependencyCallback, int32_t* resultingBytecodeSize);

    //=====================================================================================================================
    //=====================================================================================================================
    //Utility function to help converting a bytecode to a human-readable ascii file
    extern "C" __declspec(dllexport) char* ConvertBytecodeToAsciiText(uint32_t* bytecode, int32_t bytecodeSize, int32_t* asciiBufferSize);

    extern "C" __declspec(dllexport) char* ConvertBytecodeToGlsl(uint32_t* bytecode, int32_t bytecodeSize, int32_t* asciiBufferSize);

    extern "C" __declspec(dllexport) char* ConvertBytecodeToHlsl(uint32_t* bytecode, int32_t bytecodeSize, int32_t shaderModel, int32_t* asciiBufferSize);

    //Utility function Parsing the bytecode to return the names of shaders it contains
    extern "C" __declspec(dllexport) bool GetBytecodeShadersInformation(uint32_t* bytecode, int32_t bytecodeSize, BytecodeShaderInformation** shadersInfo, int32_t* countShaders);

    //=====================================================================================================================
    //=====================================================================================================================
    // Mixin functions: Mix SPX shaders to generate SPV bytecode for specific output stages

    extern "C" __declspec(dllexport) bool InitializeMixer();

    extern "C" __declspec(dllexport) void ReleaseMixer();

    //Create a new mixin object
    // Return: the mixin object handle Id, or 0 if there is any error
    extern "C" __declspec(dllexport) uint32_t CreateSpxShaderMixer();

    extern "C" __declspec(dllexport) bool ReleaseSpxShaderMixer(uint32_t mixerHandleId);

    extern "C" __declspec(dllexport) bool MixinShaders(uint32_t mixerHandleId, const char* stringShadersWithGenerics, uint32_t* shaderSpxBytecode, int32_t bytecodeSize);

    extern "C" __declspec(dllexport) bool AddComposition(uint32_t mixerHandleId, const char* shaderName, const char* variableName, uint32_t compositionMixerHandleId);

    //Functions to get the mixer's current bytecode (useful if we want to get the intermediary bytecodes, after we mix some shaders or add compositions)
    extern "C" __declspec(dllexport) uint32_t* GetMixerCurrentBytecode(uint32_t mixerHandleId, int32_t* bytecodeSize);
    extern "C" __declspec(dllexport) int32_t GetMixerCurrentBytecodeSize(uint32_t mixerHandleId);
    extern "C" __declspec(dllexport) int32_t CopyMixerCurrentBytecode(uint32_t mixerHandleId, uint32_t* bytecodeBuffer, int32_t bufferSize);

    //=====================================
    //Effect processing
    extern "C" __declspec(dllexport) uint32_t ExecuteEffectCommandLines(const char* effectCommandLine, ShaderSourceLoaderCallback shaderDependencyCallback);

    extern "C" __declspec(dllexport) bool ReleaseMixinHandle(uint32_t handleId);

    //=====================================
    //Mixer compilation (to be call after all mixin and compositions are done)
    extern "C" __declspec(dllexport) bool CompileMixer(uint32_t mixerHandleId, OutputStageEntryPoint* stageEntryPointArray, int32_t countStages);

	//Return the mixin compiled bytecode, generated when we compile the mixer
	//this bytecode can let the client retrieve Reflection data
	//The bytecode buffer is allocated on the dll side, and has to be released on the caller side
	extern "C" __declspec(dllexport) uint32_t* GetMixerCompiledBytecode(uint32_t mixerHandleId, int32_t* bytecodeSize);

	//Or we can use the following function to get the size of the compiled bytecode buffer, then to copy its data into a buffer allocated by the caller
	extern "C" __declspec(dllexport) int32_t GetMixerCompiledBytecodeSize(uint32_t mixerHandleId);
	extern "C" __declspec(dllexport) int32_t CopyMixerCompiledBytecode(uint32_t mixerHandleId, uint32_t* bytecodeBuffer, int32_t bufferSize);

    extern "C" __declspec(dllexport) int32_t GetMixerCountCompiledStages(uint32_t mixerHandleId);

    extern "C" __declspec(dllexport) uint32_t* GetMixerCompiledBytecodeForStageNum(uint32_t mixerHandleId, uint32_t stageNum, xkslang::ShadingStageEnum* stage, int32_t* bytecodeSize);
    extern "C" __declspec(dllexport) int32_t GetMixerCompiledBytecodeSizeForStageNum(uint32_t mixerHandleId, uint32_t stageNum, xkslang::ShadingStageEnum* stage);
    extern "C" __declspec(dllexport) int32_t CopyMixerCompiledBytecodeForStageNum(uint32_t mixerHandleId, uint32_t stageNum, xkslang::ShadingStageEnum* stage, uint32_t* bytecodeBuffer, int32_t bufferSize);

    //Return the compiled bytecode for the given stage
    //The bytecode buffer is allocated on the dll side, and has to be released on the caller side
    extern "C" __declspec(dllexport) uint32_t* GetMixerCompiledBytecodeForStage(uint32_t mixerHandleId, xkslang::ShadingStageEnum stage, int32_t* bytecodeSize);

    //We can use the following functions to get the size of a stage compiled bytecode buffer, then to copy its data into a buffer allocated by the caller
    extern "C" __declspec(dllexport) int32_t GetMixerCompiledBytecodeSizeForStage(uint32_t mixerHandleId, xkslang::ShadingStageEnum stage);
    extern "C" __declspec(dllexport) int32_t CopyMixerCompiledBytecodeForStage(uint32_t mixerHandleId, xkslang::ShadingStageEnum stage, uint32_t* bytecodeBuffer, int32_t bufferSize);

    extern "C" __declspec(dllexport) bool GetMixerMethodsData(uint32_t mixerHandleId, MethodData** methods, int32_t* countMethods);

	//After a mixer has been successfully compiled: call this function to get its Effect Reflection Data
	extern "C" __declspec(dllexport) bool GetMixerEffectReflectionData(uint32_t mixerHandleId,
		ConstantBufferReflectionDescriptionData** constantBuffers, int32_t* countConstantBuffers, int32_t* constantBufferStructSize, int32_t* constantBufferMemberStructSize,
		EffectResourceBindingDescriptionData** resourceBindings, int32_t* countResourceBindings, int32_t* resourceBindingsStructSize,
		ShaderInputAttributeDescriptionData** inputAttributes, int32_t* countInputAttributes, int32_t* inputAttributesStructSize);
}


#if 0
Reference code from C# app

// Copyright (c) 2014-2017 Silicon Studio Corp. All rights reserved. (https://www.siliconstudio.co.jp)
// See LICENSE.md for full license information.
using System;
using System.Collections.Generic;
using System.Collections.ObjectModel;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using SiliconStudio.Core;
using SiliconStudio.Core.Diagnostics;
using SiliconStudio.Core.IO;
using SiliconStudio.Core.Serialization.Contents;
using SiliconStudio.Core.Storage;
using SiliconStudio.Xenko.Rendering;
using SiliconStudio.Xenko.Graphics;
using SiliconStudio.Xenko.Shaders.Parser;
using SiliconStudio.Shaders.Ast;
using SiliconStudio.Shaders.Ast.Hlsl;
using SiliconStudio.Shaders.Utility;
using Encoding = System.Text.Encoding;
using LoggerResult = SiliconStudio.Core.Diagnostics.LoggerResult;
using System.Runtime.InteropServices;
using SiliconStudio.Xenko.Shaders.Parser.Mixins;

namespace SiliconStudio.Xenko.Shaders.Compiler
{
    public class XkslangDLLBindingClass
    {
        public static ShaderSourceManager ShaderSourceManager { get; set; }

        public static List<ShaderSourceManager.ShaderSourceWithHash> ListShaderSourcesLoaded { get; set; }  //everytime the Dll load a shaderSource, we add it to this list

        //=====================================================================================================================
        //callback function: called by the dll to ask the shader data
        public static StringBuilder ShaderSourceLoaderCallback(string shaderName, out Int32 dataLength)
        {
            dataLength = -1;
            if (ShaderSourceManager == null) return null;

            ShaderSourceManager.ShaderSourceWithHash source = ShaderSourceManager.LoadShaderSource(shaderName);

            source.ShaderName = shaderName;
            if (ListShaderSourcesLoaded != null) ListShaderSourcesLoaded.Add(source);

            StringBuilder shaderSource = new StringBuilder(source.Source);
            dataLength = shaderSource.Length;
            return shaderSource;
        }

        //callback function delegate
        [UnmanagedFunctionPointer(CallingConvention.StdCall)]
        public delegate StringBuilder ShaderSourceLoaderCallbackDelegate(string shaderName, out Int32 len);

        //Return the error messages after an operation failed
        [DllImport("XkslangDll.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr GetErrorMessages();

        //=====================================================================================================================
        // Parsing functions: parse and convert xksl shaders to SPX bytecode
        //=====================================================================================================================

        //Xkslang initialization: To be called before calling parsing functions
        [DllImport("XkslangDll.dll", CallingConvention = CallingConvention.Cdecl)]
        [return: MarshalAs(UnmanagedType.I1)]
        public static extern bool InitializeParser();

        //To be called when we finish parsing a file
        [DllImport("XkslangDll.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern void ReleaseParser();

        //Convert an xksl shader into SPX bytecode
        // mainShaderName: name of the main shader to convert (this shader may be depending on other shaders, resulting in parsing a whole bynch of them)
        // stringShadersWithGenerics: let us specify generic values for the shaders we're going to parse (can be null)
        //   Syntax: "shaderA<g1, g2, ...> shaderB<g1, g2, ...> ..."
        // listMacroDefinition: let us define macro definition (can be null)
        //   Syntax: "MACRO_01 "value" MACRO_02 "value" ..."
        // shaderDependencyCallback: callback function, called by xkslang everytime the parser requests data for a shader (mainShaderName at first, then all its dependencies, if any)
        // resultingBytecodeSize: pointer returning the size of the generated bytecode
        // Return:
        //  null: if the conversion failed (user can call GetErrorMessages function to get more details)
        //  pointer to the bytecode if the operation succeeded
        //   The pointer is allocated on the dll side (using LocalAlloc function), and has to be deleted by the caller (resultingBytecodeSize parameter contains the length of the bytecode buffer)
        [DllImport("XkslangDll.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr ConvertXkslShaderToSPX([MarshalAs(UnmanagedType.LPStr)] string mainShaderName,
            [MarshalAs(UnmanagedType.LPStr)] string stringShadersWithGenerics, [MarshalAs(UnmanagedType.LPStr)] string listMacroDefinition,
            [MarshalAs(UnmanagedType.FunctionPtr)] ShaderSourceLoaderCallbackDelegate callbackPointer, out Int32 bytecodeLength);

        //=====================================================================================================================
        //Utility functions to help converting a bytecode to a human-readable ascii file, or to glsl/glsl files
        //=====================================================================================================================
        [DllImport("XkslangDll.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr ConvertBytecodeToAsciiText([In] Int32[] bytecode, Int32 bytecodeSize, out Int32 bytecodeLength);

        [DllImport("XkslangDll.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr ConvertBytecodeToGlsl([In] Int32[] bytecode, Int32 bytecodeSize, out Int32 bytecodeLength);

        [DllImport("XkslangDll.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr ConvertBytecodeToHlsl([In] Int32[] bytecode, Int32 bytecodeSize, Int32 shaderModel, out Int32 bytecodeLength);

        [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi)]
        public struct BytecodeShaderInformation
        {
            public IntPtr ShaderName;
        }

        [DllImport("XkslangDll.dll", CallingConvention = CallingConvention.Cdecl)]
        [return: MarshalAs(UnmanagedType.I1)]
        public static extern bool GetBytecodeShadersInformation([In] Int32[] bytecode, Int32 bytecodeSize, out IntPtr shadersInfo, out Int32 countShaders);

        //=====================================================================================================================
        // Mixin functions: Mix SPX shaders to generate SPV bytecode for specific output stages
        //=====================================================================================================================

        //SpxMixer initialization: To be called before starting mixin a new effect
        [DllImport("XkslangDll.dll", CallingConvention = CallingConvention.Cdecl)]
        [return: MarshalAs(UnmanagedType.I1)]
        public static extern bool InitializeMixer();

        //To be called when we finish mixin an effect
        [DllImport("XkslangDll.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern void ReleaseMixer();

        //Create a new SPX shader mixer, return the mixer handle Id
        [DllImport("XkslangDll.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern UInt32 CreateSpxShaderMixer();

        [DllImport("XkslangDll.dll", CallingConvention = CallingConvention.Cdecl)]
        [return: MarshalAs(UnmanagedType.I1)]
        public static extern bool ReleaseSpxShaderMixer(UInt32 mixerHandleId);

        [DllImport("XkslangDll.dll", CallingConvention = CallingConvention.Cdecl)]
        [return: MarshalAs(UnmanagedType.I1)]
        public static extern bool ReleaseMixinHandle(UInt32 mixerHandleId);

        //=====================================================================================================================
        //Mixin a shader into the current mixer
        [DllImport("XkslangDll.dll", CallingConvention = CallingConvention.Cdecl)]
        [return: MarshalAs(UnmanagedType.I1)]
        public static extern bool MixinShaders(UInt32 mixerHandleId, [MarshalAs(UnmanagedType.LPStr)] string shadersName, [In] Int32[] bytecode, Int32 bytecodeSize);

        //Add a composition to a mixin
        [DllImport("XkslangDll.dll", CallingConvention = CallingConvention.Cdecl)]
        [return: MarshalAs(UnmanagedType.I1)]
        public static extern bool AddComposition(UInt32 mixerHandleId, [MarshalAs(UnmanagedType.LPStr)] string shaderName, [MarshalAs(UnmanagedType.LPStr)] string variableName,
            UInt32 compositionMixerHandleId);

        [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi)]
        public struct OutputStageEntryPoint
        {
            public ShadingStageEnum stage;
            public String entryPointName;

            public OutputStageEntryPoint(ShadingStageEnum stage, String entryPointName)
            {
                this.stage = stage;
                this.entryPointName = entryPointName;
            }
        }

        //=====================================
        //Process an Xkfx effect
        [DllImport("XkslangDll.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern UInt32 ExecuteEffectCommandLines([MarshalAs(UnmanagedType.LPStr)] string effectCommandLine,
            [MarshalAs(UnmanagedType.FunctionPtr)] ShaderSourceLoaderCallbackDelegate callbackPointer);

        //=====================================================================================================================
        //Compiled the mixer (to be called after all mixin and compositions have been set)
        [DllImport("XkslangDll.dll", CallingConvention = CallingConvention.Cdecl)]
        [return: MarshalAs(UnmanagedType.I1)]
        public static extern bool CompileMixer(UInt32 mixerHandleId, [In, Out] OutputStageEntryPoint[] stageEntryPointArray, Int32 countStages);

        //=====================================================================================================================
        //Get the mixer compiled bytecode (common to all output stages)
        //This compiled bytecode can be used to retrieve the Effect Reflection data
        [DllImport("XkslangDll.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr GetMixerCompiledBytecode(UInt32 mixerHandleId, out Int32 bytecodeLength);

        //Get the mixer compiled bytecode's size
        [DllImport("XkslangDll.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern Int32 GetMixerCompiledBytecodeSize(UInt32 mixerHandleId);

        //Ask the mixer to copy the compiled bytecode into the buffer
        [DllImport("XkslangDll.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern Int32 CopyMixerCompiledBytecode(UInt32 mixerHandleId, [In] Int32[] bytecodeBuffer, Int32 bufferSize);

        //=====================================================================================================================
        //Get an output stage compiled bytecode
        //=====================================================================================================================
        [DllImport("XkslangDll.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr GetMixerCompiledBytecodeForStage(UInt32 mixerHandleId, ShadingStageEnum stage, out Int32 bytecodeLength);

        //Get an output stage compiled bytecode's size
        [DllImport("XkslangDll.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern Int32 GetMixerCompiledBytecodeSizeForStage(UInt32 mixerHandleId, ShadingStageEnum stage);

        //Ask the mixer to copy a stage compiled bytecode into the buffer
        [DllImport("XkslangDll.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern Int32 CopyMixerCompiledBytecodeForStage(UInt32 mixerHandleId, ShadingStageEnum stage, [In] Int32[] bytecodeBuffer, Int32 bufferSize);

        [DllImport("XkslangDll.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern Int32 GetMixerCountCompiledStages(UInt32 mixerHandleId);

        [DllImport("XkslangDll.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr GetMixerCompiledBytecodeForStageNum(UInt32 mixerHandleId, UInt32 stageNum, out ShadingStageEnum stage, out Int32 bytecodeLength);

        [DllImport("XkslangDll.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern Int32 GetMixerCompiledBytecodeSizeForStageNum(UInt32 mixerHandleId, UInt32 stageNum, out ShadingStageEnum stage);

        [DllImport("XkslangDll.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern Int32 CopyMixerCompiledBytecodeForStageNum(UInt32 mixerHandleId, UInt32 stageNum, out ShadingStageEnum stage, [In] Int32[] bytecodeBuffer, Int32 bufferSize);

        //=====================================================================================================================
        //Get the reflection data
        //=====================================================================================================================

        //ConstantBuffer member struct
        [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi)]
        public struct ConstantBufferMemberReflectionDescriptionData
        {
            public Int32 Offset;
            public IntPtr KeyName;
            public IntPtr RawName;
            public IntPtr LogicalGroup;

            public EffectParameterReflectionClassEnum Class;
            public EffectParameterReflectionTypeEnum Type;
            public Int32 RowCount;
            public Int32 ColumnCount;
            public Int32 ArrayElements;
            public Int32 Size;
            public Int32 Alignment;
            public Int32 ArrayStride;
            public Int32 MatrixStride;

            public Int32 CountMembers;
            public IntPtr StructMembers;
        }

        //ConstantBuffer struct
        [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi)]
        public struct ConstantBufferReflectionDescriptionData
        {
            public Int32 Size;
            public Int32 CountMembers;
            public IntPtr CbufferName;
            public IntPtr Members;
        }

        //ResourceBinding struct
        [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi)]
        public struct EffectResourceBindingDescriptionData
        {
            public ShadingStageEnum Stage;
            public EffectParameterReflectionClassEnum Class;
            public EffectParameterReflectionTypeEnum Type;
            public IntPtr KeyName;
            public IntPtr RawName;
            public IntPtr ResourceGroupName;

            //[MarshalAs(UnmanagedType.LPStr)]
            //public string KeyName;
        }

        //Input Attribute struct
        [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi)]
        public struct ShaderInputAttributeDescriptionData
        {
            public IntPtr SemanticName;
            public Int32 SemanticIndex;
        }

        [DllImport("XkslangDll.dll", CallingConvention = CallingConvention.Cdecl)]
        [return: MarshalAs(UnmanagedType.I1)]
        public static extern bool GetMixerEffectReflectionData(UInt32 mixerHandleId,
            out IntPtr constantBuffersData, out Int32 countConstantBuffers, out Int32 constantBufferStructSize, out Int32 constantBufferMemberStructSize,
            out IntPtr resourceBindingsData, out Int32 countResourceBindings, out Int32 resourceBindingsStructSize,
            out IntPtr inputAttributesData, out Int32 countInputAttributes, out Int32 inputAttributesStructSize);

        //=====================================================================================================================
        // Enums: these enums are similar to xkslang enums, and set to 32bits
        // Note: maybe we could directly use Xenko enums and avoid a remapping
        //  however this could easily bring conflicts / errors in we update Xenko enums without updating Xkslang enums
        //=====================================================================================================================
        public enum ShadingStageEnum : Int32
        {
            Undefined = -1,
            Vertex = 0,
            Pixel = 1,
            TessControl = 2,
            TessEvaluation = 3,
            Geometry = 4,
            Compute = 5,
        };

        public enum EffectParameterReflectionClassEnum : Int32
        {
            Undefined = -1,
            Scalar = 0,
            Vector = 1,
            MatrixRows = 2,
            MatrixColumns = 3,
            Object = 4,
            Struct = 5,
            InterfaceClass = 6,
            InterfacePointer = 7,
            Sampler = 8,
            ShaderResourceView = 9,
            ConstantBuffer = 10,
            TextureBuffer = 11,
            UnorderedAccessView = 12,
            Color = 13,
        };

        public enum EffectParameterReflectionTypeEnum : Int32
        {
            Undefined = -1,
            Void = 0,
            Bool = 1,
            Int = 2,
            Float = 3,
            String = 4,
            Texture = 5,
            Texture1D = 6,
            Texture2D = 7,
            Texture3D = 8,
            TextureCube = 9,
            Sampler = 10,
            Sampler1D = 11,
            Sampler2D = 12,
            Sampler3D = 13,
            SamplerCube = 14,
            UInt = 19,
            UInt8 = 20,
            Buffer = 25,
            ConstantBuffer = 26,
            TextureBuffer = 27,
            Texture1DArray = 28,
            Texture2DArray = 29,
            Texture2DMultisampled = 32,
            Texture2DMultisampledArray = 33,
            TextureCubeArray = 34,
            Double = 39,
            RWTexture1D = 40,
            RWTexture1DArray = 41,
            RWTexture2D = 42,
            RWTexture2DArray = 43,
            RWTexture3D = 44,
            RWBuffer = 45,
            ByteAddressBuffer = 46,
            RWByteAddressBuffer = 47,
            StructuredBuffer = 48,
            RWStructuredBuffer = 49,
            AppendStructuredBuffer = 50,
            ConsumeStructuredBuffer = 51,
        };

        //=====================================================================================================================
        //Enum Conversion functions
        //=====================================================================================================================
        public static ShaderStage ConvertShadingStageEnum(ShadingStageEnum stage)
        {
            switch (stage)
            {
                case ShadingStageEnum.Vertex: return ShaderStage.Vertex;
                case ShadingStageEnum.TessControl: return ShaderStage.Hull;
                case ShadingStageEnum.TessEvaluation: return ShaderStage.Domain;
                case ShadingStageEnum.Geometry: return ShaderStage.Geometry;
                case ShadingStageEnum.Pixel: return ShaderStage.Pixel;
                case ShadingStageEnum.Compute: return ShaderStage.Compute;
            }

            return ShaderStage.None;
        }

        public static EffectParameterClass ConvertEffectParameterReflectionClassEnum(EffectParameterReflectionClassEnum c)
        {
            switch (c)
            {
                case EffectParameterReflectionClassEnum.Scalar: return EffectParameterClass.Scalar;
                case EffectParameterReflectionClassEnum.Vector: return EffectParameterClass.Vector;
                case EffectParameterReflectionClassEnum.MatrixRows: return EffectParameterClass.MatrixRows;
                case EffectParameterReflectionClassEnum.MatrixColumns: return EffectParameterClass.MatrixColumns;
                case EffectParameterReflectionClassEnum.Object: return EffectParameterClass.Object;
                case EffectParameterReflectionClassEnum.Struct: return EffectParameterClass.Struct;
                case EffectParameterReflectionClassEnum.InterfaceClass: return EffectParameterClass.InterfaceClass;
                case EffectParameterReflectionClassEnum.InterfacePointer: return EffectParameterClass.InterfacePointer;
                case EffectParameterReflectionClassEnum.Sampler: return EffectParameterClass.Sampler;
                case EffectParameterReflectionClassEnum.ShaderResourceView: return EffectParameterClass.ShaderResourceView;
                case EffectParameterReflectionClassEnum.ConstantBuffer: return EffectParameterClass.ConstantBuffer;
                case EffectParameterReflectionClassEnum.TextureBuffer: return EffectParameterClass.TextureBuffer;
                case EffectParameterReflectionClassEnum.UnorderedAccessView: return EffectParameterClass.UnorderedAccessView;
                case EffectParameterReflectionClassEnum.Color: return EffectParameterClass.Color;
            }

            throw new Exception("Unknown EffectParameterReflectionClassEnum: " + c);
        }

        public static EffectParameterType ConvertEffectParameterReflectionTypeEnum(EffectParameterReflectionTypeEnum t)
        {
            switch (t)
            {
                case EffectParameterReflectionTypeEnum.Void: return EffectParameterType.Void;
                case EffectParameterReflectionTypeEnum.Bool: return EffectParameterType.Bool;
                case EffectParameterReflectionTypeEnum.Int: return EffectParameterType.Int;
                case EffectParameterReflectionTypeEnum.Float: return EffectParameterType.Float;
                case EffectParameterReflectionTypeEnum.String: return EffectParameterType.String;
                case EffectParameterReflectionTypeEnum.Texture: return EffectParameterType.Texture;
                case EffectParameterReflectionTypeEnum.Texture1D: return EffectParameterType.Texture1D;
                case EffectParameterReflectionTypeEnum.Texture2D: return EffectParameterType.Texture2D;
                case EffectParameterReflectionTypeEnum.Texture3D: return EffectParameterType.Texture3D;
                case EffectParameterReflectionTypeEnum.TextureCube: return EffectParameterType.TextureCube;
                case EffectParameterReflectionTypeEnum.Sampler: return EffectParameterType.Sampler;
                case EffectParameterReflectionTypeEnum.Sampler1D: return EffectParameterType.Sampler1D;
                case EffectParameterReflectionTypeEnum.Sampler2D: return EffectParameterType.Sampler2D;
                case EffectParameterReflectionTypeEnum.Sampler3D: return EffectParameterType.Sampler3D;
                case EffectParameterReflectionTypeEnum.SamplerCube: return EffectParameterType.SamplerCube;
                case EffectParameterReflectionTypeEnum.UInt: return EffectParameterType.UInt;
                case EffectParameterReflectionTypeEnum.UInt8: return EffectParameterType.UInt8;
                case EffectParameterReflectionTypeEnum.Buffer: return EffectParameterType.Buffer;
                case EffectParameterReflectionTypeEnum.ConstantBuffer: return EffectParameterType.ConstantBuffer;
                case EffectParameterReflectionTypeEnum.TextureBuffer: return EffectParameterType.TextureBuffer;
                case EffectParameterReflectionTypeEnum.Texture1DArray: return EffectParameterType.Texture1DArray;
                case EffectParameterReflectionTypeEnum.Texture2DArray: return EffectParameterType.Texture2DArray;
                case EffectParameterReflectionTypeEnum.Texture2DMultisampled: return EffectParameterType.Texture2DMultisampled;
                case EffectParameterReflectionTypeEnum.Texture2DMultisampledArray: return EffectParameterType.Texture2DMultisampledArray;
                case EffectParameterReflectionTypeEnum.TextureCubeArray: return EffectParameterType.TextureCubeArray;
                case EffectParameterReflectionTypeEnum.Double: return EffectParameterType.Double;
                case EffectParameterReflectionTypeEnum.RWTexture1D: return EffectParameterType.RWTexture1D;
                case EffectParameterReflectionTypeEnum.RWTexture1DArray: return EffectParameterType.RWTexture1DArray;
                case EffectParameterReflectionTypeEnum.RWTexture2D: return EffectParameterType.RWTexture2D;
                case EffectParameterReflectionTypeEnum.RWTexture2DArray: return EffectParameterType.RWTexture2DArray;
                case EffectParameterReflectionTypeEnum.RWTexture3D: return EffectParameterType.RWTexture3D;
                case EffectParameterReflectionTypeEnum.RWBuffer: return EffectParameterType.RWBuffer;
                case EffectParameterReflectionTypeEnum.ByteAddressBuffer: return EffectParameterType.ByteAddressBuffer;
                case EffectParameterReflectionTypeEnum.RWByteAddressBuffer: return EffectParameterType.RWByteAddressBuffer;
                case EffectParameterReflectionTypeEnum.StructuredBuffer: return EffectParameterType.StructuredBuffer;
                case EffectParameterReflectionTypeEnum.RWStructuredBuffer: return EffectParameterType.RWStructuredBuffer;
                case EffectParameterReflectionTypeEnum.AppendStructuredBuffer: return EffectParameterType.AppendStructuredBuffer;
                case EffectParameterReflectionTypeEnum.ConsumeStructuredBuffer: return EffectParameterType.ConsumeStructuredBuffer;
            }

            throw new Exception("Unknown EffectParameterReflectionTypeEnum: " + t);
        }

        //=====================================================================================================================
        //ShaderMixinSource to Xkfx cmd lines Conversion functions
        //=====================================================================================================================
        public static string ConvertShaderSourceToXkfxCommandLine(ShaderSource shaderSource)
        {
            if (shaderSource is ShaderMixinSource)
            {
                var shaderMixinSource = shaderSource as ShaderMixinSource;
                StringBuilder shaderSourceBuilder = new StringBuilder();

                int countMixins = shaderMixinSource.Mixins.Count;
                if (countMixins > 0)
                {
                    if (countMixins > 1) shaderSourceBuilder.Append("mixin({");
                    else shaderSourceBuilder.Append("mixin(");
                    for (int i = 0; i < countMixins; i++)
                    {
                        ShaderClassSource mixin = shaderMixinSource.Mixins.ElementAt(i);
                        shaderSourceBuilder.Append(mixin.ToClassName());
                        if (i < countMixins - 1) shaderSourceBuilder.Append(", ");
                    }
                    if (countMixins > 1) shaderSourceBuilder.Append("}");

                    int countCompositions = shaderMixinSource.Compositions.Count;
                    if (countCompositions > 0)
                    {
                        shaderSourceBuilder.Append("[");
                        for (int i = 0; i < countCompositions; i++)
                        {
                            string compositionTargetName = shaderMixinSource.Compositions.ElementAt(i).Key;
                            var compositionTargetMixin = shaderMixinSource.Compositions.ElementAt(i).Value;

                            shaderSourceBuilder.Append(compositionTargetName + "=");

                            string compositionString = ConvertShaderSourceToXkfxCommandLine(compositionTargetMixin);
                            if (compositionString == null) return null;
                            if (compositionTargetMixin is ShaderArraySource)
                                shaderSourceBuilder.Append("[" + compositionString + "]");
                            else
                                shaderSourceBuilder.Append(compositionString);

                            if (countCompositions > 1)
                            {
                                if (i < countCompositions - 1) shaderSourceBuilder.Append(", ");
                            }
                        }
                        shaderSourceBuilder.Append("]");
                    }

                    shaderSourceBuilder.Append(")");
                }
                return shaderSourceBuilder.ToString();
            }

            if (shaderSource is ShaderClassSource)
            {
                var shaderClassSource = shaderSource as ShaderClassSource;
                StringBuilder shaderSourceBuilder = new StringBuilder();

                shaderSourceBuilder.Append("mixin(");
                shaderSourceBuilder.Append(shaderClassSource.ToClassName());
                shaderSourceBuilder.Append(")");

                return shaderSourceBuilder.ToString();
            }

            if (shaderSource is ShaderArraySource)
            {
                var shaderArraySource = shaderSource as ShaderArraySource;
                StringBuilder shaderSourceBuilder = new StringBuilder();

                int countShaders = shaderArraySource.Values.Count;
                for (int i = 0; i < countShaders; i++)
                {
                    var shader = shaderArraySource.Values.ElementAt(i);

                    string shaderCommandLine = ConvertShaderSourceToXkfxCommandLine(shader);
                    if (shaderCommandLine == null) return null;

                    shaderSourceBuilder.Append(shaderCommandLine);

                    if (countShaders > 1)
                    {
                        if (i < countShaders - 1) shaderSourceBuilder.Append(", ");
                    }
                }

                return shaderSourceBuilder.ToString();
            }

            return null;
        }

        public static string ConvertShaderMixinSourceToXkfxCommandLines(ShaderMixinSource mixinTree)
        {
            StringBuilder xkfxCommandLinesBuilder = new StringBuilder();
            xkfxCommandLinesBuilder.AppendLine("//" + mixinTree.Name);

            //Add the macros
            foreach (var macro in mixinTree.Macros)
            {
                xkfxCommandLinesBuilder.AppendLine("setDefine " + macro.Name + " \"" + macro.Definition + "\"");
            }

            //main mixer
            xkfxCommandLinesBuilder.AppendLine("mixer _m");

            //Add the mixins
            xkfxCommandLinesBuilder.Append("_m.mixin( ");
            int countMixins = mixinTree.Mixins.Count;
            for (int i = 0; i < countMixins; i++)
            {
                xkfxCommandLinesBuilder.Append(mixinTree.Mixins[i].ToClassName() + ((i == countMixins - 1) ? " " : ", "));
            }
            xkfxCommandLinesBuilder.AppendLine(")");
            xkfxCommandLinesBuilder.AppendLine("");

            //Add the compositions
            foreach (var composition in mixinTree.Compositions)
            {
                string compositionCommandLine = ConvertShaderSourceToXkfxCommandLine(composition.Value);
                if (compositionCommandLine == null) return null;

                if (composition.Value is ShaderArraySource)
                    compositionCommandLine = "[" + compositionCommandLine + "]";

                string compositionString = "_m.addComposition( " + composition.Key + " = " + compositionCommandLine + " )";
                xkfxCommandLinesBuilder.AppendLine(compositionString);
            }

            //instruction to compile the mixer
            xkfxCommandLinesBuilder.AppendLine("");
            xkfxCommandLinesBuilder.AppendLine("_m.compile()");

            string xkfxCommandLines = xkfxCommandLinesBuilder.ToString();
            return xkfxCommandLines;
        }
    }

    public class ShaderSpvBytecode
    {
        public ShaderStage Stage;

        public Int32[] CompiledSpvBytecode = null;

        public string BytecodeAsciiFormat = null;
        public string BytecodeHlslConversion = null;
    }

    /// <summary>
    /// An <see cref="IEffectCompiler"/> which will compile effect into multiple shader code, and compile them with a <see cref="IShaderCompiler"/>.
    /// </summary>
    public class EffectCompiler : EffectCompilerBase
    {
        private bool d3dCompilerLoaded = false;
        private bool xkslangDllLoaded = false;
        private static readonly Object WriterLock = new Object();
        private static readonly Object XkslangMixinLock = new Object();

        private ShaderMixinParser shaderMixinParser;

        private readonly object shaderMixinParserLock = new object();

        public List<string> SourceDirectories { get; private set; }

        public Dictionary<string, string> UrlToFilePath { get; private set; }

        public override IVirtualFileProvider FileProvider { get; set; }
        public bool UseFileSystem { get; set; }

        public EffectCompiler()
        {
            NativeLibrary.PreloadLibrary("d3dcompiler_47.dll");
            SourceDirectories = new List<string>();
            UrlToFilePath = new Dictionary<string, string>();
        }

        public override ObjectId GetShaderSourceHash(string type)
        {
            return GetMixinParser().SourceManager.GetShaderSourceHash(type);
        }

        /// <summary>
        /// Remove cached files for modified shaders
        /// </summary>
        /// <param name="modifiedShaders"></param>
        public override void ResetCache(HashSet<string> modifiedShaders)
        {
            GetMixinParser().DeleteObsoleteCache(modifiedShaders);
        }

        private ShaderMixinParser GetMixinParser()
        {
            lock (shaderMixinParserLock)
            {
                // Generate the AST from the mixin description
                if (shaderMixinParser == null)
                {
                    shaderMixinParser = new ShaderMixinParser(FileProvider ?? ContentManager.FileProvider);
                    shaderMixinParser.SourceManager.LookupDirectoryList.AddRange(SourceDirectories); // TODO: temp
                    shaderMixinParser.SourceManager.UseFileSystem = UseFileSystem;
                    shaderMixinParser.SourceManager.UrlToFilePath = UrlToFilePath; // TODO: temp
                }
                return shaderMixinParser;
            }
        }

        public bool ConvertAndReleaseDllStructMemberDataToReflectionStructMemberType(
            ref XkslangDLLBindingClass.ConstantBufferMemberReflectionDescriptionData structMemberSrc,
            ref EffectTypeMemberDescription structMemberDst)
        {
            if (structMemberSrc.KeyName == null) return false;

            string structMemberName = Marshal.PtrToStringAnsi(structMemberSrc.KeyName);
            structMemberDst.Name = structMemberName;
            structMemberDst.Offset = structMemberSrc.Offset;
            structMemberDst.Type = new EffectTypeDescription()
            {
                Class = XkslangDLLBindingClass.ConvertEffectParameterReflectionClassEnum(structMemberSrc.Class),
                Type = XkslangDLLBindingClass.ConvertEffectParameterReflectionTypeEnum(structMemberSrc.Type),
                RowCount = structMemberSrc.RowCount,
                ColumnCount = structMemberSrc.ColumnCount,
                Elements = structMemberSrc.ArrayElements,
                ElementSize = structMemberSrc.Size,
                Name = structMemberName,
                Members = null,
            };

            if (structMemberSrc.KeyName == null) { Marshal.FreeHGlobal(structMemberSrc.KeyName); }
            if (structMemberSrc.RawName == null) { Marshal.FreeHGlobal(structMemberSrc.RawName); }

            if (structMemberSrc.CountMembers > 0)
            {
                if (structMemberSrc.StructMembers != null)
                {
                    EffectTypeMemberDescription[] structmemberSubMembers = new EffectTypeMemberDescription[structMemberSrc.CountMembers];

                    //Set the cbuffer member's struct members
                    long memberStructSize = Marshal.SizeOf(typeof(XkslangDLLBindingClass.ConstantBufferMemberReflectionDescriptionData));
                    for (int im = 0; im < structMemberSrc.CountMembers; im++)
                    {
                        XkslangDLLBindingClass.ConstantBufferMemberReflectionDescriptionData structMemberSubMemberSrc;
                        structMemberSubMemberSrc = (XkslangDLLBindingClass.ConstantBufferMemberReflectionDescriptionData)Marshal.PtrToStructure(
                            new IntPtr(structMemberSrc.StructMembers.ToInt64() + (memberStructSize * im)),
                            typeof(XkslangDLLBindingClass.ConstantBufferMemberReflectionDescriptionData));

                        EffectTypeMemberDescription structMemberSubMemberDst = new EffectTypeMemberDescription();

                        if (!ConvertAndReleaseDllStructMemberDataToReflectionStructMemberType(ref structMemberSubMemberSrc, ref structMemberSubMemberDst))
                            throw new Exception("Failed to convert the EffectReflection cbuffer struct member data");

                        structmemberSubMembers[im] = structMemberSubMemberDst;
                    }

                    structMemberDst.Type.Members = structmemberSubMembers;
                    Marshal.FreeHGlobal(structMemberSrc.StructMembers);
                }
                else throw new Exception("A cbuffer struct member is missing Reflection data about its members");
            }
            return true;
        }

        public TaskOrResult<EffectBytecodeCompilerResult> CompileEffectWithXkslangApi(ShaderMixinSource mixinTree, EffectCompilerParameters effectParameters, CompilerParameters compilerParameters)
        {
            var log = new LoggerResult();
            var fullEffectName = mixinTree.Name;

            EffectReflection xkslangEffectReflection = new EffectReflection();
            List<ShaderSpvBytecode> shaderStageSpvBytecodes = new List<ShaderSpvBytecode>();
            HashSourceCollection hashSources = null;
            int hlslShaderModel = 40;
            string mixinCompiledBytecode_AsciiText;
            string xkfxSourceHash = "";

#if SILICONSTUDIO_PLATFORM_WINDOWS_DESKTOP
            var logDir = Path.Combine(PlatformFolders.ApplicationBinaryDirectory, "log");
            if (!Directory.Exists(logDir))
            {
                Directory.CreateDirectory(logDir);
            }
#endif

            //Parsing and mixin using XkslangDll is not threadsafe for now
            lock (XkslangMixinLock)
            {
                if (Platform.IsWindowsDesktop && !xkslangDllLoaded)
                {
                    NativeLibrary.PreloadLibrary("XkslangDll.dll");
                    xkslangDllLoaded = true;
                }

                UInt32 effectHandleId = 0;
                Int32[] mixinCompiledBytecode = null;

                //===================================================================
                //Build the effect command lines
                string xkfxCommandLines = XkslangDLLBindingClass.ConvertShaderMixinSourceToXkfxCommandLines(mixinTree);
                if (xkfxCommandLines == null) throw new Exception("Failed to convert the mixin source to XKFX command lines");

#if SILICONSTUDIO_PLATFORM_WINDOWS_DESKTOP
                xkfxSourceHash = ObjectId.FromBytes(Encoding.UTF8.GetBytes(xkfxCommandLines)).ToString();

                //write the effect's command line on the disk
                {
                    var effectFilename = Path.Combine(logDir, "effect_" + mixinTree.Name.Replace('.', '_') + "_" + xkfxSourceHash + ".xkfx");
                    lock (WriterLock) // protect write in case the same shader is created twice
                    {
                        //if (!File.Exists(effectFilename))
                        {
                            File.WriteAllText(effectFilename, xkfxCommandLines);
                        }
                    }
                }
#endif

                //===================================================================
                //Execute the effect
                try
                {
                    XkslangDLLBindingClass.ShaderSourceManager = GetMixinParser().SourceManager;
                    XkslangDLLBindingClass.ListShaderSourcesLoaded = new List<ShaderSourceManager.ShaderSourceWithHash>();

                    bool success = XkslangDLLBindingClass.InitializeParser();
                    if (!success) throw new Exception("Error initializing Xkslang");

                    //execute the effect command line
                    {
                        effectHandleId = XkslangDLLBindingClass.ExecuteEffectCommandLines(xkfxCommandLines, XkslangDLLBindingClass.ShaderSourceLoaderCallback);
                        if (effectHandleId == 0) throw new Exception("Failed to execute the effect command line for effect: " + fullEffectName);
                    }

                    //=====================================================================================================================================
                    //get the effect compiled bytecode
                    {
                        int bytecodeLength = XkslangDLLBindingClass.GetMixerCompiledBytecodeSize(effectHandleId);
                        if (bytecodeLength <= 0) throw new Exception("Failed to get the mixer compiled bytecode size");
                        mixinCompiledBytecode = new Int32[bytecodeLength];
                        int aLen = XkslangDLLBindingClass.CopyMixerCompiledBytecode(effectHandleId, mixinCompiledBytecode, mixinCompiledBytecode.Length);
                        if (aLen != bytecodeLength) throw new Exception("Failed to get the mixer compiled bytecode");

                        //Optionnal: convert the bytecode to human readable ascii text
                        {
                            int asciiBufferLength = 0;
                            IntPtr pAsciiBytecodeBuffer = XkslangDLLBindingClass.ConvertBytecodeToAsciiText(mixinCompiledBytecode, mixinCompiledBytecode.Length, out asciiBufferLength);
                            if (pAsciiBytecodeBuffer == IntPtr.Zero || asciiBufferLength <= 0) throw new Exception("Failed to convert the bytecode to Ascii");

                            Byte[] asciiByteArray = new Byte[asciiBufferLength];
                            Marshal.Copy(pAsciiBytecodeBuffer, asciiByteArray, 0, asciiBufferLength);
                            Marshal.FreeHGlobal(pAsciiBytecodeBuffer);
                            mixinCompiledBytecode_AsciiText = System.Text.Encoding.UTF8.GetString(asciiByteArray);
                        }
                    }

                    //=====================================================================================================================================
                    //Get the compiled bytecode for all output stages
                    Int32 countStages = XkslangDLLBindingClass.GetMixerCountCompiledStages(effectHandleId);
                    for (UInt32 stageNum = 0; stageNum < countStages; ++stageNum)
                    {
                        ShaderSpvBytecode stageShaderBytecode = new ShaderSpvBytecode();

                        XkslangDLLBindingClass.ShadingStageEnum stage;
                        int bytecodeLength = 0;
                        bytecodeLength = XkslangDLLBindingClass.GetMixerCompiledBytecodeSizeForStageNum(effectHandleId, stageNum, out stage);
                        if (bytecodeLength <= 0) throw new Exception("Failed to get the bytecode size for VS");
                        Int32[] stageCompiledBytecode = new Int32[bytecodeLength];
                        int aLen = XkslangDLLBindingClass.CopyMixerCompiledBytecodeForStageNum(effectHandleId, stageNum, out stage, stageCompiledBytecode, stageCompiledBytecode.Length);
                        if (aLen != bytecodeLength) throw new Exception("Failed to get the bytecode for VS");

                        stageShaderBytecode.Stage = XkslangDLLBindingClass.ConvertShadingStageEnum(stage);
                        stageShaderBytecode.CompiledSpvBytecode = stageCompiledBytecode;
                        shaderStageSpvBytecodes.Add(stageShaderBytecode);
                    }

                    //Get the list of hashSources for the loaded shader
                    if (XkslangDLLBindingClass.ListShaderSourcesLoaded != null && XkslangDLLBindingClass.ListShaderSourcesLoaded.Count > 0)
                    {
                        hashSources = new HashSourceCollection();
                        foreach (var shaderLoaded in XkslangDLLBindingClass.ListShaderSourcesLoaded)
                        {
                            hashSources[shaderLoaded.ShaderName] = shaderLoaded.Hash;
                        }
                    }

                    //Query and build the EffectReflection data
                    {
                        int countConstantBuffers = 0;
                        IntPtr pAllocsConstantBuffers = IntPtr.Zero;
                        int countResourceBindings = 0;
                        IntPtr pAllocsResourceBindings = IntPtr.Zero;
                        int countInputAttributes = 0;
                        IntPtr pAllocsInputAttributes = IntPtr.Zero;
                        int constantBufferStructSize = 0, constantBufferMemberStructSize = 0, resourceBindingsStructSize = 0, inputAttributesStructSize = 0;

                        success = XkslangDLLBindingClass.GetMixerEffectReflectionData(effectHandleId,
                            out pAllocsConstantBuffers, out countConstantBuffers, out constantBufferStructSize, out constantBufferMemberStructSize,
                            out pAllocsResourceBindings, out countResourceBindings, out resourceBindingsStructSize,
                            out pAllocsInputAttributes, out countInputAttributes, out inputAttributesStructSize);
                        if (!success) throw new Exception("Failed to get the Effect Reflection data");

                        //Process the ResourceBindings
                        if (countResourceBindings > 0 && pAllocsResourceBindings != IntPtr.Zero)
                        {
                            long structSize = Marshal.SizeOf(typeof(XkslangDLLBindingClass.EffectResourceBindingDescriptionData));
                            if (structSize != resourceBindingsStructSize) throw new Exception("Incompatible data structure for EffectReflection ResourceBinding object");

                            XkslangDLLBindingClass.EffectResourceBindingDescriptionData effectResourceBinding;
                            for (int i = 0; i < countResourceBindings; i++)
                            {
                                effectResourceBinding = (XkslangDLLBindingClass.EffectResourceBindingDescriptionData)Marshal.PtrToStructure(
                                    new IntPtr(pAllocsResourceBindings.ToInt64() + (structSize * i)), typeof(XkslangDLLBindingClass.EffectResourceBindingDescriptionData));

                                string keyName = Marshal.PtrToStringAnsi(effectResourceBinding.KeyName);
                                string rawName = Marshal.PtrToStringAnsi(effectResourceBinding.RawName);
                                string resourceGroupName = Marshal.PtrToStringAnsi(effectResourceBinding.ResourceGroupName);
                                var binding = new EffectResourceBindingDescription()
                                {
                                    Stage = ShaderStage.None, //XkslangDLLBindingClass.ConvertShadingStageEnum(effectResourceBinding.Stage), (this will be set by ShaderCompiler)
                                    Class = XkslangDLLBindingClass.ConvertEffectParameterReflectionClassEnum(effectResourceBinding.Class),
                                    Type = XkslangDLLBindingClass.ConvertEffectParameterReflectionTypeEnum(effectResourceBinding.Type),
                                    RawName = rawName,
                                    ResourceGroup = resourceGroupName == null ? rawName : resourceGroupName,
                                    KeyInfo =
                                        {
                                            KeyName = keyName,
                                        },
                                };
                                xkslangEffectReflection.ResourceBindings.Add(binding);

                                if (effectResourceBinding.KeyName != null) Marshal.FreeHGlobal(effectResourceBinding.KeyName);
                                if (effectResourceBinding.RawName != null) Marshal.FreeHGlobal(effectResourceBinding.RawName);
                                if (effectResourceBinding.ResourceGroupName != null) Marshal.FreeHGlobal(effectResourceBinding.ResourceGroupName);
                            }

                            //delete the data allocated on the native code
                            Marshal.FreeHGlobal(pAllocsResourceBindings);
                        }

                        //Process the InputAttributes
                        if (countInputAttributes > 0 && pAllocsInputAttributes != IntPtr.Zero)
                        {
                            long structSize = Marshal.SizeOf(typeof(XkslangDLLBindingClass.ShaderInputAttributeDescriptionData));
                            if (structSize != inputAttributesStructSize) throw new Exception("Incompatible data structure for EffectReflection InputAttribute object");

                            XkslangDLLBindingClass.ShaderInputAttributeDescriptionData shaderInputAttribute;
                            for (int i = 0; i < countInputAttributes; i++)
                            {
                                shaderInputAttribute = (XkslangDLLBindingClass.ShaderInputAttributeDescriptionData)Marshal.PtrToStructure(
                                    new IntPtr(pAllocsInputAttributes.ToInt64() + (structSize * i)), typeof(XkslangDLLBindingClass.ShaderInputAttributeDescriptionData));

                                var inputAttribute = new ShaderInputAttributeDescription()
                                {
                                    SemanticName = Marshal.PtrToStringAnsi(shaderInputAttribute.SemanticName),
                                    SemanticIndex = shaderInputAttribute.SemanticIndex,
                                };
                                xkslangEffectReflection.InputAttributes.Add(inputAttribute);

                                Marshal.FreeHGlobal(shaderInputAttribute.SemanticName);
                            }

                            //delete the data allocated on the native code
                            Marshal.FreeHGlobal(pAllocsInputAttributes);
                        }

                        //Process the ConstantBuffers
                        if (countConstantBuffers > 0 && pAllocsConstantBuffers != IntPtr.Zero)
                        {
                            long structSize = Marshal.SizeOf(typeof(XkslangDLLBindingClass.ConstantBufferReflectionDescriptionData));
                            if (structSize != constantBufferStructSize) throw new Exception("Incompatible data structure for EffectReflection ConstantBuffer object");
                            long memberStructSize = Marshal.SizeOf(typeof(XkslangDLLBindingClass.ConstantBufferMemberReflectionDescriptionData));
                            if (memberStructSize != constantBufferMemberStructSize) throw new Exception("Incompatible data structure for EffectReflection ConstantBuffer member object");

                            for (int i = 0; i < countConstantBuffers; i++)
                            {
                                //read the cbuffer data
                                XkslangDLLBindingClass.ConstantBufferReflectionDescriptionData constantBufferData;
                                constantBufferData = (XkslangDLLBindingClass.ConstantBufferReflectionDescriptionData)Marshal.PtrToStructure(
                                    new IntPtr(pAllocsConstantBuffers.ToInt64() + (structSize * i)), typeof(XkslangDLLBindingClass.ConstantBufferReflectionDescriptionData));

                                string cbufferName = Marshal.PtrToStringAnsi(constantBufferData.CbufferName);
                                Marshal.FreeHGlobal(constantBufferData.CbufferName);

                                //process the cbuffer members
                                EffectValueDescription[] cbufferMembers = null;
                                if (constantBufferData.CountMembers > 0)
                                {
                                    cbufferMembers = new EffectValueDescription[constantBufferData.CountMembers];

                                    //read the member data
                                    for (int m = 0; m < constantBufferData.CountMembers; ++m)
                                    {
                                        XkslangDLLBindingClass.ConstantBufferMemberReflectionDescriptionData memberData;
                                        memberData = (XkslangDLLBindingClass.ConstantBufferMemberReflectionDescriptionData)Marshal.PtrToStructure(
                                            new IntPtr(constantBufferData.Members.ToInt64() + (memberStructSize * m)), typeof(XkslangDLLBindingClass.ConstantBufferMemberReflectionDescriptionData));

                                        string keyName = Marshal.PtrToStringAnsi(memberData.KeyName);
                                        string rawName = Marshal.PtrToStringAnsi(memberData.RawName);
                                        string logicalGroup = Marshal.PtrToStringAnsi(memberData.LogicalGroup);

                                        cbufferMembers[m] = new EffectValueDescription()
                                        {
                                            KeyInfo =
                                                {
                                                    KeyName = keyName,
                                                },
                                            RawName = rawName,
                                            Offset = memberData.Offset,
                                            Size = memberData.Size,
                                            LogicalGroup = logicalGroup,
                                            Type = new EffectTypeDescription()
                                            {
                                                Class = XkslangDLLBindingClass.ConvertEffectParameterReflectionClassEnum(memberData.Class),
                                                Type = XkslangDLLBindingClass.ConvertEffectParameterReflectionTypeEnum(memberData.Type),
                                                RowCount = memberData.RowCount,
                                                ColumnCount = memberData.ColumnCount,
                                                Elements = memberData.ArrayElements,
                                                ElementSize = memberData.Size,
                                                Name = rawName,
                                                Members = null,  //will be set below
                                            },
                                        };

                                        if (memberData.KeyName != null) Marshal.FreeHGlobal(memberData.KeyName);
                                        if (memberData.RawName != null) Marshal.FreeHGlobal(memberData.RawName);
                                        if (memberData.LogicalGroup != null) Marshal.FreeHGlobal(memberData.LogicalGroup);

                                        //if the cbuffer member is a struct, we analyse its struct members
                                        if (memberData.CountMembers > 0)
                                        {
                                            if (memberData.StructMembers != null)
                                            {
                                                EffectTypeMemberDescription[] structMembers = new EffectTypeMemberDescription[memberData.CountMembers];

                                                //Set the cbuffer member's struct members
                                                for (int im = 0; im < memberData.CountMembers; im++)
                                                {
                                                    XkslangDLLBindingClass.ConstantBufferMemberReflectionDescriptionData structMemberSrc;
                                                    structMemberSrc = (XkslangDLLBindingClass.ConstantBufferMemberReflectionDescriptionData)Marshal.PtrToStructure(
                                                        new IntPtr(memberData.StructMembers.ToInt64() + (memberStructSize * im)),
                                                        typeof(XkslangDLLBindingClass.ConstantBufferMemberReflectionDescriptionData));

                                                    EffectTypeMemberDescription structMemberDst = new EffectTypeMemberDescription();

                                                    if (!ConvertAndReleaseDllStructMemberDataToReflectionStructMemberType(ref structMemberSrc, ref structMemberDst))
                                                        throw new Exception("Failed to convert the EffectReflection cbuffer struct member data");

                                                    structMembers[im] = structMemberDst;
                                                }

                                                cbufferMembers[m].Type.Members = structMembers;
                                                Marshal.FreeHGlobal(memberData.StructMembers);
                                            }
                                            else throw new Exception("A cbuffer struct member is missing Reflection data about its members");
                                        }
                                    }

                                    Marshal.FreeHGlobal(constantBufferData.Members);
                                }

                                //create the cbuffer
                                EffectConstantBufferDescription constantBuffer = new EffectConstantBufferDescription()
                                {
                                    Name = cbufferName,
                                    Size = constantBufferData.Size,
                                    Type = ConstantBufferType.ConstantBuffer,
                                    Members = cbufferMembers,
                                };
                                xkslangEffectReflection.ConstantBuffers.Add(constantBuffer);
                            }

                            Marshal.FreeHGlobal(pAllocsConstantBuffers);
                        }
                    }  //end of: //Query and build the EffectReflection data
                }
                catch (Exception e)
                {
                    string errorMessages = e.Message;

                    //Get details about the error from Xkslang
                    IntPtr pErrorMsgs = XkslangDLLBindingClass.GetErrorMessages();
                    if (pErrorMsgs != IntPtr.Zero)
                    {
                        string xkslangErrorMsg = Marshal.PtrToStringAnsi(pErrorMsgs);
                        if (xkslangErrorMsg != null && xkslangErrorMsg.Length > 0) errorMessages = errorMessages + '\n' + xkslangErrorMsg;
                        Marshal.FreeHGlobal(pErrorMsgs);
                    }

                    //string[] messages = errorMessages.Split('\n');
                    //foreach (string str in messages) if (str.Length > 0) Console.WriteLine(str);
                    throw new Exception(errorMessages, e);
                }
                finally
                {
                    if (effectHandleId > 0) XkslangDLLBindingClass.ReleaseMixinHandle(effectHandleId); //release the mixer handle
                    XkslangDLLBindingClass.ReleaseMixer();
                    XkslangDLLBindingClass.ReleaseParser();

                    XkslangDLLBindingClass.ShaderSourceManager = null;
                    XkslangDLLBindingClass.ListShaderSourcesLoaded = null;
                }

                try
                {
                    //convert the stages bytecode to Hlsl and ascii (optionnal)
                    foreach (var stageShaderSpvBytecode in shaderStageSpvBytecodes)
                    {
                        var stageCompiledBytecode = stageShaderSpvBytecode.CompiledSpvBytecode;

                        ////Optionnal: convert the bytecode to human readable ascii text
                        //{
                        //    int asciiBufferLength = 0;
                        //    IntPtr pAsciiBytecodeBuffer = XkslangDLLBindingClass.ConvertBytecodeToAsciiText(stageCompiledBytecode, stageCompiledBytecode.Length, out asciiBufferLength);
                        //    if (pAsciiBytecodeBuffer == IntPtr.Zero || asciiBufferLength <= 0) throw new Exception("Failed to convert the bytecode to Ascii");
                        //
                        //    Byte[] asciiByteArray = new Byte[asciiBufferLength];
                        //    Marshal.Copy(pAsciiBytecodeBuffer, asciiByteArray, 0, asciiBufferLength);
                        //    Marshal.FreeHGlobal(pAsciiBytecodeBuffer);
                        //    stageShaderSpvBytecode.BytecodeAsciiFormat = System.Text.Encoding.UTF8.GetString(asciiByteArray);
                        //}

                        //convert the bytecode to HLSL
                        {
                            int bufferLen = 0;
                            IntPtr pBuffer = XkslangDLLBindingClass.ConvertBytecodeToHlsl(stageCompiledBytecode, stageCompiledBytecode.Length, hlslShaderModel, out bufferLen);
                            if (pBuffer == IntPtr.Zero || bufferLen <= 0) throw new Exception("Failed to convert the VS bytecode to HLSL");

                            Byte[] byteArray = new Byte[bufferLen];
                            Marshal.Copy(pBuffer, byteArray, 0, bufferLen);
                            Marshal.FreeHGlobal(pBuffer);
                            stageShaderSpvBytecode.BytecodeHlslConversion = System.Text.Encoding.UTF8.GetString(byteArray);
                        }
                    }
                }
                catch (Exception e)
                {
                    //string errorMessages = e.Message;
                    //Console.WriteLine(errorMessages);
                    throw e;
                }

                //Optionnal: write all generated shaders on the log folder
#if SILICONSTUDIO_PLATFORM_WINDOWS_DESKTOP
                {
                    //write the stages SPV ascii and shaders on the disk
                    if (mixinCompiledBytecode_AsciiText != null)
                    {
                        var bytecodeSourceFilename = Path.Combine(logDir, "effect_" + fullEffectName.Replace('.', '_') + "_" + xkfxSourceHash + "_compiled_final.spv.txt");
                        lock (WriterLock)
                        {
                            File.WriteAllText(bytecodeSourceFilename, mixinCompiledBytecode_AsciiText);
                        }
                    }
                    foreach (var stageShaderSpvBytecode in shaderStageSpvBytecodes)
                    {
                        if (stageShaderSpvBytecode.BytecodeAsciiFormat != null)
                        {
                            var bytecodeSourceFilename = Path.Combine(logDir, "shader_" + fullEffectName.Replace('.', '_') + "_" + xkfxSourceHash + "_" + stageShaderSpvBytecode.Stage + "_compiled.spv.txt");
                            lock (WriterLock)
                            {
                                File.WriteAllText(bytecodeSourceFilename, stageShaderSpvBytecode.BytecodeAsciiFormat);
                            }
                        }
                        if (stageShaderSpvBytecode.BytecodeHlslConversion != null)
                        {
                            var bytecodeSourceFilename = Path.Combine(logDir, "shader_" + fullEffectName.Replace('.', '_') + "_" + xkfxSourceHash + "_" + stageShaderSpvBytecode.Stage + ".hlsl");
                            lock (WriterLock)
                            {
                                File.WriteAllText(bytecodeSourceFilename, stageShaderSpvBytecode.BytecodeHlslConversion);
                            }
                        }
                    }
                }
#endif
            } //end of lock (XkslangMixinLock)

            //=====================================================================================================
            //Compile the generated HLSL shaders
            {
                var shaderStageBytecodes = new List<ShaderBytecode>();

#if SILICONSTUDIO_PLATFORM_WINDOWS_DESKTOP
                var stageStringBuilder = new StringBuilder();
#endif

                //TODO: check with other compilers !
                IShaderCompiler d3dcompiler = new Direct3D.ShaderCompiler();

                foreach (var stageShaderSpvBytecode in shaderStageSpvBytecodes)
                {
                    string shaderSource = stageShaderSpvBytecode.BytecodeHlslConversion;

                    string entryPoint = "main";
                    ShaderStage stage = stageShaderSpvBytecode.Stage;
                    var result = d3dcompiler.Compile(shaderSource, entryPoint, stage, effectParameters, xkslangEffectReflection, null);
                    result.CopyTo(log);

                    if (result.HasErrors)
                    {
                        continue;
                    }

                    // -------------------------------------------------------
                    // Append bytecode id to shader log
#if SILICONSTUDIO_PLATFORM_WINDOWS_DESKTOP
                    stageStringBuilder.AppendLine("@G    {0} => {1}".ToFormat(stage, result.Bytecode.Id));
                    if (result.DisassembleText != null)
                    {
                        stageStringBuilder.Append(result.DisassembleText);
                    }
#endif
                    // -------------------------------------------------------

                    shaderStageBytecodes.Add(result.Bytecode);

                    // When this is a compute shader, there is no need to scan other stages
                    if (stage == ShaderStage.Compute)
                        break;
                }

                var bytecode = new EffectBytecode { Reflection = xkslangEffectReflection, HashSources = hashSources };
                CleanupReflection(bytecode.Reflection);  // Remove unused reflection data, as it is entirely resolved at compile time.
                bytecode.Stages = shaderStageBytecodes.ToArray();

#if SILICONSTUDIO_PLATFORM_WINDOWS_DESKTOP
                lock (WriterLock) // protect write in case the same shader is created twice
                {
                    var builder = new StringBuilder();

                    if (log.HasErrors)
                    {
                        builder.AppendLine("/**************************");
                        builder.AppendLine("***** Compilation Errors *****");
                        builder.AppendLine("***************************");
                        foreach (var error in log.Messages)
                        {
                            builder.AppendLine(error.Text);
                        }
                        builder.AppendLine("***************************");
                    }

                    builder.AppendLine("/**************************");
                    builder.AppendLine("***** Compiler Parameters *****");
                    builder.AppendLine("***************************");
                    builder.Append("@P EffectName: ");
                    builder.AppendLine(fullEffectName ?? "");
                    builder.Append(compilerParameters?.ToStringPermutationsDetailed());
                    builder.AppendLine("***************************");

                    if (bytecode.Reflection.ConstantBuffers.Count > 0)
                    {
                        builder.AppendLine("****  ConstantBuffers  ****");
                        builder.AppendLine("***************************");
                        foreach (var cBuffer in bytecode.Reflection.ConstantBuffers)
                        {
                            builder.AppendFormat("cbuffer {0} [Size: {1}]", cBuffer.Name, cBuffer.Size).AppendLine();
                            foreach (var parameter in cBuffer.Members)
                            {
                                builder.AppendFormat("@C    {0} => {1}", parameter.RawName, parameter.KeyInfo.KeyName).AppendLine();
                            }
                        }
                        builder.AppendLine("***************************");
                    }

                    if (bytecode.Reflection.ResourceBindings.Count > 0)
                    {
                        builder.AppendLine("******  Resources    ******");
                        builder.AppendLine("***************************");
                        foreach (var resource in bytecode.Reflection.ResourceBindings)
                        {
                            builder.AppendFormat("@R    {0} => {1} [Stage: {2}, Slot: ({3}-{4})]", resource.RawName, resource.KeyInfo.KeyName, resource.Stage, resource.SlotStart, resource.SlotStart + resource.SlotCount - 1).AppendLine();
                        }
                        builder.AppendLine("***************************");
                    }

                    if (bytecode.HashSources != null && bytecode.HashSources.Count > 0)
                    {
                        builder.AppendLine("*****     Sources     *****");
                        builder.AppendLine("***************************");
                        foreach (var hashSource in bytecode.HashSources)
                        {
                            builder.AppendFormat("@S    {0} => {1}", hashSource.Key, hashSource.Value).AppendLine();
                        }
                        builder.AppendLine("***************************");
                    }

                    if (bytecode.Stages.Length > 0)
                    {
                        builder.AppendLine("*****     Stages      *****");
                        builder.AppendLine("***************************");
                        builder.Append(stageStringBuilder);
                        builder.AppendLine("***************************");
                    }
                    builder.AppendLine("*************************/");

                    foreach (var stageShaderSpvBytecode in shaderStageSpvBytecodes)
                    {
                        builder.AppendLine("");
                        builder.AppendLine("/*************************/");
                        builder.AppendLine("//" + stageShaderSpvBytecode.Stage);
                        builder.Append(stageShaderSpvBytecode.BytecodeHlslConversion);
                    }

                    var shaderSourceFilename = Path.Combine(logDir, "shader_" + fullEffectName.Replace('.', '_') + "_" + xkfxSourceHash + "_fullLogs" + ".hlsl");
                    File.WriteAllText(shaderSourceFilename, builder.ToString());
                }
#endif

                return new EffectBytecodeCompilerResult(bytecode, log);
            } //end of compilation

        }

        //If true, we compile the mixin using xkslangDll library
        public static readonly bool useXkslangShaderCompiler = true;

        public override TaskOrResult<EffectBytecodeCompilerResult> Compile(ShaderMixinSource mixinTree, EffectCompilerParameters effectParameters, CompilerParameters compilerParameters)
        {
            var log = new LoggerResult();
            var fullEffectName = mixinTree.Name;

            if (useXkslangShaderCompiler)
            {
                try
                {
                    var result = CompileEffectWithXkslangApi(mixinTree, effectParameters, compilerParameters);
                    return result;
                }
                catch(Exception)
                {
                    //temporary solution: do nothing for now, we fall back into the previous compiler
                }
            }
            
            // Load D3D compiler dll
            // Note: No lock, it's probably fine if it gets called from multiple threads at the same time.
            if (Platform.IsWindowsDesktop && !d3dCompilerLoaded)
            {
                NativeLibrary.PreloadLibrary("d3dcompiler_47.dll");
                d3dCompilerLoaded = true;
            }

            var shaderMixinSource = mixinTree;

            // Make a copy of shaderMixinSource. Use deep clone since shaderMixinSource can be altered during compilation (e.g. macros)
            var shaderMixinSourceCopy = new ShaderMixinSource();
            shaderMixinSourceCopy.DeepCloneFrom(shaderMixinSource);
            shaderMixinSource = shaderMixinSourceCopy;

            // Generate platform-specific macros
            switch (effectParameters.Platform)
            {
                case GraphicsPlatform.Direct3D11:
                    shaderMixinSource.AddMacro("SILICONSTUDIO_XENKO_GRAPHICS_API_DIRECT3D", 1);
                    shaderMixinSource.AddMacro("SILICONSTUDIO_XENKO_GRAPHICS_API_DIRECT3D11", 1);
                    break;
                case GraphicsPlatform.Direct3D12:
                    shaderMixinSource.AddMacro("SILICONSTUDIO_XENKO_GRAPHICS_API_DIRECT3D", 1);
                    shaderMixinSource.AddMacro("SILICONSTUDIO_XENKO_GRAPHICS_API_DIRECT3D12", 1);
                    break;
                case GraphicsPlatform.OpenGL:
                    shaderMixinSource.AddMacro("SILICONSTUDIO_XENKO_GRAPHICS_API_OPENGL", 1);
                    shaderMixinSource.AddMacro("SILICONSTUDIO_XENKO_GRAPHICS_API_OPENGLCORE", 1);
                    break;
                case GraphicsPlatform.OpenGLES:
                    shaderMixinSource.AddMacro("SILICONSTUDIO_XENKO_GRAPHICS_API_OPENGL", 1);
                    shaderMixinSource.AddMacro("SILICONSTUDIO_XENKO_GRAPHICS_API_OPENGLES", 1);
                    break;
                case GraphicsPlatform.Vulkan:
                    shaderMixinSource.AddMacro("SILICONSTUDIO_XENKO_GRAPHICS_API_VULKAN", 1);
                    break;
                default:
                    throw new NotSupportedException();
            }

            // Generate profile-specific macros
            shaderMixinSource.AddMacro("SILICONSTUDIO_XENKO_GRAPHICS_PROFILE", (int)effectParameters.Profile);
            shaderMixinSource.AddMacro("GRAPHICS_PROFILE_LEVEL_9_1", (int)GraphicsProfile.Level_9_1);
            shaderMixinSource.AddMacro("GRAPHICS_PROFILE_LEVEL_9_2", (int)GraphicsProfile.Level_9_2);
            shaderMixinSource.AddMacro("GRAPHICS_PROFILE_LEVEL_9_3", (int)GraphicsProfile.Level_9_3);
            shaderMixinSource.AddMacro("GRAPHICS_PROFILE_LEVEL_10_0", (int)GraphicsProfile.Level_10_0);
            shaderMixinSource.AddMacro("GRAPHICS_PROFILE_LEVEL_10_1", (int)GraphicsProfile.Level_10_1);
            shaderMixinSource.AddMacro("GRAPHICS_PROFILE_LEVEL_11_0", (int)GraphicsProfile.Level_11_0);
            shaderMixinSource.AddMacro("GRAPHICS_PROFILE_LEVEL_11_1", (int)GraphicsProfile.Level_11_1);
            shaderMixinSource.AddMacro("GRAPHICS_PROFILE_LEVEL_11_2", (int)GraphicsProfile.Level_11_2);

            // In .xksl, class has been renamed to shader to avoid ambiguities with HLSL
            shaderMixinSource.AddMacro("class", "shader");

            var parsingResult = GetMixinParser().Parse(shaderMixinSource, shaderMixinSource.Macros.ToArray());

            // Copy log from parser results to output
            CopyLogs(parsingResult, log);

            // Return directly if there are any errors
            if (parsingResult.HasErrors)
            {
                return new EffectBytecodeCompilerResult(null, log);
            }

            // Convert the AST to HLSL
            var writer = new SiliconStudio.Shaders.Writer.Hlsl.HlslWriter
            {
                EnablePreprocessorLine = false // Allow to output links to original pdxsl via #line pragmas
            };
            writer.Visit(parsingResult.Shader);
            var shaderSourceText = writer.Text;

            if (string.IsNullOrEmpty(shaderSourceText))
            {
                log.Error($"No code generated for effect [{fullEffectName}]");
                return new EffectBytecodeCompilerResult(null, log);
            }

            // -------------------------------------------------------
            // Save shader log
            // TODO: TEMP code to allow debugging generated shaders on Windows Desktop
#if SILICONSTUDIO_PLATFORM_WINDOWS_DESKTOP
            var shaderId = ObjectId.FromBytes(Encoding.UTF8.GetBytes(shaderSourceText));

            var logDir = Path.Combine(PlatformFolders.ApplicationBinaryDirectory, "log");
            if (!Directory.Exists(logDir))
            {
                Directory.CreateDirectory(logDir);
            }
            var shaderSourceFilename = Path.Combine(logDir, "shader_" + fullEffectName.Replace('.', '_') + "_" + shaderId + ".hlsl");
            lock (WriterLock) // protect write in case the same shader is created twice
            {
                // Write shader before generating to make sure that we are having a trace before compiling it (compiler may crash...etc.)
                if (!File.Exists(shaderSourceFilename))
                {
                    File.WriteAllText(shaderSourceFilename, shaderSourceText);
                }
            }
#else
            string shaderSourceFilename = null;
#endif
            // -------------------------------------------------------
            var bytecode = new EffectBytecode { Reflection = parsingResult.Reflection, HashSources = parsingResult.HashSources };

            // Select the correct backend compiler
            IShaderCompiler compiler;
            switch (effectParameters.Platform)
            {
#if SILICONSTUDIO_PLATFORM_WINDOWS
                case GraphicsPlatform.Direct3D11:
                case GraphicsPlatform.Direct3D12:
                    compiler = new Direct3D.ShaderCompiler();
                    break;
#endif
                case GraphicsPlatform.OpenGL:
                case GraphicsPlatform.OpenGLES:
                case GraphicsPlatform.Vulkan:
                    // get the number of render target outputs
                    var rtOutputs = 0;
                    var psOutput = parsingResult.Shader.Declarations.OfType<StructType>().FirstOrDefault(x => x.Name.Text == "PS_OUTPUT");
                    if (psOutput != null)
                    {
                        foreach (var rto in psOutput.Fields)
                        {
                            var sem = rto.Qualifiers.OfType<Semantic>().FirstOrDefault();
                            if (sem != null)
                            {
                                // special case SV_Target
                                if (rtOutputs == 0 && sem.Name.Text == "SV_Target")
                                {
                                    rtOutputs = 1;
                                    break;
                                }
                                for (var i = rtOutputs; i < 8; ++i)
                                {
                                    if (sem.Name.Text == ("SV_Target" + i))
                                    {
                                        rtOutputs = i + 1;
                                        break;
                                    }
                                }
                            }
                        }
                    }
                    compiler = new OpenGL.ShaderCompiler(rtOutputs);
                    break;
                default:
                    throw new NotSupportedException();
            }

            var shaderStageBytecodes = new List<ShaderBytecode>();

#if SILICONSTUDIO_PLATFORM_WINDOWS_DESKTOP
            var stageStringBuilder = new StringBuilder();
#endif
            // if the shader (non-compute) does not have a pixel shader, we should add it for OpenGL and OpenGL ES.
            if ((effectParameters.Platform == GraphicsPlatform.OpenGL || effectParameters.Platform == GraphicsPlatform.OpenGLES) && !parsingResult.EntryPoints.ContainsKey(ShaderStage.Pixel) && !parsingResult.EntryPoints.ContainsKey(ShaderStage.Compute))
            {
                parsingResult.EntryPoints.Add(ShaderStage.Pixel, null);
            }

            foreach (var stageBinding in parsingResult.EntryPoints)
            {
                // Compile
                // TODO: We could compile stages in different threads to improve compiler throughput?
                var result = compiler.Compile(shaderSourceText, stageBinding.Value, stageBinding.Key, effectParameters, bytecode.Reflection, shaderSourceFilename);
                result.CopyTo(log);

                if (result.HasErrors)
                {
                    continue;
                }

                // -------------------------------------------------------
                // Append bytecode id to shader log
#if SILICONSTUDIO_PLATFORM_WINDOWS_DESKTOP
                stageStringBuilder.AppendLine("@G    {0} => {1}".ToFormat(stageBinding.Key, result.Bytecode.Id));
                if (result.DisassembleText != null)
                {
                    stageStringBuilder.Append(result.DisassembleText);
                }
#endif
                // -------------------------------------------------------

                shaderStageBytecodes.Add(result.Bytecode);

                // When this is a compute shader, there is no need to scan other stages
                if (stageBinding.Key == ShaderStage.Compute)
                    break;
            }

            // Remove unused reflection data, as it is entirely resolved at compile time.
            CleanupReflection(bytecode.Reflection);
            bytecode.Stages = shaderStageBytecodes.ToArray();

            //bytecode.Reflection.ConstantBuffers = xkslangEffectReflection.ConstantBuffers;

#if SILICONSTUDIO_PLATFORM_WINDOWS_DESKTOP
            lock (WriterLock) // protect write in case the same shader is created twice
            {
                var builder = new StringBuilder();
                builder.AppendLine("/**************************");
                builder.AppendLine("***** Compiler Parameters *****");
                builder.AppendLine("***************************");
                builder.Append("@P EffectName: ");
                builder.AppendLine(fullEffectName ?? "");
                builder.Append(compilerParameters?.ToStringPermutationsDetailed());
                builder.AppendLine("***************************");

                if (bytecode.Reflection.ConstantBuffers.Count > 0)
                {
                    builder.AppendLine("****  ConstantBuffers  ****");
                    builder.AppendLine("***************************");
                    foreach (var cBuffer in bytecode.Reflection.ConstantBuffers)
                    {
                        builder.AppendFormat("cbuffer {0} [Size: {1}]", cBuffer.Name, cBuffer.Size).AppendLine();
                        foreach (var parameter in cBuffer.Members)
                        {
                            builder.AppendFormat("@C    {0} => {1}", parameter.RawName, parameter.KeyInfo.KeyName).AppendLine();
                        }
                    }
                    builder.AppendLine("***************************");
                }

                if (bytecode.Reflection.ResourceBindings.Count > 0)
                {
                    builder.AppendLine("******  Resources    ******");
                    builder.AppendLine("***************************");
                    foreach (var resource in bytecode.Reflection.ResourceBindings)
                    {
                        builder.AppendFormat("@R    {0} => {1} [Stage: {2}, Slot: ({3}-{4})]", resource.RawName, resource.KeyInfo.KeyName, resource.Stage, resource.SlotStart, resource.SlotStart + resource.SlotCount - 1).AppendLine();
                    }
                    builder.AppendLine("***************************");
                }

                if (bytecode.HashSources.Count > 0)
                {
                    builder.AppendLine("*****     Sources     *****");
                    builder.AppendLine("***************************");
                    foreach (var hashSource in bytecode.HashSources)
                    {
                        builder.AppendFormat("@S    {0} => {1}", hashSource.Key, hashSource.Value).AppendLine();
                    }
                    builder.AppendLine("***************************");
                }

                if (bytecode.Stages.Length > 0)
                {
                    builder.AppendLine("*****     Stages      *****");
                    builder.AppendLine("***************************");
                    builder.Append(stageStringBuilder);
                    builder.AppendLine("***************************");
                }
                builder.AppendLine("*************************/");

                // Re-append the shader with all informations
                builder.Append(shaderSourceText);

                File.WriteAllText(shaderSourceFilename, builder.ToString());
            }
#endif

            return new EffectBytecodeCompilerResult(bytecode, log);
        }

        private static void CopyLogs(SiliconStudio.Shaders.Utility.LoggerResult inputLog, LoggerResult outputLog)
        {
            foreach (var inputMessage in inputLog.Messages)
            {
                var logType = LogMessageType.Info;
                switch (inputMessage.Level)
                {
                    case ReportMessageLevel.Error:
                        logType = LogMessageType.Error;
                        break;
                    case ReportMessageLevel.Info:
                        logType = LogMessageType.Info;
                        break;
                    case ReportMessageLevel.Warning:
                        logType = LogMessageType.Warning;
                        break;
                }
                var outputMessage = new LogMessage(inputMessage.Span.ToString(), logType, string.Format(" {0}: {1}", inputMessage.Code, inputMessage.Text));
                outputLog.Log(outputMessage);
            }
            outputLog.HasErrors = inputLog.HasErrors;
        }

        private static void CleanupReflection(EffectReflection reflection)
        {
            // TODO GRAPHICS REFACTOR we hardcode several resource group we want to preserve or optimize completly
            // Somehow this should be handled some other place (or probably we shouldn't cleanup reflection at all?)
            bool hasMaterialGroup = false;
            bool hasLightingGroup = false;

            foreach (var resourceBinding in reflection.ResourceBindings)
            {
                if (resourceBinding.Stage != ShaderStage.None)
                {
                    if (!hasLightingGroup && resourceBinding.ResourceGroup == "PerLighting")
                        hasLightingGroup = true;
                    else if (!hasMaterialGroup && resourceBinding.ResourceGroup == "PerMaterial")
                        hasMaterialGroup = true;
                }
            }

            var usedConstantBuffers = new HashSet<string>();

            for (int i = reflection.ResourceBindings.Count - 1; i >= 0; i--)
            {
                var resourceBinding = reflection.ResourceBindings[i];

                // Do not touch anything if there is logical groups
                // TODO: We can do better than that: remove only if the full group can be optimized away
                if (resourceBinding.LogicalGroup != null)
                    continue;

                if (resourceBinding.Stage == ShaderStage.None && !(hasMaterialGroup && resourceBinding.ResourceGroup == "PerMaterial") && !(hasLightingGroup && resourceBinding.ResourceGroup == "PerLighting"))
                {
                    reflection.ResourceBindings.RemoveAt(i);
                }
                else if (resourceBinding.Class == EffectParameterClass.ConstantBuffer
                    || resourceBinding.Class == EffectParameterClass.TextureBuffer)
                {
                    // Mark associated cbuffer/tbuffer as used
                    usedConstantBuffers.Add(resourceBinding.KeyInfo.KeyName);
                }
            }

            // Remove unused cbuffer
            for (int i = reflection.ConstantBuffers.Count - 1; i >= 0; i--)
            {
                var cbuffer = reflection.ConstantBuffers[i];

                // Do not touch anything if there is logical groups
                // TODO: We can do better than that: remove only if the full group can be optimized away
                var hasLogicalGroup = false;
                foreach (var member in cbuffer.Members)
                {
                    if (member.LogicalGroup != null)
                    {
                        hasLogicalGroup = true;
                        break;
                    }
                }

                if (hasLogicalGroup)
                    continue;

                if (!usedConstantBuffers.Contains(cbuffer.Name))
                {
                    reflection.ConstantBuffers.RemoveAt(i);
                }
            }
        }
    }
}

#endif
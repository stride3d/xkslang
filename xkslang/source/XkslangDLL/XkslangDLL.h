#pragma once

#include <stdio.h>
#include <iostream>
#include <objbase.h>

#include "../Common/define.h"
#include "../SpxMixer/EffectReflection.h"

namespace xkslang
{
	//=====================================================================================================================
	//Structs declaration
	//=====================================================================================================================
    struct OutputStageEntryPoint
    {
        ShadingStageEnum stage;
        const char* entryPointName;
    };

	//struct containing a constant buffer member data (to be easily exchanged between native and managed apps)
	struct ConstantBufferMemberReflectionDescriptionData
	{
	public:
		int32_t Offset;
		const char* KeyName;
        const char* RawName;

		EffectParameterReflectionClass Class;
		EffectParameterReflectionType Type;
		int32_t RowCount;
		int32_t ColumnCount;
		int32_t ArrayElements;
		int32_t Size;
		int32_t Alignment;
		int32_t ArrayStride;
		int32_t MatrixStride;
		int32_t CountMembers;
		//TypeMemberReflectionDescription* Members;

		ConstantBufferMemberReflectionDescriptionData(const int32_t offset, const char* keyName, const char* rawName, const TypeReflectionDescription& t)
			: Offset(offset), KeyName(keyName), RawName(rawName),
            Class(t.Class), Type(t.Type), RowCount(t.RowCount), ColumnCount(t.ColumnCount), ArrayElements(t.ArrayElements), Size(t.Size),
			Alignment(t.Alignment), ArrayStride(t.ArrayStride), MatrixStride(t.MatrixStride), CountMembers(t.CountMembers) {}
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

	//struct containing a resource binding data (to be easily exchanged between native and managed apps)
	struct EffectResourceBindingDescriptionData
	{
	public:
		ShadingStageEnum Stage;
		EffectParameterReflectionClass Class;
		EffectParameterReflectionType Type;
		const char* KeyName;
        const char* RawName;

		EffectResourceBindingDescriptionData(const EffectResourceBindingDescription& e, const char* keyName, const char* rawName)
            : Stage(e.Stage), Class(e.Class), Type(e.Type), KeyName(keyName), RawName(rawName) {}
	};

	//struct containing an input attribute data (to be easily exchanged between native and managed apps)
	struct ShaderInputAttributeDescriptionData
	{
	public:
		int SemanticIndex;
		const char* SemanticName;

		ShaderInputAttributeDescriptionData(int semanticIndex, const char* semanticName) : SemanticIndex(semanticIndex), SemanticName(semanticName) {}
	};

	//=====================================================================================================================
	//=====================================================================================================================
	//Return the error messages after an operation failed
	extern "C" __declspec(dllexport) const char* GetErrorMessages();

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
    // shaderName: name of the shader to convert
    // shaderDependencyCallback: callback function, called by xkslang everytime the parser requests data for a shader (shaderName at first, then all its dependencies if any)
    // Return:
    //  null: if the conversion failed (user can call GetErrorMessages function to get more details)
    //  pointer to the bytecode if the operation succeeded
    //    The pointer is allocated on the dll side (using LocalAlloc function), and has to be deleted by the caller
    //    bytecodeSize parameter contains the length of the bytecode buffer
    extern "C" __declspec(dllexport) uint32_t* ConvertXkslShaderToSPX(char* shaderName, ShaderSourceLoaderCallback shaderDependencyCallback, int32_t* bytecodeSize);

    //=====================================================================================================================
    //=====================================================================================================================
    //Utility function to help converting a bytecode to a human-readable ascii file
    extern "C" __declspec(dllexport) char* ConvertBytecodeToAsciiText(uint32_t* bytecode, int32_t bytecodeSize, int32_t* asciiBufferSize);

    extern "C" __declspec(dllexport) char* ConvertBytecodeToGlsl(uint32_t* bytecode, int32_t bytecodeSize, int32_t* asciiBufferSize);

    extern "C" __declspec(dllexport) char* ConvertBytecodeToHlsl(uint32_t* bytecode, int32_t bytecodeSize, int32_t shaderModel, int32_t* asciiBufferSize);

    //=====================================================================================================================
    //=====================================================================================================================
    // Mixin functions: Mix SPX shaders to generate SPV bytecode for specific output stages

    extern "C" __declspec(dllexport) bool InitializeMixer();

    extern "C" __declspec(dllexport) void ReleaseMixer();

    //Create a new mixin object
    // Return: the mixin object handle Id, or 0 if there is any error
    extern "C" __declspec(dllexport) uint32_t CreateSpxShaderMixer();

    extern "C" __declspec(dllexport) bool ReleaseSpxShaderMixer(uint32_t mixerHandleId);

    extern "C" __declspec(dllexport) bool MixinShader(uint32_t mixerHandleId, const char* shaderName, uint32_t* shaderSpxBytecode, int32_t bytecodeSize);

    extern "C" __declspec(dllexport) bool CompileMixer(uint32_t mixerHandleId, OutputStageEntryPoint* stageEntryPointArray, int32_t countStages);

	//Return the mixin compiled bytecode, generated when we compile the mixer
	//this bytecode can let the client retrieve Reflection data
	//The bytecode buffer is allocated on the dll side, and has to be released on the caller side
	extern "C" __declspec(dllexport) uint32_t* GetMixerCompiledBytecode(uint32_t mixerHandleId, int32_t* bytecodeSize);

	//Or we can use the following function to get the size of the compiled bytecode buffer, then to copy its data into a buffer allocated by the caller
	extern "C" __declspec(dllexport) int32_t GetMixerCompiledBytecodeSize(uint32_t mixerHandleId);
	extern "C" __declspec(dllexport) int32_t CopyMixerCompiledBytecode(uint32_t mixerHandleId, uint32_t* bytecodeBuffer, int32_t bufferSize);

    //Return the compiled bytecode for the given stage
    //The bytecode buffer is allocated on the dll side, and has to be released on the caller side
    extern "C" __declspec(dllexport) uint32_t* GetMixerCompiledBytecodeForStage(uint32_t mixerHandleId, ShadingStageEnum stage, int32_t* bytecodeSize);

    //We can use the following functions to get the size of a stage compiled bytecode buffer, then to copy its data into a buffer allocated by the caller
    extern "C" __declspec(dllexport) int32_t GetMixerCompiledBytecodeSizeForStage(uint32_t mixerHandleId, ShadingStageEnum stage);
    extern "C" __declspec(dllexport) int32_t CopyMixerCompiledBytecodeForStage(uint32_t mixerHandleId, ShadingStageEnum stage, uint32_t* bytecodeBuffer, int32_t bufferSize);

	//After a mixer has been successfully compiled: call this function to get its Effect Reflection Data
	extern "C" __declspec(dllexport) bool GetMixerEffectReflectionData(uint32_t mixerHandleId,
		ConstantBufferReflectionDescriptionData** constantBuffers, int32_t* countConstantBuffers,
		EffectResourceBindingDescriptionData** resourceBindings, int32_t* countResourceBindings,
		ShaderInputAttributeDescriptionData** inputAttributes, int32_t* countInputAttributes);
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
        [DllImport("Xkslang.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr GetErrorMessages();

        //=====================================================================================================================
        // Parsing functions: parse and convert xksl shaders to SPX bytecode
        //=====================================================================================================================

        //Xkslang initialization: To be called before calling parsing functions
        [DllImport("Xkslang.dll", CallingConvention = CallingConvention.Cdecl)]
        [return: MarshalAs(UnmanagedType.I1)]
        public static extern bool InitializeParser();

        //To be called when we finish parsing a file
        [DllImport("Xkslang.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern void ReleaseParser();

        //Convert an xksl shader into SPX bytecode
        // shaderName: name of the shader to convert
        // shaderDependencyCallback: callback function, called by xkslang everytime the parser requests data for a shader (shaderName at first, then all its dependencies if any)
        // Return:
        //  null: if the conversion failed (user can call GetErrorMessages function to get more details)
        //  pointer to the bytecode if the operation succeeded
        //    The pointer is allocated on the dll side (using LocalAlloc function), and has to be deleted by the caller
        //    bytecodeSize parameter contains the length of the bytecode buffer
        [DllImport("Xkslang.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr ConvertXkslShaderToSPX([MarshalAs(UnmanagedType.LPStr)] string shaderName, [MarshalAs(UnmanagedType.FunctionPtr)] ShaderSourceLoaderCallbackDelegate callbackPointer, out Int32 bytecodeLength);

        //=====================================================================================================================
        //Utility functions to help converting a bytecode to a human-readable ascii file, or to glsl/glsl files
        //=====================================================================================================================
        [DllImport("Xkslang.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr ConvertBytecodeToAsciiText([In] Int32[] bytecode, Int32 bytecodeSize, out Int32 bytecodeLength);

        [DllImport("Xkslang.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr ConvertBytecodeToGlsl([In] Int32[] bytecode, Int32 bytecodeSize, out Int32 bytecodeLength);

        [DllImport("Xkslang.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr ConvertBytecodeToHlsl([In] Int32[] bytecode, Int32 bytecodeSize, Int32 shaderModel, out Int32 bytecodeLength);

        //=====================================================================================================================
        // Mixin functions: Mix SPX shaders to generate SPV bytecode for specific output stages
        //=====================================================================================================================

        //SpxMixer initialization: To be called before starting mixin a new effect
        [DllImport("Xkslang.dll", CallingConvention = CallingConvention.Cdecl)]
        [return: MarshalAs(UnmanagedType.I1)]
        public static extern bool InitializeMixer();

        //To be called when we finish mixin an effect
        [DllImport("Xkslang.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern void ReleaseMixer();

        //Create a new SPX shader mixer, return the mixer handle Id
        [DllImport("Xkslang.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern UInt32 CreateSpxShaderMixer();

        [DllImport("Xkslang.dll", CallingConvention = CallingConvention.Cdecl)]
        [return: MarshalAs(UnmanagedType.I1)]
        public static extern bool ReleaseSpxShaderMixer(UInt32 mixerHandleId);

        //=====================================================================================================================
        //Mixin a shader into the current mixer
        [DllImport("Xkslang.dll", CallingConvention = CallingConvention.Cdecl)]
        [return: MarshalAs(UnmanagedType.I1)]
        public static extern bool MixinShader(UInt32 mixerHandleId, [MarshalAs(UnmanagedType.LPStr)] string shaderName, [In] Int32[] bytecode, Int32 bytecodeSize);

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

        //=====================================================================================================================
        //Compiled the mixer (to be called after all mixin and compositions have been set)
        [DllImport("Xkslang.dll", CallingConvention = CallingConvention.Cdecl)]
        [return: MarshalAs(UnmanagedType.I1)]
        public static extern bool CompileMixer(UInt32 mixerHandleId, [In, Out] OutputStageEntryPoint[] stageEntryPointArray, Int32 countStages);

        //=====================================================================================================================
        //Get the mixer compiled bytecode (common to all output stages)
        //This compiled bytecode can be used to retrieve the Effect Reflection data
        [DllImport("Xkslang.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr GetMixerCompiledBytecode(UInt32 mixerHandleId, out Int32 bytecodeLength);

        //Get the mixer compiled bytecode's size
        [DllImport("Xkslang.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern Int32 GetMixerCompiledBytecodeSize(UInt32 mixerHandleId);

        //Ask the mixer to copy the compiled bytecode into the buffer
        [DllImport("Xkslang.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern Int32 CopyMixerCompiledBytecode(UInt32 mixerHandleId, [In] Int32[] bytecodeBuffer, Int32 bufferSize);

        //=====================================================================================================================
        //Get an output stage compiled bytecode
        //=====================================================================================================================
        [DllImport("Xkslang.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr GetMixerCompiledBytecodeForStage(UInt32 mixerHandleId, ShadingStageEnum stage, out Int32 bytecodeLength);

        //Get an output stage compiled bytecode's size
        [DllImport("Xkslang.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern Int32 GetMixerCompiledBytecodeSizeForStage(UInt32 mixerHandleId, ShadingStageEnum stage);

        //Ask the mixer to copy a stage compiled bytecode into the buffer
        [DllImport("Xkslang.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern Int32 CopyMixerCompiledBytecodeForStage(UInt32 mixerHandleId, ShadingStageEnum stage, [In] Int32[] bytecodeBuffer, Int32 bufferSize);

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

            //[MarshalAs(UnmanagedType.LPStr)]
            //public string KeyName;
        }

        //Input Attribute struct
        [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi)]
        public struct ShaderInputAttributeDescriptionData
        {
            public Int32 SemanticIndex;
            public IntPtr SemanticName;
        }

        [DllImport("Xkslang.dll", CallingConvention = CallingConvention.Cdecl)]
        [return: MarshalAs(UnmanagedType.I1)]
        public static extern bool GetMixerEffectReflectionData(UInt32 mixerHandleId,
            out IntPtr constantBuffersData, out Int32 countConstantBuffers,
            out IntPtr resourceBindingsData, out Int32 countResourceBindings,
            out IntPtr inputAttributesData, out Int32 countInputAttributes);

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
        //Conversion functions
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
    }

    /// <summary>
    /// An <see cref="IEffectCompiler"/> which will compile effect into multiple shader code, and compile them with a <see cref="IShaderCompiler"/>.
    /// </summary>
    public class EffectCompiler : EffectCompilerBase
    {
        private bool d3dCompilerLoaded = false;
        private static readonly Object WriterLock = new Object();

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

        public static readonly bool CompileEffectUsingXkslang = true;

        public override TaskOrResult<EffectBytecodeCompilerResult> Compile(ShaderMixinSource mixinTree, EffectCompilerParameters effectParameters, CompilerParameters compilerParameters)
        {
            var log = new LoggerResult();
            var fullEffectName = mixinTree.Name;

            if (CompileEffectUsingXkslang && fullEffectName == "Effect")
            {
#if SILICONSTUDIO_PLATFORM_WINDOWS_DESKTOP
                var logDir = Path.Combine(PlatformFolders.ApplicationBinaryDirectory, "log");
                if (!Directory.Exists(logDir))
                {
                    Directory.CreateDirectory(logDir);
                }
#endif

                //=====================================================================================================================================
                //TEST loading and parsing the xksl shader
                string shaderName = mixinTree.Name;
                Int32[] effectSpxBytecode = null;
                string effectSpxBytecode_AsciiText = null;

                Int32[] mixinCompiledBytecode = null;
                string mixinCompiledBytecode_AsciiText;

                Int32[] spvBytecodeVS = null;
                Int32[] spvBytecodePS = null;
                string spvBytecodeVS_AsciiText = null;
                string spvBytecodePS_AsciiText = null;
                int hlslShaderModel = 40;
                string shaderGlslVS = null;
                string shaderGlslPS = null;
                string shaderHlslVS = null;
                string shaderHlslPS = null;

                try
                {
                    XkslangDLLBindingClass.ShaderSourceManager = GetMixinParser().SourceManager;
                    XkslangDLLBindingClass.ListShaderSourcesLoaded = new List<ShaderSourceManager.ShaderSourceWithHash>();

                    bool success = true;

                    //initialise xkslang
                    success = XkslangDLLBindingClass.InitializeParser();
                    if (!success) throw new Exception("Error initializing Xkslang");

                    //convert the XKSL shader to SPX bytecode
                    int bytecodeLength = 0;
                    IntPtr pBytecodeBuffer = XkslangDLLBindingClass.ConvertXkslShaderToSPX(shaderName, XkslangDLLBindingClass.ShaderSourceLoaderCallback, out bytecodeLength);
                    if (pBytecodeBuffer == IntPtr.Zero || bytecodeLength < 0)
                    {
                        log.Error($"No Spx bytecode generated for effect [{fullEffectName}]");
                        return new EffectBytecodeCompilerResult(null, log);
                    }

                    //copy the bytecode and free the object (allocated by the dll)
                    effectSpxBytecode = new Int32[bytecodeLength];
                    Marshal.Copy(pBytecodeBuffer, effectSpxBytecode, 0, bytecodeLength);
                    Marshal.FreeHGlobal(pBytecodeBuffer);

                    //=====================================================================================================================================
                    //Optionnal: convert the bytecode to human readable ascii text
                    {
                        int asciiBufferLength = 0;
                        IntPtr pAsciiBytecodeBuffer = XkslangDLLBindingClass.ConvertBytecodeToAsciiText(effectSpxBytecode, effectSpxBytecode.Length, out asciiBufferLength);
                        if (pAsciiBytecodeBuffer == IntPtr.Zero || asciiBufferLength < 0) throw new Exception("Failed to convert the Spx bytecode into Ascii");

                        Byte[] asciiByteArray = new Byte[asciiBufferLength];
                        Marshal.Copy(pAsciiBytecodeBuffer, asciiByteArray, 0, asciiBufferLength);
                        Marshal.FreeHGlobal(pAsciiBytecodeBuffer);
                        effectSpxBytecode_AsciiText = System.Text.Encoding.UTF8.GetString(asciiByteArray);
                    }
                }
                catch (Exception e)
                {
                    string errorMessages = e.Message;

                    //Check if we can get some error messages from xkslang DLL
                    IntPtr pErrorMsgs = XkslangDLLBindingClass.GetErrorMessages();
                    if (pErrorMsgs != IntPtr.Zero)
                    {
                        string xkslangErrorMsg = Marshal.PtrToStringAnsi(pErrorMsgs);
                        if (xkslangErrorMsg != null && xkslangErrorMsg.Length > 0) errorMessages = errorMessages + '\n' + xkslangErrorMsg;
                        Marshal.FreeHGlobal(pErrorMsgs);
                    }

                    string[] messages = errorMessages.Split('\n');
                    foreach (string str in messages) if (str.Length > 0) Console.WriteLine(str);
                    throw e;
                }
                finally
                {
                    XkslangDLLBindingClass.ReleaseParser();
                }
                
#if SILICONSTUDIO_PLATFORM_WINDOWS_DESKTOP
                //=====================================================================================================================================
                //write the SPX ascii bytecodes on the disk
                if (effectSpxBytecode_AsciiText != null)
                {
                    //var bytecodeId = ObjectId.FromBytes(Encoding.UTF8.GetBytes(spxAsciiBytecode));
                    //var bytecodeSourceFilename = Path.Combine(strLogDir, "shader_" + mixinTree.Name.Replace('.', '_') + "_" + bytecodeId + ".hr.spv");
                    var bytecodeSourceFilename = Path.Combine(logDir, "shader_" + mixinTree.Name.Replace('.', '_') + ".hr.spv");

                    lock (WriterLock) // protect write in case the same shader is created twice
                    {
                        // Write shader before generating to make sure that we are having a trace before compiling it (compiler may crash...etc.)
                        //if (!File.Exists(bytecodeSourceFilename))
                        {
                            File.WriteAllText(bytecodeSourceFilename, effectSpxBytecode_AsciiText);
                        }
                    }
                }
#endif

                EffectReflection xkslangEffectReflection = new EffectReflection();

                //=====================================================================================================================================
                //Mix the effect to generate the output stage SPV bytecode. Hardcoded for now: mix the effect shader class only, for PS and VS stages
                UInt32 mixerHandleId = 0;
                try
                {
                    if (!XkslangDLLBindingClass.InitializeMixer())
                        throw new Exception("Failed to initialize the mixer");

                    mixerHandleId = XkslangDLLBindingClass.CreateSpxShaderMixer();
                    if (mixerHandleId == 0) throw new Exception("Failed to create a new spx mixer");

                    bool success;
                    success = XkslangDLLBindingClass.MixinShader(mixerHandleId, shaderName, effectSpxBytecode, effectSpxBytecode.Length);
                    if (!success) throw new Exception("Failed to mix the shader: " + shaderName);

                    XkslangDLLBindingClass.OutputStageEntryPoint[] stageEntryPointArray = {
                        new XkslangDLLBindingClass.OutputStageEntryPoint(XkslangDLLBindingClass.ShadingStageEnum.Vertex, "VSMain"),
                        new XkslangDLLBindingClass.OutputStageEntryPoint(XkslangDLLBindingClass.ShadingStageEnum.Pixel, "PSMain")
                    };

                    success = XkslangDLLBindingClass.CompileMixer(mixerHandleId, stageEntryPointArray, stageEntryPointArray.Length);
                    if (!success) throw new Exception("Failed to compile the shader: " + shaderName);

                    //=====================================================================================================================================
                    //get the mixin compiled bytecode
                    {
                        ////1st option: the dll allocate, copy and returns the buffer: caller needs to delete this buffer by calling "Marshal.FreeHGlobal"
                        //int bytecodeLength = 0;
                        //IntPtr pBytecodeBuffer = XkslangDLLBindingClass.GetMixerCompiledBytecode(mixerHandleId, out bytecodeLength);
                        //if (pBytecodeBuffer == null || bytecodeLength < 0) throw new Exception("Failed to get the mixin compiled bytecode");
                        ////copy the bytecode and free the buffer allocated by the dll
                        //mixinCompiledBytecode = new Int32[bytecodeLength];
                        //Marshal.Copy(pBytecodeBuffer, mixinCompiledBytecode, 0, bytecodeLength);
                        //Marshal.FreeHGlobal(pBytecodeBuffer);

                        //2nd option: allocate the buffer on the caller side and ask the dll to fill it
                        int bytecodeLength = XkslangDLLBindingClass.GetMixerCompiledBytecodeSize(mixerHandleId);
                        if (bytecodeLength <= 0) throw new Exception("Failed to get the mixer compiled bytecode size");
                        mixinCompiledBytecode = new Int32[bytecodeLength];
                        int aLen = XkslangDLLBindingClass.CopyMixerCompiledBytecode(mixerHandleId, mixinCompiledBytecode, mixinCompiledBytecode.Length);
                        if (aLen != bytecodeLength) throw new Exception("Failed to get the mixer compiled bytecode");

                        //Optionnal: convert the bytecode to human readable ascii text
                        {
                            int asciiBufferLength = 0;
                            IntPtr pAsciiBytecodeBuffer = XkslangDLLBindingClass.ConvertBytecodeToAsciiText(mixinCompiledBytecode, mixinCompiledBytecode.Length, out asciiBufferLength);
                            if (pAsciiBytecodeBuffer == IntPtr.Zero || asciiBufferLength < 0) throw new Exception("Failed to convert the bytecode to Ascii");

                            Byte[] asciiByteArray = new Byte[asciiBufferLength];
                            Marshal.Copy(pAsciiBytecodeBuffer, asciiByteArray, 0, asciiBufferLength);
                            Marshal.FreeHGlobal(pAsciiBytecodeBuffer);
                            mixinCompiledBytecode_AsciiText = System.Text.Encoding.UTF8.GetString(asciiByteArray);
                        }
                    }

                    //=====================================================================================================================================
                    //get the VS SPIRV bytecode
                    {
                        ////1st option: the dll allocate, copy and returns the buffer: caller needs to delete this buffer by calling "Marshal.FreeHGlobal"
                        //int bytecodeLength = 0;
                        //IntPtr pBytecodeBuffer = XkslangDLLBindingClass.GetMixerCompiledBytecodeForStage(mixerHandleId, XkslangDLLBindingClass.ShadingStageEnum.Vertex, out bytecodeLength);
                        //if (pBytecodeBuffer == null || bytecodeLength < 0) throw new Exception("Failed to get the bytecode for VS stage");
                        ////copy the bytecode and free the buffer allocated by the dll
                        //spvBytecodeVS = new Int32[bytecodeLength];
                        //Marshal.Copy(pBytecodeBuffer, spvBytecodeVS, 0, bytecodeLength);
                        //Marshal.FreeHGlobal(pBytecodeBuffer);

                        //2nd option: allocate the buffer on the caller side and ask the dll to fill it
                        int bytecodeLength = XkslangDLLBindingClass.GetMixerCompiledBytecodeSizeForStage(mixerHandleId, XkslangDLLBindingClass.ShadingStageEnum.Vertex);
                        if (bytecodeLength <= 0) throw new Exception("Failed to get the bytecode size for VS");
                        spvBytecodeVS = new Int32[bytecodeLength];
                        int aLen = XkslangDLLBindingClass.CopyMixerCompiledBytecodeForStage(mixerHandleId, XkslangDLLBindingClass.ShadingStageEnum.Vertex, spvBytecodeVS, spvBytecodeVS.Length);
                        if (aLen != bytecodeLength) throw new Exception("Failed to get the bytecode for VS");

                        //Optionnal: convert the bytecode to human readable ascii text
                        {
                            int asciiBufferLength = 0;
                            IntPtr pAsciiBytecodeBuffer = XkslangDLLBindingClass.ConvertBytecodeToAsciiText(spvBytecodeVS, spvBytecodeVS.Length, out asciiBufferLength);
                            if (pAsciiBytecodeBuffer == IntPtr.Zero || asciiBufferLength < 0) throw new Exception("Failed to convert the bytecode to Ascii");

                            Byte[] asciiByteArray = new Byte[asciiBufferLength];
                            Marshal.Copy(pAsciiBytecodeBuffer, asciiByteArray, 0, asciiBufferLength);
                            Marshal.FreeHGlobal(pAsciiBytecodeBuffer);
                            spvBytecodeVS_AsciiText = System.Text.Encoding.UTF8.GetString(asciiByteArray);
                        }

                        //convert the bytecode to GLSL
                        {
                            int bufferLen = 0;
                            IntPtr pBuffer = XkslangDLLBindingClass.ConvertBytecodeToGlsl(spvBytecodeVS, spvBytecodeVS.Length, out bufferLen);
                            if (pBuffer == null || bufferLen < 0) throw new Exception("Failed to convert the VS bytecode to GLSL");

                            Byte[] byteArray = new Byte[bufferLen];
                            Marshal.Copy(pBuffer, byteArray, 0, bufferLen);
                            Marshal.FreeHGlobal(pBuffer);
                            shaderGlslVS = System.Text.Encoding.UTF8.GetString(byteArray);
                        }

                        //convert the bytecode to HLSL
                        {
                            int bufferLen = 0;
                            IntPtr pBuffer = XkslangDLLBindingClass.ConvertBytecodeToHlsl(spvBytecodeVS, spvBytecodeVS.Length, hlslShaderModel, out bufferLen);
                            if (pBuffer == IntPtr.Zero || bufferLen < 0) throw new Exception("Failed to convert the VS bytecode to HLSL");

                            Byte[] byteArray = new Byte[bufferLen];
                            Marshal.Copy(pBuffer, byteArray, 0, bufferLen);
                            Marshal.FreeHGlobal(pBuffer);
                            shaderHlslVS = System.Text.Encoding.UTF8.GetString(byteArray);
                        }
                    }

                    //=====================================================================================================================================
                    //get the PS SPIRV bytecode
                    {
                        ////1st option: the dll allocate, copy and returns the buffer: caller needs to delete this buffer by calling "Marshal.FreeHGlobal"
                        //int bytecodeLength = 0;
                        //IntPtr pBytecodeBuffer = XkslangDLLBindingClass.GetMixerCompiledBytecodeForStage(mixerHandleId, XkslangDLLBindingClass.ShadingStageEnum.Pixel, out bytecodeLength);
                        //if (pBytecodeBuffer == null || bytecodeLength < 0) throw new Exception("Failed to get the bytecode for PS stage");
                        ////copy the bytecode and free the object (allocated by the dll)
                        //spvBytecodePS = new Int32[bytecodeLength];
                        //Marshal.Copy(pBytecodeBuffer, spvBytecodePS, 0, bytecodeLength);
                        //Marshal.FreeHGlobal(pBytecodeBuffer);

                        //2nd option: allocate the buffer on the caller side and ask the dll to fill it
                        int bytecodeLength = XkslangDLLBindingClass.GetMixerCompiledBytecodeSizeForStage(mixerHandleId, XkslangDLLBindingClass.ShadingStageEnum.Pixel);
                        if (bytecodeLength <= 0) throw new Exception("Failed to get the bytecode size for PS");
                        spvBytecodePS = new Int32[bytecodeLength];
                        int aLen = XkslangDLLBindingClass.CopyMixerCompiledBytecodeForStage(mixerHandleId, XkslangDLLBindingClass.ShadingStageEnum.Pixel, spvBytecodePS, spvBytecodePS.Length);
                        if (aLen != bytecodeLength) throw new Exception("Failed to get the bytecode for PS");

                        //Optionnal: convert the bytecode to human readable ascii text
                        {
                            int asciiBufferLength = 0;
                            IntPtr pAsciiBytecodeBuffer = XkslangDLLBindingClass.ConvertBytecodeToAsciiText(spvBytecodePS, spvBytecodePS.Length, out asciiBufferLength);
                            if (pAsciiBytecodeBuffer == IntPtr.Zero || asciiBufferLength < 0) throw new Exception("Failed to convert the PS bytecode to Ascii");

                            Byte[] asciiByteArray = new Byte[asciiBufferLength];
                            Marshal.Copy(pAsciiBytecodeBuffer, asciiByteArray, 0, asciiBufferLength);
                            Marshal.FreeHGlobal(pAsciiBytecodeBuffer);
                            spvBytecodePS_AsciiText = System.Text.Encoding.UTF8.GetString(asciiByteArray);
                        }

                        //convert the bytecode to GLSL
                        {
                            int bufferLen = 0;
                            IntPtr pBuffer = XkslangDLLBindingClass.ConvertBytecodeToGlsl(spvBytecodePS, spvBytecodePS.Length, out bufferLen);
                            if (pBuffer == null || bufferLen < 0) throw new Exception("Failed to convert the PS bytecode to GLSL");

                            Byte[] byteArray = new Byte[bufferLen];
                            Marshal.Copy(pBuffer, byteArray, 0, bufferLen);
                            Marshal.FreeHGlobal(pBuffer);
                            shaderGlslPS = System.Text.Encoding.UTF8.GetString(byteArray);
                        }

                        //convert the bytecode to HLSL
                        {
                            int bufferLen = 0;
                            IntPtr pBuffer = XkslangDLLBindingClass.ConvertBytecodeToHlsl(spvBytecodePS, spvBytecodePS.Length, hlslShaderModel, out bufferLen);
                            if (pBuffer == IntPtr.Zero || bufferLen < 0) throw new Exception("Failed to convert the PS bytecode to HLSL");

                            Byte[] byteArray = new Byte[bufferLen];
                            Marshal.Copy(pBuffer, byteArray, 0, bufferLen);
                            Marshal.FreeHGlobal(pBuffer);
                            shaderHlslPS = System.Text.Encoding.UTF8.GetString(byteArray);
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

                        success = XkslangDLLBindingClass.GetMixerEffectReflectionData(mixerHandleId,
                            out pAllocsConstantBuffers, out countConstantBuffers,
                            out pAllocsResourceBindings, out countResourceBindings,
                            out pAllocsInputAttributes, out countInputAttributes);
                        if (!success) throw new Exception("Failed to get the Effect Reflection data");

                        //Process the ResourceBindings
                        if (countResourceBindings > 0 && pAllocsResourceBindings != IntPtr.Zero)
                        {
                            int structSize = Marshal.SizeOf(typeof(XkslangDLLBindingClass.EffectResourceBindingDescriptionData));
                            XkslangDLLBindingClass.EffectResourceBindingDescriptionData effectResourceBinding;
                            for (int i = 0; i < countResourceBindings; i++)
                            {
                                effectResourceBinding = (XkslangDLLBindingClass.EffectResourceBindingDescriptionData)Marshal.PtrToStructure(
                                    new IntPtr(pAllocsResourceBindings.ToInt32() + (structSize * i)), typeof(XkslangDLLBindingClass.EffectResourceBindingDescriptionData));

                                string keyName = Marshal.PtrToStringAnsi(effectResourceBinding.KeyName);
                                string rawName = Marshal.PtrToStringAnsi(effectResourceBinding.RawName);
                                var binding = new EffectResourceBindingDescription()
                                {
                                    Stage = ShaderStage.None, //XkslangDLLBindingClass.ConvertShadingStageEnum(effectResourceBinding.Stage), (this will be set by ShaderCompiler)
                                    Class = XkslangDLLBindingClass.ConvertEffectParameterReflectionClassEnum(effectResourceBinding.Class),
                                    Type = XkslangDLLBindingClass.ConvertEffectParameterReflectionTypeEnum(effectResourceBinding.Type),
                                    RawName = rawName,
                                    ResourceGroup = rawName,
                                    KeyInfo =
                                    {
                                        KeyName = keyName,
                                    },
                                };
                                xkslangEffectReflection.ResourceBindings.Add(binding);

                                Marshal.FreeHGlobal(effectResourceBinding.KeyName);
                                Marshal.FreeHGlobal(effectResourceBinding.RawName);
                            }

                            //delete the data allocated on the native code
                            Marshal.FreeHGlobal(pAllocsResourceBindings);
                        }

                        //Process the InputAttributes
                        if (countInputAttributes > 0 && pAllocsInputAttributes != IntPtr.Zero)
                        {
                            int structSize = Marshal.SizeOf(typeof(XkslangDLLBindingClass.ShaderInputAttributeDescriptionData));
                            XkslangDLLBindingClass.ShaderInputAttributeDescriptionData shaderInputAttribute;
                            for (int i = 0; i < countInputAttributes; i++)
                            {
                                shaderInputAttribute = (XkslangDLLBindingClass.ShaderInputAttributeDescriptionData)Marshal.PtrToStructure(
                                    new IntPtr(pAllocsInputAttributes.ToInt32() + (structSize * i)), typeof(XkslangDLLBindingClass.ShaderInputAttributeDescriptionData));

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
                            int structSize = Marshal.SizeOf(typeof(XkslangDLLBindingClass.ConstantBufferReflectionDescriptionData));
                            for (int i = 0; i < countConstantBuffers; i++)
                            {
                                //read the cbuffer data
                                XkslangDLLBindingClass.ConstantBufferReflectionDescriptionData constantBufferData;
                                constantBufferData = (XkslangDLLBindingClass.ConstantBufferReflectionDescriptionData)Marshal.PtrToStructure(
                                    new IntPtr(pAllocsConstantBuffers.ToInt32() + (structSize * i)), typeof(XkslangDLLBindingClass.ConstantBufferReflectionDescriptionData));

                                //process the cbuffer members
                                EffectValueDescription[] cbufferMembers = null;
                                if (constantBufferData.CountMembers > 0)
                                {
                                    cbufferMembers = new EffectValueDescription[constantBufferData.CountMembers];

                                    //read the member data
                                    int memberStructSize = Marshal.SizeOf(typeof(XkslangDLLBindingClass.ConstantBufferMemberReflectionDescriptionData));
                                    for (int m = 0; m < constantBufferData.CountMembers; ++m)
                                    {
                                        XkslangDLLBindingClass.ConstantBufferMemberReflectionDescriptionData memberData;
                                        memberData = (XkslangDLLBindingClass.ConstantBufferMemberReflectionDescriptionData)Marshal.PtrToStructure(
                                            new IntPtr(constantBufferData.Members.ToInt32() + (memberStructSize * m)), typeof(XkslangDLLBindingClass.ConstantBufferMemberReflectionDescriptionData));

                                        string keyName = Marshal.PtrToStringAnsi(memberData.KeyName);
                                        string rawName = Marshal.PtrToStringAnsi(memberData.RawName);
                                        cbufferMembers[m] = new EffectValueDescription()
                                        {
                                            KeyInfo =
                                            {
                                                KeyName = keyName,
                                            },
                                            RawName = rawName,
                                            Offset = memberData.Offset,
                                            Size = memberData.Size,
                                            Type = new EffectTypeDescription()
                                            {
                                                Class = XkslangDLLBindingClass.ConvertEffectParameterReflectionClassEnum(memberData.Class),
                                                Type = XkslangDLLBindingClass.ConvertEffectParameterReflectionTypeEnum(memberData.Type),
                                                RowCount = memberData.RowCount,
                                                ColumnCount = memberData.ColumnCount,
                                                Elements = memberData.ArrayElements,
                                                ElementSize = memberData.Size,
                                                Name = rawName,
                                                Members = null,  //we have memberData.CountMembers, but not the members data yet
                                            },
                                        };

                                        Marshal.FreeHGlobal(memberData.KeyName);
                                        Marshal.FreeHGlobal(memberData.RawName);
                                    }

                                    Marshal.FreeHGlobal(constantBufferData.Members);
                                }

                                //create the cbuffer
                                string cbufferName = Marshal.PtrToStringAnsi(constantBufferData.CbufferName);
                                EffectConstantBufferDescription constantBuffer = new EffectConstantBufferDescription()
                                {
                                    Name = cbufferName,
                                    Size = constantBufferData.Size,
                                    Type = ConstantBufferType.ConstantBuffer,
                                    Members = cbufferMembers,
                                };
                                xkslangEffectReflection.ConstantBuffers.Add(constantBuffer);

                                Marshal.FreeHGlobal(constantBufferData.CbufferName);
                            }

                            Marshal.FreeHGlobal(pAllocsConstantBuffers);
                        }

                    }  //end of: //Query and build the EffectReflection data
                }
                catch (Exception e)
                {
                    string errorMessages = e.Message;

                    IntPtr pErrorMsgs = XkslangDLLBindingClass.GetErrorMessages();
                    if (pErrorMsgs != IntPtr.Zero)
                    {
                        string xkslangErrorMsg = Marshal.PtrToStringAnsi(pErrorMsgs);
                        if (xkslangErrorMsg != null && xkslangErrorMsg.Length > 0) errorMessages = errorMessages + '\n' + xkslangErrorMsg;
                        Marshal.FreeHGlobal(pErrorMsgs);
                    }

                    string[] messages = errorMessages.Split('\n');
                    foreach (string str in messages) if (str.Length > 0) Console.WriteLine(str);
                    throw e;
                }
                finally
                {
                    if (mixerHandleId > 0) XkslangDLLBindingClass.ReleaseSpxShaderMixer(mixerHandleId); //release the mixer handle
                    XkslangDLLBindingClass.ReleaseMixer();
                }

                //Write all generated shaders on the disk
#if SILICONSTUDIO_PLATFORM_WINDOWS_DESKTOP
                {
                    //write the stages SPV ascii and shaders on the disk
                    if (mixinCompiledBytecode_AsciiText != null)
                    {
                        var bytecodeSourceFilename = Path.Combine(logDir, "shader_" + mixinTree.Name.Replace('.', '_') + "_compiled_final.hr.spv");
                        lock (WriterLock)
                        {
                            File.WriteAllText(bytecodeSourceFilename, mixinCompiledBytecode_AsciiText);
                        }
                    }

                    if (spvBytecodeVS_AsciiText != null)
                    {
                        var bytecodeSourceFilename = Path.Combine(logDir, "shader_" + mixinTree.Name.Replace('.', '_') + "_compiled_VS.hr.spv");
                        lock (WriterLock)
                        {
                            File.WriteAllText(bytecodeSourceFilename, spvBytecodeVS_AsciiText);
                        }
                    }
                    if (spvBytecodePS_AsciiText != null)
                    {
                        var bytecodeSourceFilename = Path.Combine(logDir, "shader_" + mixinTree.Name.Replace('.', '_') + "_compiled_PS.hr.spv");
                        lock (WriterLock)
                        {
                            File.WriteAllText(bytecodeSourceFilename, spvBytecodePS_AsciiText);
                        }
                    }
                    if (shaderGlslVS != null)
                    {
                        var bytecodeSourceFilename = Path.Combine(logDir, "shader_" + mixinTree.Name.Replace('.', '_') + "_VS.glsl");
                        lock (WriterLock)
                        {
                            File.WriteAllText(bytecodeSourceFilename, shaderGlslVS);
                        }
                    }
                    if (shaderGlslPS != null)
                    {
                        var bytecodeSourceFilename = Path.Combine(logDir, "shader_" + mixinTree.Name.Replace('.', '_') + "_PS.glsl");
                        lock (WriterLock)
                        {
                            File.WriteAllText(bytecodeSourceFilename, shaderGlslPS);
                        }
                    }
                    if (shaderHlslVS != null)
                    {
                        var bytecodeSourceFilename = Path.Combine(logDir, "shader_" + mixinTree.Name.Replace('.', '_') + "_VS.hlsl");
                        lock (WriterLock)
                        {
                            File.WriteAllText(bytecodeSourceFilename, shaderHlslVS);
                        }
                    }
                    if (shaderHlslPS != null)
                    {
                        var bytecodeSourceFilename = Path.Combine(logDir, "shader_" + mixinTree.Name.Replace('.', '_') + "_PS.hlsl");
                        lock (WriterLock)
                        {
                            File.WriteAllText(bytecodeSourceFilename, shaderHlslPS);
                        }
                    }
                }
#endif

                //=====================================================================================================
                //Compile the generated HLSL shaders
                {
                    var shaderStageBytecodes = new List<ShaderBytecode>();

#if SILICONSTUDIO_PLATFORM_WINDOWS_DESKTOP
                    var stageStringBuilder = new StringBuilder();
#endif

                    Dictionary<ShaderStage, string> entryPoints = new Dictionary<ShaderStage, string>();
                    entryPoints.Add(ShaderStage.Vertex, "main");
                    entryPoints.Add(ShaderStage.Pixel, "main");
                    IShaderCompiler d3dcompiler = new Direct3D.ShaderCompiler();
                    foreach (var stageBinding in entryPoints)
                    {
                        string shaderSource = (stageBinding.Key == ShaderStage.Vertex) ? shaderHlslVS : shaderHlslPS;
                        var result = d3dcompiler.Compile(shaderSource, stageBinding.Value, stageBinding.Key, effectParameters, xkslangEffectReflection, null);

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

                    //Get the list of hashSources loaded
                    HashSourceCollection hashSources = null;
                    if (XkslangDLLBindingClass.ListShaderSourcesLoaded != null && XkslangDLLBindingClass.ListShaderSourcesLoaded.Count > 0)
                    {
                        hashSources = new HashSourceCollection();
                        foreach (var shaderLoaded in XkslangDLLBindingClass.ListShaderSourcesLoaded)
                        {
                            hashSources[shaderLoaded.ShaderName] = shaderLoaded.Hash;
                        }
                    }
                    var bytecode = new EffectBytecode { Reflection = xkslangEffectReflection, HashSources = hashSources };

                    // Remove unused reflection data, as it is entirely resolved at compile time.
                    CleanupReflection(bytecode.Reflection);
                    bytecode.Stages = shaderStageBytecodes.ToArray();

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

                        builder.AppendLine("");
                        builder.AppendLine("/*************************/");
                        builder.AppendLine("//Vertex Stage");
                        builder.Append(shaderHlslVS);

                        builder.AppendLine("");
                        builder.AppendLine("/*************************/");
                        builder.AppendLine("//Pixel Stage");
                        builder.Append(shaderHlslPS);

                        var shaderSourceFilename = Path.Combine(logDir, "shader_" + fullEffectName.Replace('.', '_') + "_fullLogs" + ".hlsl");
                        File.WriteAllText(shaderSourceFilename, builder.ToString());
                    }
#endif

                    return new EffectBytecodeCompilerResult(bytecode, log);
                } //end of compilation

            }  //end of: CompileEffectUsingXkslang

            {
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
                var shaderSourceFilename = Path.Combine(logDir, "shader_" +  fullEffectName.Replace('.', '_') + "_" + shaderId + ".hlsl");
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
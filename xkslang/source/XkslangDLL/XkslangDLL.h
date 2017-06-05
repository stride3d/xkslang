#pragma once

#include <stdio.h>
#include <iostream>
#include <objbase.h>

#include "../Common/define.h"
#include "../SpxMixer/EffectReflection.h"

namespace xkslang
{
    struct OutputStageEntryPoint
    {
        ShadingStageEnum stage;
        const char* entryPointName;
    };

    //Return the error messages after an operation failed
    extern "C" __declspec(dllexport) void GetErrorMessages(char *buffer, int32_t bufferLength);

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
	extern "C" __declspec(dllexport) bool GetMixerEffectReflectionData(uint32_t mixerHandleId, EffectResourceBindingDescription** resourceBindings, int32_t* countResourceBindings);
}


/*
Reference code from C# app


namespace SiliconStudio.Xenko.Shaders.Compiler
{
    public class XkslangDLLBindingClass
    {
        public static ShaderSourceManager ShaderSourceManager { get; set; }

        //=====================================================================================================================
        //callback function: called by the dll to ask the shader data
        public static StringBuilder ShaderSourceLoaderCallback(string shaderName, ref Int32 dataLength)
        {
            dataLength = -1;
            if (ShaderSourceManager == null) return null;

            ShaderSourceManager.ShaderSourceWithHash source = ShaderSourceManager.LoadShaderSource(shaderName);

            StringBuilder shaderSource = new StringBuilder(source.Source);
            dataLength = shaderSource.Length;
            return shaderSource;
        }

        //callback function delegate
        [UnmanagedFunctionPointer(CallingConvention.StdCall)]
        public delegate StringBuilder ShaderSourceLoaderCallbackDelegate(string shaderName, ref Int32 len);

        //Return the error messages after an operation failed
        [DllImport("Xkslang.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern void GetErrorMessages(StringBuilder str, Int32 bufferMaxLen);

        //=====================================================================================================================
        // Parsing functions: parse and convert xksl shaders to SPX bytecode

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
        public static extern IntPtr ConvertXkslShaderToSPX([MarshalAs(UnmanagedType.LPStr)] string shaderName, [MarshalAs(UnmanagedType.FunctionPtr)] ShaderSourceLoaderCallbackDelegate callbackPointer, ref Int32 bytecodeLength);

        //=====================================================================================================================
        //Utility functions to help converting a bytecode to a human-readable ascii file, or to glsl/glsl files
        [DllImport("Xkslang.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr ConvertBytecodeToAsciiText([In] Int32[] bytecode, Int32 bytecodeSize, ref Int32 bytecodeLength);

        [DllImport("Xkslang.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr ConvertBytecodeToGlsl([In] Int32[] bytecode, Int32 bytecodeSize, ref Int32 bytecodeLength);

        [DllImport("Xkslang.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr ConvertBytecodeToHlsl([In] Int32[] bytecode, Int32 bytecodeSize, Int32 shaderModel, ref Int32 bytecodeLength);

        //=====================================================================================================================
        // Mixin functions: Mix SPX shaders to generate SPV bytecode for specific output stages

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

        public enum ShadingStageEnum
        {
            Undefined = -1,
            Vertex = 0,
            Pixel = 1,
            TessControl = 2,
            TessEvaluation = 3,
            Geometry = 4,
            Compute = 5,
        };

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

        [StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi)]
        public struct EffectResourceBindingDescription
        {
            public ShadingStageEnum Stage;
            public int Class; // EffectParameterReflectionClass Class;
            public int Type; // EffectParameterReflectionType Type;
            public string KeyName;
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
        public static extern IntPtr GetMixerCompiledBytecode(UInt32 mixerHandleId, ref Int32 bytecodeLength);

        //Get the mixer compiled bytecode's size
        [DllImport("Xkslang.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern Int32 GetMixerCompiledBytecodeSize(UInt32 mixerHandleId);

        //Ask the mixer to copy the compiled bytecode into the buffer
        [DllImport("Xkslang.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern Int32 CopyMixerCompiledBytecode(UInt32 mixerHandleId, [In] Int32[] bytecodeBuffer, Int32 bufferSize);

        //=====================================================================================================================
        //Get an output stage compiled bytecode
        [DllImport("Xkslang.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr GetMixerCompiledBytecodeForStage(UInt32 mixerHandleId, ShadingStageEnum stage, ref Int32 bytecodeLength);

        //Get an output stage compiled bytecode's size
        [DllImport("Xkslang.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern Int32 GetMixerCompiledBytecodeSizeForStage(UInt32 mixerHandleId, ShadingStageEnum stage);

        //Ask the mixer to copy a stage compiled bytecode into the buffer
        [DllImport("Xkslang.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern Int32 CopyMixerCompiledBytecodeForStage(UInt32 mixerHandleId, ShadingStageEnum stage, [In] Int32[] bytecodeBuffer, Int32 bufferSize);

        //=====================================================================================================================
        //Get the reflection data
        [DllImport("Xkslang.dll", CallingConvention = CallingConvention.Cdecl)]
        [return: MarshalAs(UnmanagedType.I1)]
        public static extern bool GetMixerEffectReflectionData(UInt32 mixerHandleId, out EffectResourceBindingDescription[] resourceBindings, ref Int32 countResourceBindings);
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

        public override TaskOrResult<EffectBytecodeCompilerResult> Compile(ShaderMixinSource mixinTree, EffectCompilerParameters effectParameters, CompilerParameters compilerParameters)
        {
            if (mixinTree.Name == "Effect")
            {
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
                    bool success = true;

                    //initialise xkslang
                    success = XkslangDLLBindingClass.InitializeParser();
                    if (!success) throw new Exception("Error initializing Xkslang");

                    //convert the XKSL shader to SPX bytecode
                    int bytecodeLength = 0;
                    IntPtr pBytecodeBuffer = XkslangDLLBindingClass.ConvertXkslShaderToSPX(shaderName, XkslangDLLBindingClass.ShaderSourceLoaderCallback, ref bytecodeLength);
                    if (pBytecodeBuffer == null || bytecodeLength < 0) throw new Exception("Failed to convert the Xksl shader: " + shaderName);

                    //copy the bytecode and free the object (allocated by the dll)
                    effectSpxBytecode = new Int32[bytecodeLength];
                    Marshal.Copy(pBytecodeBuffer, effectSpxBytecode, 0, bytecodeLength);
                    Marshal.FreeHGlobal(pBytecodeBuffer);

                    //=====================================================================================================================================
                    //Optionnal: convert the bytecode to human readable ascii text
                    {
                        int asciiBufferLength = 0;
                        IntPtr pAsciiBytecodeBuffer = XkslangDLLBindingClass.ConvertBytecodeToAsciiText(effectSpxBytecode, effectSpxBytecode.Length, ref asciiBufferLength);
                        if (pAsciiBytecodeBuffer == null || asciiBufferLength < 0) throw new Exception("Failed to convert the Spx bytecode into Ascii");

                        Byte[] asciiByteArray = new Byte[asciiBufferLength];
                        Marshal.Copy(pAsciiBytecodeBuffer, asciiByteArray, 0, asciiBufferLength);
                        Marshal.FreeHGlobal(pAsciiBytecodeBuffer);
                        effectSpxBytecode_AsciiText = System.Text.Encoding.UTF8.GetString(asciiByteArray);
                    }
                }
                catch (Exception e)
                {
                    //Get the error messages from xkslang DLL
                    StringBuilder errorMsg = new StringBuilder(1024);
                    XkslangDLLBindingClass.GetErrorMessages(errorMsg, errorMsg.Capacity);
                    string errorMessages = e.Message + '\n' + errorMsg;

                    string[] messages = errorMessages.Split('\n');
                    foreach (string str in messages) if (str.Length > 0) Console.WriteLine(str);
                    throw e;
                }
                finally
                {
                    XkslangDLLBindingClass.ReleaseParser();
                }

                //=====================================================================================================================================
                //write the SPX ascii bytecodes on the disk
                if (effectSpxBytecode_AsciiText != null)
                {
#if SILICONSTUDIO_PLATFORM_WINDOWS_DESKTOP
                    var strLogDir = Path.Combine(PlatformFolders.ApplicationBinaryDirectory, "log");
                    if (!Directory.Exists(strLogDir)) Directory.CreateDirectory(strLogDir);

                    //var bytecodeId = ObjectId.FromBytes(Encoding.UTF8.GetBytes(spxAsciiBytecode));
                    //var bytecodeSourceFilename = Path.Combine(strLogDir, "shader_" + mixinTree.Name.Replace('.', '_') + "_" + bytecodeId + ".hr.spv");
                    var bytecodeSourceFilename = Path.Combine(strLogDir, "shader_" + mixinTree.Name.Replace('.', '_') + ".hr.spv");

                    lock (WriterLock) // protect write in case the same shader is created twice
                    {
                        // Write shader before generating to make sure that we are having a trace before compiling it (compiler may crash...etc.)
                        //if (!File.Exists(bytecodeSourceFilename))
                        {
                            File.WriteAllText(bytecodeSourceFilename, effectSpxBytecode_AsciiText);
                        }
                    }
#endif
                }

                //=====================================================================================================================================
                //Mix the effect to generate the output stage SPV bytecode. Hardcoded for now: mix the effect shader class only, for PS and VS stages
                if (effectSpxBytecode != null)
                {
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
                            //IntPtr pBytecodeBuffer = XkslangDLLBindingClass.GetMixerCompiledBytecode(mixerHandleId, ref bytecodeLength);
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
                                IntPtr pAsciiBytecodeBuffer = XkslangDLLBindingClass.ConvertBytecodeToAsciiText(mixinCompiledBytecode, mixinCompiledBytecode.Length, ref asciiBufferLength);
                                if (pAsciiBytecodeBuffer == null || asciiBufferLength < 0) throw new Exception("Failed to convert the bytecode to Ascii");

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
                            //IntPtr pBytecodeBuffer = XkslangDLLBindingClass.GetMixerCompiledBytecodeForStage(mixerHandleId, XkslangDLLBindingClass.ShadingStageEnum.Vertex, ref bytecodeLength);
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
                                IntPtr pAsciiBytecodeBuffer = XkslangDLLBindingClass.ConvertBytecodeToAsciiText(spvBytecodeVS, spvBytecodeVS.Length, ref asciiBufferLength);
                                if (pAsciiBytecodeBuffer == null || asciiBufferLength < 0) throw new Exception("Failed to convert the bytecode to Ascii");

                                Byte[] asciiByteArray = new Byte[asciiBufferLength];
                                Marshal.Copy(pAsciiBytecodeBuffer, asciiByteArray, 0, asciiBufferLength);
                                Marshal.FreeHGlobal(pAsciiBytecodeBuffer);
                                spvBytecodeVS_AsciiText = System.Text.Encoding.UTF8.GetString(asciiByteArray);
                            }

                            //convert the bytecode to GLSL
                            {
                                int bufferLen = 0;
                                IntPtr pBuffer = XkslangDLLBindingClass.ConvertBytecodeToGlsl(spvBytecodeVS, spvBytecodeVS.Length, ref bufferLen);
                                if (pBuffer == null || bufferLen < 0) throw new Exception("Failed to convert the VS bytecode to GLSL");

                                Byte[] byteArray = new Byte[bufferLen];
                                Marshal.Copy(pBuffer, byteArray, 0, bufferLen);
                                Marshal.FreeHGlobal(pBuffer);
                                shaderGlslVS = System.Text.Encoding.UTF8.GetString(byteArray);
                            }

                            //convert the bytecode to HLSL
                            {
                                int bufferLen = 0;
                                IntPtr pBuffer = XkslangDLLBindingClass.ConvertBytecodeToHlsl(spvBytecodeVS, spvBytecodeVS.Length, hlslShaderModel, ref bufferLen);
                                if (pBuffer == null || bufferLen < 0) throw new Exception("Failed to convert the VS bytecode to HLSL");

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
                            //IntPtr pBytecodeBuffer = XkslangDLLBindingClass.GetMixerCompiledBytecodeForStage(mixerHandleId, XkslangDLLBindingClass.ShadingStageEnum.Pixel, ref bytecodeLength);
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
                                IntPtr pAsciiBytecodeBuffer = XkslangDLLBindingClass.ConvertBytecodeToAsciiText(spvBytecodePS, spvBytecodePS.Length, ref asciiBufferLength);
                                if (pAsciiBytecodeBuffer == null || asciiBufferLength < 0) throw new Exception("Failed to convert the PS bytecode to Ascii");

                                Byte[] asciiByteArray = new Byte[asciiBufferLength];
                                Marshal.Copy(pAsciiBytecodeBuffer, asciiByteArray, 0, asciiBufferLength);
                                Marshal.FreeHGlobal(pAsciiBytecodeBuffer);
                                spvBytecodePS_AsciiText = System.Text.Encoding.UTF8.GetString(asciiByteArray);
                            }

                            //convert the bytecode to GLSL
                            {
                                int bufferLen = 0;
                                IntPtr pBuffer = XkslangDLLBindingClass.ConvertBytecodeToGlsl(spvBytecodePS, spvBytecodePS.Length, ref bufferLen);
                                if (pBuffer == null || bufferLen < 0) throw new Exception("Failed to convert the PS bytecode to GLSL");

                                Byte[] byteArray = new Byte[bufferLen];
                                Marshal.Copy(pBuffer, byteArray, 0, bufferLen);
                                Marshal.FreeHGlobal(pBuffer);
                                shaderGlslPS = System.Text.Encoding.UTF8.GetString(byteArray);
                            }

                            //convert the bytecode to HLSL
                            {
                                int bufferLen = 0;
                                IntPtr pBuffer = XkslangDLLBindingClass.ConvertBytecodeToHlsl(spvBytecodePS, spvBytecodePS.Length, hlslShaderModel, ref bufferLen);
                                if (pBuffer == null || bufferLen < 0) throw new Exception("Failed to convert the PS bytecode to HLSL");

                                Byte[] byteArray = new Byte[bufferLen];
                                Marshal.Copy(pBuffer, byteArray, 0, bufferLen);
                                Marshal.FreeHGlobal(pBuffer);
                                shaderHlslPS = System.Text.Encoding.UTF8.GetString(byteArray);
                            }
                        }

                        //Get the reflection data
                        {
                            XkslangDLLBindingClass.EffectResourceBindingDescription[] resourceBindings = null;
                            int countResourceBindings = 0;
                            success = XkslangDLLBindingClass.GetMixerEffectReflectionData(mixerHandleId, out resourceBindings, ref countResourceBindings);
                            if (!success) throw new Exception("Failed to get the Effect Reflection data");
                        }
                    }
                    catch (Exception e)
                    {
                        //Get the error messages from xkslang DLL
                        StringBuilder errorMsg = new StringBuilder(1024);
                        XkslangDLLBindingClass.GetErrorMessages(errorMsg, errorMsg.Capacity);
                        string errorMessages = e.Message + '\n' + errorMsg;

                        string[] messages = errorMessages.Split('\n');
                        foreach (string str in messages) if (str.Length > 0) Console.WriteLine(str);
                        throw e;
                    }
                    finally
                    {
                        if (mixerHandleId > 0) XkslangDLLBindingClass.ReleaseSpxShaderMixer(mixerHandleId);

                        XkslangDLLBindingClass.ReleaseMixer();
                    }

#if SILICONSTUDIO_PLATFORM_WINDOWS_DESKTOP
                    {
                        //write the stages SPV ascii and shaders on the disk
                        var strLogDir = Path.Combine(PlatformFolders.ApplicationBinaryDirectory, "log");
                        if (!Directory.Exists(strLogDir)) Directory.CreateDirectory(strLogDir);

                        if (mixinCompiledBytecode_AsciiText != null)
                        {
                            var bytecodeSourceFilename = Path.Combine(strLogDir, "shader_" + mixinTree.Name.Replace('.', '_') + "_compiled_final.hr.spv");
                            lock (WriterLock)
                            {
                                File.WriteAllText(bytecodeSourceFilename, mixinCompiledBytecode_AsciiText);
                            }
                        }

                        if (spvBytecodeVS_AsciiText != null)
                        {
                            var bytecodeSourceFilename = Path.Combine(strLogDir, "shader_" + mixinTree.Name.Replace('.', '_') + "_compiled_VS.hr.spv");
                            lock (WriterLock)
                            {
                                File.WriteAllText(bytecodeSourceFilename, spvBytecodeVS_AsciiText);
                            }
                        }
                        if (spvBytecodePS_AsciiText != null)
                        {
                            var bytecodeSourceFilename = Path.Combine(strLogDir, "shader_" + mixinTree.Name.Replace('.', '_') + "_compiled_PS.hr.spv");
                            lock (WriterLock)
                            {
                                File.WriteAllText(bytecodeSourceFilename, spvBytecodePS_AsciiText);
                            }
                        }
                        if (shaderGlslVS != null)
                        {
                            var bytecodeSourceFilename = Path.Combine(strLogDir, "shader_" + mixinTree.Name.Replace('.', '_') + "_VS.glsl");
                            lock (WriterLock)
                            {
                                File.WriteAllText(bytecodeSourceFilename, shaderGlslVS);
                            }
                        }
                        if (shaderGlslPS != null)
                        {
                            var bytecodeSourceFilename = Path.Combine(strLogDir, "shader_" + mixinTree.Name.Replace('.', '_') + "_PS.glsl");
                            lock (WriterLock)
                            {
                                File.WriteAllText(bytecodeSourceFilename, shaderGlslPS);
                            }
                        }
                        if (shaderHlslVS != null)
                        {
                            var bytecodeSourceFilename = Path.Combine(strLogDir, "shader_" + mixinTree.Name.Replace('.', '_') + "_VS.hlsl");
                            lock (WriterLock)
                            {
                                File.WriteAllText(bytecodeSourceFilename, shaderHlslVS);
                            }
                        }
                        if (shaderHlslPS != null)
                        {
                            var bytecodeSourceFilename = Path.Combine(strLogDir, "shader_" + mixinTree.Name.Replace('.', '_') + "_PS.hlsl");
                            lock (WriterLock)
                            {
                                File.WriteAllText(bytecodeSourceFilename, shaderHlslPS);
                            }
                        }
                    }
#endif

                } //end of: if (spxBytecode != null)

            }  //end of: //TEST loading and parsing the xksl shader

            var log = new LoggerResult();


*/
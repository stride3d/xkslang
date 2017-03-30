// Copyright (c) 2014-2017 The Khronos Group Inc.
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and/or associated documentation files (the "Materials"),
// to deal in the Materials without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Materials, and to permit persons to whom the
// Materials are furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Materials.
// 
// MODIFICATIONS TO THIS FILE MAY MEAN IT NO LONGER ACCURATELY REFLECTS KHRONOS
// STANDARDS. THE UNMODIFIED, NORMATIVE VERSIONS OF KHRONOS SPECIFICATIONS AND
// HEADER INFORMATION ARE LOCATED AT https://www.khronos.org/registry/ 
// 
// THE MATERIALS ARE PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM,OUT OF OR IN CONNECTION WITH THE MATERIALS OR THE USE OR OTHER DEALINGS
// IN THE MATERIALS.

// This header is automatically generated by the same tool that creates
// the Binary Section of the SPIR-V specification.

// Enumeration tokens for SPIR-V, in various styles:
//   C, C++, C++11, JSON, Lua, Python
// 
// - C will have tokens with a "Spv" prefix, e.g.: SpvSourceLanguageGLSL
// - C++ will have tokens in the "spv" name space, e.g.: spv::SourceLanguageGLSL
// - C++11 will use enum classes in the spv namespace, e.g.: spv::SourceLanguage::GLSL
// - Lua will use tables, e.g.: spv.SourceLanguage.GLSL
// - Python will use dictionaries, e.g.: spv['SourceLanguage']['GLSL']
// 
// Some tokens act like mask values, which can be OR'd together,
// while others are mutually exclusive.  The mask-like ones have
// "Mask" in their name, and a parallel enum that has the shift
// amount (1 << x) for each corresponding enumerant.

#ifndef spirv_HPP
#define spirv_HPP

namespace spv {

typedef unsigned int Id;

#define SPV_VERSION 0x10000
#define SPV_REVISION 10

static const unsigned int MagicNumber = 0x07230203;
static const unsigned int Version = 0x00010000;
static const unsigned int Revision = 10;
static const unsigned int OpCodeMask = 0xffff;
static const unsigned int WordCountShift = 16;

enum SourceLanguage {
    SourceLanguageUnknown = 0,
    SourceLanguageESSL = 1,
    SourceLanguageGLSL = 2,
    SourceLanguageOpenCL_C = 3,
    SourceLanguageOpenCL_CPP = 4,
    SourceLanguageMax = 0x7fffffff,
};

enum ExecutionModel {
    ExecutionModelVertex = 0,
    ExecutionModelTessellationControl = 1,
    ExecutionModelTessellationEvaluation = 2,
    ExecutionModelGeometry = 3,
    ExecutionModelFragment = 4,
    ExecutionModelGLCompute = 5,
    ExecutionModelKernel = 6,
    ExecutionModelMax = 0x7fffffff,
};

enum AddressingModel {
    AddressingModelLogical = 0,
    AddressingModelPhysical32 = 1,
    AddressingModelPhysical64 = 2,
    AddressingModelMax = 0x7fffffff,
};

enum MemoryModel {
    MemoryModelSimple = 0,
    MemoryModelGLSL450 = 1,
    MemoryModelOpenCL = 2,
    MemoryModelMax = 0x7fffffff,
};

enum ExecutionMode {
    ExecutionModeInvocations = 0,
    ExecutionModeSpacingEqual = 1,
    ExecutionModeSpacingFractionalEven = 2,
    ExecutionModeSpacingFractionalOdd = 3,
    ExecutionModeVertexOrderCw = 4,
    ExecutionModeVertexOrderCcw = 5,
    ExecutionModePixelCenterInteger = 6,
    ExecutionModeOriginUpperLeft = 7,
    ExecutionModeOriginLowerLeft = 8,
    ExecutionModeEarlyFragmentTests = 9,
    ExecutionModePointMode = 10,
    ExecutionModeXfb = 11,
    ExecutionModeDepthReplacing = 12,
    ExecutionModeDepthGreater = 14,
    ExecutionModeDepthLess = 15,
    ExecutionModeDepthUnchanged = 16,
    ExecutionModeLocalSize = 17,
    ExecutionModeLocalSizeHint = 18,
    ExecutionModeInputPoints = 19,
    ExecutionModeInputLines = 20,
    ExecutionModeInputLinesAdjacency = 21,
    ExecutionModeTriangles = 22,
    ExecutionModeInputTrianglesAdjacency = 23,
    ExecutionModeQuads = 24,
    ExecutionModeIsolines = 25,
    ExecutionModeOutputVertices = 26,
    ExecutionModeOutputPoints = 27,
    ExecutionModeOutputLineStrip = 28,
    ExecutionModeOutputTriangleStrip = 29,
    ExecutionModeVecTypeHint = 30,
    ExecutionModeContractionOff = 31,
    ExecutionModeMax = 0x7fffffff,
};

enum StorageClass {
    StorageClassUniformConstant = 0,
    StorageClassInput = 1,
    StorageClassUniform = 2,
    StorageClassOutput = 3,
    StorageClassWorkgroup = 4,
    StorageClassCrossWorkgroup = 5,
    StorageClassPrivate = 6,
    StorageClassFunction = 7,
    StorageClassGeneric = 8,
    StorageClassPushConstant = 9,
    StorageClassAtomicCounter = 10,
    StorageClassImage = 11,
    StorageClassMax = 0x7fffffff,
};

enum Dim {
    Dim1D = 0,
    Dim2D = 1,
    Dim3D = 2,
    DimCube = 3,
    DimRect = 4,
    DimBuffer = 5,
    DimSubpassData = 6,
    DimMax = 0x7fffffff,
};

enum SamplerAddressingMode {
    SamplerAddressingModeNone = 0,
    SamplerAddressingModeClampToEdge = 1,
    SamplerAddressingModeClamp = 2,
    SamplerAddressingModeRepeat = 3,
    SamplerAddressingModeRepeatMirrored = 4,
    SamplerAddressingModeMax = 0x7fffffff,
};

enum SamplerFilterMode {
    SamplerFilterModeNearest = 0,
    SamplerFilterModeLinear = 1,
    SamplerFilterModeMax = 0x7fffffff,
};

enum ImageFormat {
    ImageFormatUnknown = 0,
    ImageFormatRgba32f = 1,
    ImageFormatRgba16f = 2,
    ImageFormatR32f = 3,
    ImageFormatRgba8 = 4,
    ImageFormatRgba8Snorm = 5,
    ImageFormatRg32f = 6,
    ImageFormatRg16f = 7,
    ImageFormatR11fG11fB10f = 8,
    ImageFormatR16f = 9,
    ImageFormatRgba16 = 10,
    ImageFormatRgb10A2 = 11,
    ImageFormatRg16 = 12,
    ImageFormatRg8 = 13,
    ImageFormatR16 = 14,
    ImageFormatR8 = 15,
    ImageFormatRgba16Snorm = 16,
    ImageFormatRg16Snorm = 17,
    ImageFormatRg8Snorm = 18,
    ImageFormatR16Snorm = 19,
    ImageFormatR8Snorm = 20,
    ImageFormatRgba32i = 21,
    ImageFormatRgba16i = 22,
    ImageFormatRgba8i = 23,
    ImageFormatR32i = 24,
    ImageFormatRg32i = 25,
    ImageFormatRg16i = 26,
    ImageFormatRg8i = 27,
    ImageFormatR16i = 28,
    ImageFormatR8i = 29,
    ImageFormatRgba32ui = 30,
    ImageFormatRgba16ui = 31,
    ImageFormatRgba8ui = 32,
    ImageFormatR32ui = 33,
    ImageFormatRgb10a2ui = 34,
    ImageFormatRg32ui = 35,
    ImageFormatRg16ui = 36,
    ImageFormatRg8ui = 37,
    ImageFormatR16ui = 38,
    ImageFormatR8ui = 39,
    ImageFormatMax = 0x7fffffff,
};

enum ImageChannelOrder {
    ImageChannelOrderR = 0,
    ImageChannelOrderA = 1,
    ImageChannelOrderRG = 2,
    ImageChannelOrderRA = 3,
    ImageChannelOrderRGB = 4,
    ImageChannelOrderRGBA = 5,
    ImageChannelOrderBGRA = 6,
    ImageChannelOrderARGB = 7,
    ImageChannelOrderIntensity = 8,
    ImageChannelOrderLuminance = 9,
    ImageChannelOrderRx = 10,
    ImageChannelOrderRGx = 11,
    ImageChannelOrderRGBx = 12,
    ImageChannelOrderDepth = 13,
    ImageChannelOrderDepthStencil = 14,
    ImageChannelOrdersRGB = 15,
    ImageChannelOrdersRGBx = 16,
    ImageChannelOrdersRGBA = 17,
    ImageChannelOrdersBGRA = 18,
    ImageChannelOrderABGR = 19,
    ImageChannelOrderMax = 0x7fffffff,
};

enum ImageChannelDataType {
    ImageChannelDataTypeSnormInt8 = 0,
    ImageChannelDataTypeSnormInt16 = 1,
    ImageChannelDataTypeUnormInt8 = 2,
    ImageChannelDataTypeUnormInt16 = 3,
    ImageChannelDataTypeUnormShort565 = 4,
    ImageChannelDataTypeUnormShort555 = 5,
    ImageChannelDataTypeUnormInt101010 = 6,
    ImageChannelDataTypeSignedInt8 = 7,
    ImageChannelDataTypeSignedInt16 = 8,
    ImageChannelDataTypeSignedInt32 = 9,
    ImageChannelDataTypeUnsignedInt8 = 10,
    ImageChannelDataTypeUnsignedInt16 = 11,
    ImageChannelDataTypeUnsignedInt32 = 12,
    ImageChannelDataTypeHalfFloat = 13,
    ImageChannelDataTypeFloat = 14,
    ImageChannelDataTypeUnormInt24 = 15,
    ImageChannelDataTypeUnormInt101010_2 = 16,
    ImageChannelDataTypeMax = 0x7fffffff,
};

enum ImageOperandsShift {
    ImageOperandsBiasShift = 0,
    ImageOperandsLodShift = 1,
    ImageOperandsGradShift = 2,
    ImageOperandsConstOffsetShift = 3,
    ImageOperandsOffsetShift = 4,
    ImageOperandsConstOffsetsShift = 5,
    ImageOperandsSampleShift = 6,
    ImageOperandsMinLodShift = 7,
    ImageOperandsMax = 0x7fffffff,
};

enum ImageOperandsMask {
    ImageOperandsMaskNone = 0,
    ImageOperandsBiasMask = 0x00000001,
    ImageOperandsLodMask = 0x00000002,
    ImageOperandsGradMask = 0x00000004,
    ImageOperandsConstOffsetMask = 0x00000008,
    ImageOperandsOffsetMask = 0x00000010,
    ImageOperandsConstOffsetsMask = 0x00000020,
    ImageOperandsSampleMask = 0x00000040,
    ImageOperandsMinLodMask = 0x00000080,
};

enum FPFastMathModeShift {
    FPFastMathModeNotNaNShift = 0,
    FPFastMathModeNotInfShift = 1,
    FPFastMathModeNSZShift = 2,
    FPFastMathModeAllowRecipShift = 3,
    FPFastMathModeFastShift = 4,
    FPFastMathModeMax = 0x7fffffff,
};

enum FPFastMathModeMask {
    FPFastMathModeMaskNone = 0,
    FPFastMathModeNotNaNMask = 0x00000001,
    FPFastMathModeNotInfMask = 0x00000002,
    FPFastMathModeNSZMask = 0x00000004,
    FPFastMathModeAllowRecipMask = 0x00000008,
    FPFastMathModeFastMask = 0x00000010,
};

enum FPRoundingMode {
    FPRoundingModeRTE = 0,
    FPRoundingModeRTZ = 1,
    FPRoundingModeRTP = 2,
    FPRoundingModeRTN = 3,
    FPRoundingModeMax = 0x7fffffff,
};

enum LinkageType {
    LinkageTypeExport = 0,
    LinkageTypeImport = 1,
    LinkageTypeMax = 0x7fffffff,
};

enum AccessQualifier {
    AccessQualifierReadOnly = 0,
    AccessQualifierWriteOnly = 1,
    AccessQualifierReadWrite = 2,
    AccessQualifierMax = 0x7fffffff,
};

enum FunctionParameterAttribute {
    FunctionParameterAttributeZext = 0,
    FunctionParameterAttributeSext = 1,
    FunctionParameterAttributeByVal = 2,
    FunctionParameterAttributeSret = 3,
    FunctionParameterAttributeNoAlias = 4,
    FunctionParameterAttributeNoCapture = 5,
    FunctionParameterAttributeNoWrite = 6,
    FunctionParameterAttributeNoReadWrite = 7,
    FunctionParameterAttributeMax = 0x7fffffff,
};

enum Decoration {
    DecorationRelaxedPrecision = 0,
    DecorationSpecId = 1,
    DecorationBlock = 2,
    DecorationBufferBlock = 3,
    DecorationRowMajor = 4,
    DecorationColMajor = 5,
    DecorationArrayStride = 6,
    DecorationMatrixStride = 7,
    DecorationGLSLShared = 8,
    DecorationGLSLPacked = 9,
    DecorationCPacked = 10,
    DecorationBuiltIn = 11,
    DecorationNoPerspective = 13,
    DecorationFlat = 14,
    DecorationPatch = 15,
    DecorationCentroid = 16,
    DecorationSample = 17,
    DecorationInvariant = 18,
    DecorationRestrict = 19,
    DecorationAliased = 20,
    DecorationVolatile = 21,
    DecorationConstant = 22,
    DecorationCoherent = 23,
    DecorationNonWritable = 24,
    DecorationNonReadable = 25,
    DecorationUniform = 26,
    DecorationSaturatedConversion = 28,
    DecorationStream = 29,
    DecorationLocation = 30,
    DecorationComponent = 31,
    DecorationIndex = 32,
    DecorationBinding = 33,
    DecorationDescriptorSet = 34,
    DecorationOffset = 35,
    DecorationXfbBuffer = 36,
    DecorationXfbStride = 37,
    DecorationFuncParamAttr = 38,
    DecorationFPRoundingMode = 39,
    DecorationFPFastMathMode = 40,
    DecorationLinkageAttributes = 41,
    DecorationNoContraction = 42,
    DecorationInputAttachmentIndex = 43,
    DecorationAlignment = 44,
    DecorationOverrideCoverageNV = 5248,
    DecorationPassthroughNV = 5250,
    DecorationViewportRelativeNV = 5252,
    DecorationSecondaryViewportRelativeNV = 5256,
    DecorationMax = 0x7fffffff,
};

enum BuiltIn {
    BuiltInPosition = 0,
    BuiltInPointSize = 1,
    BuiltInClipDistance = 3,
    BuiltInCullDistance = 4,
    BuiltInVertexId = 5,
    BuiltInInstanceId = 6,
    BuiltInPrimitiveId = 7,
    BuiltInInvocationId = 8,
    BuiltInLayer = 9,
    BuiltInViewportIndex = 10,
    BuiltInTessLevelOuter = 11,
    BuiltInTessLevelInner = 12,
    BuiltInTessCoord = 13,
    BuiltInPatchVertices = 14,
    BuiltInFragCoord = 15,
    BuiltInPointCoord = 16,
    BuiltInFrontFacing = 17,
    BuiltInSampleId = 18,
    BuiltInSamplePosition = 19,
    BuiltInSampleMask = 20,
    BuiltInFragDepth = 22,
    BuiltInHelperInvocation = 23,
    BuiltInNumWorkgroups = 24,
    BuiltInWorkgroupSize = 25,
    BuiltInWorkgroupId = 26,
    BuiltInLocalInvocationId = 27,
    BuiltInGlobalInvocationId = 28,
    BuiltInLocalInvocationIndex = 29,
    BuiltInWorkDim = 30,
    BuiltInGlobalSize = 31,
    BuiltInEnqueuedWorkgroupSize = 32,
    BuiltInGlobalOffset = 33,
    BuiltInGlobalLinearId = 34,
    BuiltInSubgroupSize = 36,
    BuiltInSubgroupMaxSize = 37,
    BuiltInNumSubgroups = 38,
    BuiltInNumEnqueuedSubgroups = 39,
    BuiltInSubgroupId = 40,
    BuiltInSubgroupLocalInvocationId = 41,
    BuiltInVertexIndex = 42,
    BuiltInInstanceIndex = 43,
    BuiltInSubgroupEqMaskKHR = 4416,
    BuiltInSubgroupGeMaskKHR = 4417,
    BuiltInSubgroupGtMaskKHR = 4418,
    BuiltInSubgroupLeMaskKHR = 4419,
    BuiltInSubgroupLtMaskKHR = 4420,
    BuiltInBaseVertex = 4424,
    BuiltInBaseInstance = 4425,
    BuiltInDrawIndex = 4426,
    BuiltInDeviceIndex = 4438,
    BuiltInViewIndex = 4440,
    BuiltInViewportMaskNV = 5253,
    BuiltInSecondaryPositionNV = 5257,
    BuiltInSecondaryViewportMaskNV = 5258,
    BuiltInPositionPerViewNV = 5261,
    BuiltInViewportMaskPerViewNV = 5262,
    BuiltInMax = 0x7fffffff,
};

enum SelectionControlShift {
    SelectionControlFlattenShift = 0,
    SelectionControlDontFlattenShift = 1,
    SelectionControlMax = 0x7fffffff,
};

enum SelectionControlMask {
    SelectionControlMaskNone = 0,
    SelectionControlFlattenMask = 0x00000001,
    SelectionControlDontFlattenMask = 0x00000002,
};

enum LoopControlShift {
    LoopControlUnrollShift = 0,
    LoopControlDontUnrollShift = 1,
    LoopControlMax = 0x7fffffff,
};

enum LoopControlMask {
    LoopControlMaskNone = 0,
    LoopControlUnrollMask = 0x00000001,
    LoopControlDontUnrollMask = 0x00000002,
};

enum FunctionControlShift {
    FunctionControlInlineShift = 0,
    FunctionControlDontInlineShift = 1,
    FunctionControlPureShift = 2,
    FunctionControlConstShift = 3,
    FunctionControlMax = 0x7fffffff,
};

enum FunctionControlMask {
    FunctionControlMaskNone = 0,
    FunctionControlInlineMask = 0x00000001,
    FunctionControlDontInlineMask = 0x00000002,
    FunctionControlPureMask = 0x00000004,
    FunctionControlConstMask = 0x00000008,
};

enum MemorySemanticsShift {
    MemorySemanticsAcquireShift = 1,
    MemorySemanticsReleaseShift = 2,
    MemorySemanticsAcquireReleaseShift = 3,
    MemorySemanticsSequentiallyConsistentShift = 4,
    MemorySemanticsUniformMemoryShift = 6,
    MemorySemanticsSubgroupMemoryShift = 7,
    MemorySemanticsWorkgroupMemoryShift = 8,
    MemorySemanticsCrossWorkgroupMemoryShift = 9,
    MemorySemanticsAtomicCounterMemoryShift = 10,
    MemorySemanticsImageMemoryShift = 11,
    MemorySemanticsMax = 0x7fffffff,
};

enum MemorySemanticsMask {
    MemorySemanticsMaskNone = 0,
    MemorySemanticsAcquireMask = 0x00000002,
    MemorySemanticsReleaseMask = 0x00000004,
    MemorySemanticsAcquireReleaseMask = 0x00000008,
    MemorySemanticsSequentiallyConsistentMask = 0x00000010,
    MemorySemanticsUniformMemoryMask = 0x00000040,
    MemorySemanticsSubgroupMemoryMask = 0x00000080,
    MemorySemanticsWorkgroupMemoryMask = 0x00000100,
    MemorySemanticsCrossWorkgroupMemoryMask = 0x00000200,
    MemorySemanticsAtomicCounterMemoryMask = 0x00000400,
    MemorySemanticsImageMemoryMask = 0x00000800,
};

enum MemoryAccessShift {
    MemoryAccessVolatileShift = 0,
    MemoryAccessAlignedShift = 1,
    MemoryAccessNontemporalShift = 2,
    MemoryAccessMax = 0x7fffffff,
};

enum MemoryAccessMask {
    MemoryAccessMaskNone = 0,
    MemoryAccessVolatileMask = 0x00000001,
    MemoryAccessAlignedMask = 0x00000002,
    MemoryAccessNontemporalMask = 0x00000004,
};

enum Scope {
    ScopeCrossDevice = 0,
    ScopeDevice = 1,
    ScopeWorkgroup = 2,
    ScopeSubgroup = 3,
    ScopeInvocation = 4,
    ScopeMax = 0x7fffffff,
};

enum GroupOperation {
    GroupOperationReduce = 0,
    GroupOperationInclusiveScan = 1,
    GroupOperationExclusiveScan = 2,
    GroupOperationMax = 0x7fffffff,
};

enum KernelEnqueueFlags {
    KernelEnqueueFlagsNoWait = 0,
    KernelEnqueueFlagsWaitKernel = 1,
    KernelEnqueueFlagsWaitWorkGroup = 2,
    KernelEnqueueFlagsMax = 0x7fffffff,
};

enum KernelProfilingInfoShift {
    KernelProfilingInfoCmdExecTimeShift = 0,
    KernelProfilingInfoMax = 0x7fffffff,
};

enum KernelProfilingInfoMask {
    KernelProfilingInfoMaskNone = 0,
    KernelProfilingInfoCmdExecTimeMask = 0x00000001,
};

enum Capability {
    CapabilityMatrix = 0,
    CapabilityShader = 1,
    CapabilityGeometry = 2,
    CapabilityTessellation = 3,
    CapabilityAddresses = 4,
    CapabilityLinkage = 5,
    CapabilityKernel = 6,
    CapabilityVector16 = 7,
    CapabilityFloat16Buffer = 8,
    CapabilityFloat16 = 9,
    CapabilityFloat64 = 10,
    CapabilityInt64 = 11,
    CapabilityInt64Atomics = 12,
    CapabilityImageBasic = 13,
    CapabilityImageReadWrite = 14,
    CapabilityImageMipmap = 15,
    CapabilityPipes = 17,
    CapabilityGroups = 18,
    CapabilityDeviceEnqueue = 19,
    CapabilityLiteralSampler = 20,
    CapabilityAtomicStorage = 21,
    CapabilityInt16 = 22,
    CapabilityTessellationPointSize = 23,
    CapabilityGeometryPointSize = 24,
    CapabilityImageGatherExtended = 25,
    CapabilityStorageImageMultisample = 27,
    CapabilityUniformBufferArrayDynamicIndexing = 28,
    CapabilitySampledImageArrayDynamicIndexing = 29,
    CapabilityStorageBufferArrayDynamicIndexing = 30,
    CapabilityStorageImageArrayDynamicIndexing = 31,
    CapabilityClipDistance = 32,
    CapabilityCullDistance = 33,
    CapabilityImageCubeArray = 34,
    CapabilitySampleRateShading = 35,
    CapabilityImageRect = 36,
    CapabilitySampledRect = 37,
    CapabilityGenericPointer = 38,
    CapabilityInt8 = 39,
    CapabilityInputAttachment = 40,
    CapabilitySparseResidency = 41,
    CapabilityMinLod = 42,
    CapabilitySampled1D = 43,
    CapabilityImage1D = 44,
    CapabilitySampledCubeArray = 45,
    CapabilitySampledBuffer = 46,
    CapabilityImageBuffer = 47,
    CapabilityImageMSArray = 48,
    CapabilityStorageImageExtendedFormats = 49,
    CapabilityImageQuery = 50,
    CapabilityDerivativeControl = 51,
    CapabilityInterpolationFunction = 52,
    CapabilityTransformFeedback = 53,
    CapabilityGeometryStreams = 54,
    CapabilityStorageImageReadWithoutFormat = 55,
    CapabilityStorageImageWriteWithoutFormat = 56,
    CapabilityMultiViewport = 57,
    CapabilitySubgroupBallotKHR = 4423,
    CapabilityDrawParameters = 4427,
    CapabilitySubgroupVoteKHR = 4431,
    CapabilityStorageUniformBufferBlock16 = 4433,
    CapabilityStorageUniform16 = 4434,
    CapabilityStoragePushConstant16 = 4435,
    CapabilityStorageInputOutput16 = 4436,
    CapabilityDeviceGroup = 4437,
    CapabilityMultiView = 4439,
    CapabilitySampleMaskOverrideCoverageNV = 5249,
    CapabilityGeometryShaderPassthroughNV = 5251,
    CapabilityShaderViewportIndexLayerNV = 5254,
    CapabilityShaderViewportMaskNV = 5255,
    CapabilityShaderStereoViewNV = 5259,
    CapabilityPerViewAttributesNV = 5260,
    CapabilityMax = 0x7fffffff,
};

enum XkslPropertyEnum {
    PropertyStage = 1,              //member or method is a stage
    PropertyStream = 2,             //member or method is a stream
    PropertyMethodAbstract = 3,     //abstract method
    PropertyMethodOverride = 4,     //override method
    PropertyMethodClone = 5,        //clone method
    PropertyMethodStatic = 6,       //static method
    CBufferUndefined = 7,           //the cbuffer is undefined and global to a shader class
    CBufferDefined = 8,             //the cbuffer has been defined within the shader class
    CBufferStage = 9,               //stage cbuffer
    CBufferUnstage = 10,            //normal cbuffer

    GSInputPoints = 11,
    GSInputLines = 12,
    GSInputTriangles = 13,
    GSInputLinesAdjacency = 14,
    GSInputTrianglesAdjacency = 15,
    GSOutputPointStream = 16,
    GSOutputLineStream = 17,
    GSOutputTriangleStream = 18,
};

enum Op {
    OpNop = 0,
    OpUndef = 1,
    OpSourceContinued = 2,
    OpSource = 3,
    OpSourceExtension = 4,
    OpName = 5,
    OpMemberName = 6,
    OpString = 7,
    OpLine = 8,
    OpExtension = 10,
    OpExtInstImport = 11,
    OpExtInst = 12,
    OpMemoryModel = 14,
    OpEntryPoint = 15,
    OpExecutionMode = 16,
    OpCapability = 17,

	//New OpType for XKSL extensions
	//set at 18 because with current code all types must appear before OpConstant = 43
	OpTypeXlslShaderClass = 18,

    OpTypeVoid = 19,
    OpTypeBool = 20,
    OpTypeInt = 21,
    OpTypeFloat = 22,
    OpTypeVector = 23,
    OpTypeMatrix = 24,
    OpTypeImage = 25,
    OpTypeSampler = 26,
    OpTypeSampledImage = 27,
    OpTypeArray = 28,
    OpTypeRuntimeArray = 29,
    OpTypeStruct = 30,
    OpTypeOpaque = 31,
    OpTypePointer = 32,
    OpTypeFunction = 33,
    OpTypeEvent = 34,
    OpTypeDeviceEvent = 35,
    OpTypeReserveId = 36,
    OpTypeQueue = 37,
    OpTypePipe = 38,
    OpTypeForwardPointer = 39,
    OpConstantTrue = 41,
    OpConstantFalse = 42,
    OpConstant = 43,
    OpConstantComposite = 44,
    OpConstantSampler = 45,
    OpConstantNull = 46,
    OpSpecConstantTrue = 48,
    OpSpecConstantFalse = 49,
    OpSpecConstant = 50,
    OpSpecConstantComposite = 51,
    OpSpecConstantOp = 52,
    OpFunction = 54,
    OpFunctionParameter = 55,
    OpFunctionEnd = 56,
    OpFunctionCall = 57,
    OpVariable = 59,
    OpImageTexelPointer = 60,
    OpLoad = 61,
    OpStore = 62,
    OpCopyMemory = 63,
    OpCopyMemorySized = 64,
    OpAccessChain = 65,
    OpInBoundsAccessChain = 66,
    OpPtrAccessChain = 67,
    OpArrayLength = 68,
    OpGenericPtrMemSemantics = 69,
    OpInBoundsPtrAccessChain = 70,
    OpDecorate = 71,
    OpMemberDecorate = 72,
    OpDecorationGroup = 73,
    OpGroupDecorate = 74,
    OpGroupMemberDecorate = 75,
    OpVectorExtractDynamic = 77,
    OpVectorInsertDynamic = 78,
    OpVectorShuffle = 79,
    OpCompositeConstruct = 80,
    OpCompositeExtract = 81,
    OpCompositeInsert = 82,
    OpCopyObject = 83,
    OpTranspose = 84,
    OpSampledImage = 86,
    OpImageSampleImplicitLod = 87,
    OpImageSampleExplicitLod = 88,
    OpImageSampleDrefImplicitLod = 89,
    OpImageSampleDrefExplicitLod = 90,
    OpImageSampleProjImplicitLod = 91,
    OpImageSampleProjExplicitLod = 92,
    OpImageSampleProjDrefImplicitLod = 93,
    OpImageSampleProjDrefExplicitLod = 94,
    OpImageFetch = 95,
    OpImageGather = 96,
    OpImageDrefGather = 97,
    OpImageRead = 98,
    OpImageWrite = 99,
    OpImage = 100,
    OpImageQueryFormat = 101,
    OpImageQueryOrder = 102,
    OpImageQuerySizeLod = 103,
    OpImageQuerySize = 104,
    OpImageQueryLod = 105,
    OpImageQueryLevels = 106,
    OpImageQuerySamples = 107,
    OpConvertFToU = 109,
    OpConvertFToS = 110,
    OpConvertSToF = 111,
    OpConvertUToF = 112,
    OpUConvert = 113,
    OpSConvert = 114,
    OpFConvert = 115,
    OpQuantizeToF16 = 116,
    OpConvertPtrToU = 117,
    OpSatConvertSToU = 118,
    OpSatConvertUToS = 119,
    OpConvertUToPtr = 120,
    OpPtrCastToGeneric = 121,
    OpGenericCastToPtr = 122,
    OpGenericCastToPtrExplicit = 123,
    OpBitcast = 124,
    OpSNegate = 126,
    OpFNegate = 127,
    OpIAdd = 128,
    OpFAdd = 129,
    OpISub = 130,
    OpFSub = 131,
    OpIMul = 132,
    OpFMul = 133,
    OpUDiv = 134,
    OpSDiv = 135,
    OpFDiv = 136,
    OpUMod = 137,
    OpSRem = 138,
    OpSMod = 139,
    OpFRem = 140,
    OpFMod = 141,
    OpVectorTimesScalar = 142,
    OpMatrixTimesScalar = 143,
    OpVectorTimesMatrix = 144,
    OpMatrixTimesVector = 145,
    OpMatrixTimesMatrix = 146,
    OpOuterProduct = 147,
    OpDot = 148,
    OpIAddCarry = 149,
    OpISubBorrow = 150,
    OpUMulExtended = 151,
    OpSMulExtended = 152,
    OpAny = 154,
    OpAll = 155,
    OpIsNan = 156,
    OpIsInf = 157,
    OpIsFinite = 158,
    OpIsNormal = 159,
    OpSignBitSet = 160,
    OpLessOrGreater = 161,
    OpOrdered = 162,
    OpUnordered = 163,
    OpLogicalEqual = 164,
    OpLogicalNotEqual = 165,
    OpLogicalOr = 166,
    OpLogicalAnd = 167,
    OpLogicalNot = 168,
    OpSelect = 169,
    OpIEqual = 170,
    OpINotEqual = 171,
    OpUGreaterThan = 172,
    OpSGreaterThan = 173,
    OpUGreaterThanEqual = 174,
    OpSGreaterThanEqual = 175,
    OpULessThan = 176,
    OpSLessThan = 177,
    OpULessThanEqual = 178,
    OpSLessThanEqual = 179,
    OpFOrdEqual = 180,
    OpFUnordEqual = 181,
    OpFOrdNotEqual = 182,
    OpFUnordNotEqual = 183,
    OpFOrdLessThan = 184,
    OpFUnordLessThan = 185,
    OpFOrdGreaterThan = 186,
    OpFUnordGreaterThan = 187,
    OpFOrdLessThanEqual = 188,
    OpFUnordLessThanEqual = 189,
    OpFOrdGreaterThanEqual = 190,
    OpFUnordGreaterThanEqual = 191,
    OpShiftRightLogical = 194,
    OpShiftRightArithmetic = 195,
    OpShiftLeftLogical = 196,
    OpBitwiseOr = 197,
    OpBitwiseXor = 198,
    OpBitwiseAnd = 199,
    OpNot = 200,
    OpBitFieldInsert = 201,
    OpBitFieldSExtract = 202,
    OpBitFieldUExtract = 203,
    OpBitReverse = 204,
    OpBitCount = 205,
    OpDPdx = 207,
    OpDPdy = 208,
    OpFwidth = 209,
    OpDPdxFine = 210,
    OpDPdyFine = 211,
    OpFwidthFine = 212,
    OpDPdxCoarse = 213,
    OpDPdyCoarse = 214,
    OpFwidthCoarse = 215,
    OpEmitVertex = 218,
    OpEndPrimitive = 219,
    OpEmitStreamVertex = 220,
    OpEndStreamPrimitive = 221,
    OpControlBarrier = 224,
    OpMemoryBarrier = 225,
    OpAtomicLoad = 227,
    OpAtomicStore = 228,
    OpAtomicExchange = 229,
    OpAtomicCompareExchange = 230,
    OpAtomicCompareExchangeWeak = 231,
    OpAtomicIIncrement = 232,
    OpAtomicIDecrement = 233,
    OpAtomicIAdd = 234,
    OpAtomicISub = 235,
    OpAtomicSMin = 236,
    OpAtomicUMin = 237,
    OpAtomicSMax = 238,
    OpAtomicUMax = 239,
    OpAtomicAnd = 240,
    OpAtomicOr = 241,
    OpAtomicXor = 242,
    OpPhi = 245,
    OpLoopMerge = 246,
    OpSelectionMerge = 247,
    OpLabel = 248,
    OpBranch = 249,
    OpBranchConditional = 250,
    OpSwitch = 251,
    OpKill = 252,
    OpReturn = 253,
    OpReturnValue = 254,
    OpUnreachable = 255,
    OpLifetimeStart = 256,
    OpLifetimeStop = 257,
    OpGroupAsyncCopy = 259,
    OpGroupWaitEvents = 260,
    OpGroupAll = 261,
    OpGroupAny = 262,
    OpGroupBroadcast = 263,
    OpGroupIAdd = 264,
    OpGroupFAdd = 265,
    OpGroupFMin = 266,
    OpGroupUMin = 267,
    OpGroupSMin = 268,
    OpGroupFMax = 269,
    OpGroupUMax = 270,
    OpGroupSMax = 271,
    OpReadPipe = 274,
    OpWritePipe = 275,
    OpReservedReadPipe = 276,
    OpReservedWritePipe = 277,
    OpReserveReadPipePackets = 278,
    OpReserveWritePipePackets = 279,
    OpCommitReadPipe = 280,
    OpCommitWritePipe = 281,
    OpIsValidReserveId = 282,
    OpGetNumPipePackets = 283,
    OpGetMaxPipePackets = 284,
    OpGroupReserveReadPipePackets = 285,
    OpGroupReserveWritePipePackets = 286,
    OpGroupCommitReadPipe = 287,
    OpGroupCommitWritePipe = 288,
    OpEnqueueMarker = 291,
    OpEnqueueKernel = 292,
    OpGetKernelNDrangeSubGroupCount = 293,
    OpGetKernelNDrangeMaxSubGroupSize = 294,
    OpGetKernelWorkGroupSize = 295,
    OpGetKernelPreferredWorkGroupSizeMultiple = 296,
    OpRetainEvent = 297,
    OpReleaseEvent = 298,
    OpCreateUserEvent = 299,
    OpIsValidEvent = 300,
    OpSetUserEventStatus = 301,
    OpCaptureEventProfilingInfo = 302,
    OpGetDefaultQueue = 303,
    OpBuildNDRange = 304,
    OpImageSparseSampleImplicitLod = 305,
    OpImageSparseSampleExplicitLod = 306,
    OpImageSparseSampleDrefImplicitLod = 307,
    OpImageSparseSampleDrefExplicitLod = 308,
    OpImageSparseSampleProjImplicitLod = 309,
    OpImageSparseSampleProjExplicitLod = 310,
    OpImageSparseSampleProjDrefImplicitLod = 311,
    OpImageSparseSampleProjDrefExplicitLod = 312,
    OpImageSparseFetch = 313,
    OpImageSparseGather = 314,
    OpImageSparseDrefGather = 315,
    OpImageSparseTexelsResident = 316,
    OpNoLine = 317,
    OpAtomicFlagTestAndSet = 318,
    OpAtomicFlagClear = 319,
    OpImageSparseRead = 320,
    OpSubgroupBallotKHR = 4421,
    OpSubgroupFirstInvocationKHR = 4422,
    OpSubgroupAllKHR = 4428,
    OpSubgroupAnyKHR = 4429,
    OpSubgroupAllEqualKHR = 4430,
    OpSubgroupReadInvocationKHR = 4432,
    
    //================================================================================================
    //XKSL extensions
    OpDeclarationName = 4600,                //declaration name for shader, shaders' functions and shaders' block types
    OpShaderInheritance = 4601,              //List of a shader inheritance
    OpBelongsToShader = 4602,                //Link the functions and types with the shader that created them
    OpShaderCompositionDeclaration = 4603,   //Declare a composition within a shader. [OpShaderCompositionDeclaration shaderOwnerId compositionNum compositionShaderTypeId isArray countInstance name]
    OpShaderCompositionInstance = 4604,      //Declare a composition instance. [OpShaderCompositionInstance shaderOwnerId compositionNum instanceNum instanceShaderId]
    OpMethodProperties = 4605,               //Add XKSL properties to a method: cf XkslPropertyEnum
    OpMemberProperties = 4606,               //Add XKSL properties to a member: cf XkslPropertyEnum
    OpMemberSemanticName = 4607,             //Record the semantic name as declared by the user
    OpCBufferMemberProperties = 4608,        //Additionnal data when defining a cbuffer. cbufferType (cf XkslPropertyEnum), cbufferStage (cf XkslPropertyEnum), member counts, size And alignment for each member
    OpGSMethodProperties = 4609,             //Additonnal data for GS methods. GSInputType GSOuputType

    OpForEachCompositionStartLoop = 4620,    //start a foreach loop: 2 first params define the array composition targeted (shaderId then compositionId)
    OpForEachCompositionEndLoop = 46021,

    OpFunctionCallBaseUnresolved = 4630,              //an unresolved function call with base accessor
    OpFunctionCallBaseResolved = 4631,                //a resolved function call with base accessor
    OpFunctionCallThroughCompositionVariable = 4632,  //a function is called through a composition variable
    //================================================================================================

    OpMax = 0x7fffffff,
};

// Overload operator| for mask bit combining

inline ImageOperandsMask operator|(ImageOperandsMask a, ImageOperandsMask b) { return ImageOperandsMask(unsigned(a) | unsigned(b)); }
inline FPFastMathModeMask operator|(FPFastMathModeMask a, FPFastMathModeMask b) { return FPFastMathModeMask(unsigned(a) | unsigned(b)); }
inline SelectionControlMask operator|(SelectionControlMask a, SelectionControlMask b) { return SelectionControlMask(unsigned(a) | unsigned(b)); }
inline LoopControlMask operator|(LoopControlMask a, LoopControlMask b) { return LoopControlMask(unsigned(a) | unsigned(b)); }
inline FunctionControlMask operator|(FunctionControlMask a, FunctionControlMask b) { return FunctionControlMask(unsigned(a) | unsigned(b)); }
inline MemorySemanticsMask operator|(MemorySemanticsMask a, MemorySemanticsMask b) { return MemorySemanticsMask(unsigned(a) | unsigned(b)); }
inline MemoryAccessMask operator|(MemoryAccessMask a, MemoryAccessMask b) { return MemoryAccessMask(unsigned(a) | unsigned(b)); }
inline KernelProfilingInfoMask operator|(KernelProfilingInfoMask a, KernelProfilingInfoMask b) { return KernelProfilingInfoMask(unsigned(a) | unsigned(b)); }

}  // end namespace spv

#endif  // #ifndef spirv_HPP


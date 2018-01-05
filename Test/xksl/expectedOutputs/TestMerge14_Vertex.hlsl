struct ShaderBase_sBase2
{
    int i;
};

struct ShaderBase_sBase1
{
    int i;
};

struct VS_STREAMS
{
    int streamBase_id0;
    int streamA_id1;
};

static int VS_OUT_streamBase;
static int VS_OUT_streamA;

struct SPIRV_Cross_Output
{
    int VS_OUT_streamBase : STREAMBASE;
    int VS_OUT_streamA : STREAMA;
};

void ShaderBase_ComputeBase(inout VS_STREAMS _streams, ShaderBase_sBase1 s1, ShaderBase_sBase2 s2)
{
    _streams.streamBase_id0 = s1.i + s2.i;
}

void ShaderA_Compute(inout VS_STREAMS _streams, ShaderBase_sBase1 s)
{
    ShaderBase_sBase1 sb1 = { s.i };
    ShaderBase_sBase1 sb2 = { s.i };
    ShaderBase_sBase1 param = sb1;
    ShaderBase_sBase1 param_1 = sb2;
    ShaderBase_ComputeBase(_streams, param, param_1);
    _streams.streamA_id1 = 1;
}

void vert_main()
{
    VS_STREAMS _streams = { 0, 0 };
    ShaderBase_sBase1 s = { 1 };
    ShaderBase_sBase1 param = s;
    ShaderA_Compute(_streams, param);
    VS_OUT_streamBase = _streams.streamBase_id0;
    VS_OUT_streamA = _streams.streamA_id1;
}

SPIRV_Cross_Output main()
{
    vert_main();
    SPIRV_Cross_Output stage_output;
    stage_output.VS_OUT_streamBase = VS_OUT_streamBase;
    stage_output.VS_OUT_streamA = VS_OUT_streamA;
    return stage_output;
}

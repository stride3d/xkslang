struct VS_STREAMS
{
    float sbase1_id0;
};

static const VS_STREAMS _32 = { 0.0f };

cbuffer Globals
{
    int ShaderBase_Var0;
    int ShaderBase_Var1;
};

static float VS_IN_SBASE1;

struct SPIRV_Cross_Input
{
    float VS_IN_SBASE1 : SBASE1;
};

void vert_main()
{
    VS_STREAMS _streams = _32;
    _streams.sbase1_id0 = VS_IN_SBASE1;
    int i = int(((_streams.sbase1_id0 + float(ShaderBase_Var0)) + float(ShaderBase_Var1)) + 2.0f);
}

void main(SPIRV_Cross_Input stage_input)
{
    VS_IN_SBASE1 = stage_input.VS_IN_SBASE1;
    vert_main();
}

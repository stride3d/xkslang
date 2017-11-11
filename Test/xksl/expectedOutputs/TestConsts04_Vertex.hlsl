struct VS_STREAMS
{
    float sbase1_id0;
};

cbuffer Globals
{
    int ShaderBase_Var0;
    int ShaderBase_Var1;
};

static float VS_IN_sbase1;

struct SPIRV_Cross_Input
{
    float VS_IN_sbase1 : SBASE1;
};

void vert_main()
{
    VS_STREAMS _streams = { 0.0f };
    _streams.sbase1_id0 = VS_IN_sbase1;
    int i = int(((_streams.sbase1_id0 + float(ShaderBase_Var0)) + float(ShaderBase_Var1)) + 2.0f);
}

void main(SPIRV_Cross_Input stage_input)
{
    VS_IN_sbase1 = stage_input.VS_IN_sbase1;
    vert_main();
}

struct S1 {
    bool b;
    float4 f;
};

static struct {
	float4 f;
    bool b;
} S2;

S1 u_s1;

float process(int i)
{
	return i * S2.f;
}

float4 PixelShaderFunction() : COLOR0
{
	S1 as0;   //as0 is not used, will be removed from AST and SPIRV
	S1 as1;
	S1 as2;
	S1 as3;

	as1.f = 10;
	as1.b = false;
	as2.b = true;
	as3 = as1;

    struct FS {
        bool3 b;
    } as4;

    as3 == as3;
	//u_s1.f = 2;  //invalid: u_s1 is an uniform
    S2.f = as1.f;  //valid: S2 is a static struct (set in global storage)
	
    return S2.f + u_s1.f;
}
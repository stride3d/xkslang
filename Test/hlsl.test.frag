/*cbuffer C1 {
    float toto1;
};

cbuffer {
    float toto2;
	float toto3;
};*/

/*cbuffer cbufName : register(b2, space10) {
    float4 toto3;
    int i3 : packoffset(c1.y);
};*/

cbuffer TOTO
{
	int Var0;
	int Var1;
};
	
int PixelShaderFunction()
{
    return Var1;
}

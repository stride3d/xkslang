/****************************************************************************************
Parse Methods and members' post declaration keyword
Accessibility: public, private, internal, protected: those will be ignored when parsed

Member: stage, stream, static, staging
Method: stage, override, abstract, clone
==> We just parse and add the qualifier attributes in the SPIR-X files (as decorate), to resolve things at compilation time

TODO: Input, Output, Input2, Constants (for geometry and tessellation shaders)
TODO streams: not for declaration (global structure)
*****************************************************************************************/

//initialisations test
int TOTOTO_03 = 6 + 10;   //uniform: ignore initialization
static int TOTOTO_02 = 5 + 10;
static const int TOTOTO_01 = 5 + 9;

/*shader ShaderTestMember
{
	static const int constValue = 5 + 9;
};*/

/*int main(int i, int j, int k, float f)
{
	return TOTOTO;
}

int main2()
{
	TOTOTO += 2;
	int k = main(TOTOTO, TOTOTO + 1, 3, 4 );
	return main(k, k + 1, 3, 4 );
}*/

/*
shader ShaderTestAccessor
{
	public int i0;
	private int i1;
	protected int i2;
	internal int i3;
	
	//make no sense, but to check the parser
	public internal private protected float f;
};
*/

/*
shader ShaderTestMember
{
    // Default SV_POSITION output for VS/GS shaders
    //stage stream float4 ShadingPosition : SV_Position;
	
	static const int constValue;// = 5;
	static const int[] constArray = {0, 1, 2, 3};
	
	int main()
	{
		return TOTOTO;
	}
	
	static const float CoCCompareEpsilon = 0.01;
	static const float4 constWhiteColor = new float4(1, 1, 1, 1);
	
	public static const int iconst;
}
*/

/*
shader Math
{
	static const float3 colors[5] = { float3(0,1,0), float3(0,0,1), float3(1,0,1), float3(1,0,0), float3(1,1,1)};
	
    static const float PI = 3.14159265358979323846;
}

shader SphericalHarmonicsBase<int TOrder> : Math
{
	static const int CoefficientsCount = TOrder * TOrder;
	
	static const float PI4  =  4 * PI;
	static const float PI16 = 16 * PI;
	static const float PI64 = 64 * PI;        
	static const float SQRT_PI  = 1.77245385090551602729;
	
	static const float A0 = 1.0;
	static const float A1 = 2.0/3.0;
	static const float A2 = 1.0/4.0;
	static const float A3 = 0.0;
	static const float A4 = -1.0/24.0;
	static const float A[5*5] = 
	{ 
		A0, 
		A1, A1, A1, 
		A2, A2, A2, A2, A2,
		A3, A3, A3, A3, A3, A3, A3,
		A4, A4, A4, A4, A4, A4, A4, A4, A4
	};
}

shader TestShaderMethod
{
	// Declare Vertex shader main method
    stage void VSMain() {}

    // Declare Pixel shader main method
    stage void PSMain() {}
	
	virtual float aVirtualMethod();
}
*/
//decorate function and class?
//check function prototype (virtual)
//what if members is declared after function body?
//generics?
//static function ?



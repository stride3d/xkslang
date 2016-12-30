//static const int totoB = toto;

//static const int toto = 55;

static const float3 colors[5] = { float3(0,1,0), float3(0,0,1), float3(1,0,1), float3(1,0,0), float3(1,1,1)};

float3 compute()
{
	return colors[1];
}


//static const float PI = 3.14159265358979323846;


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
*/
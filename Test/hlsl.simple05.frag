//Test parsing a shader "class", defining a bunch of methods and attributes

shader ShaderSimple
{
	float2 aFloat2;
	
	int Compute01()
	{
		int res = 4;
		return 1;
	}
	
	void Compute02(void)
	{
		bool doNothing = false;
		return;
	}
	
	float4 Compute03(int toto, float f, bool b){
		return float4(1,2,3,4);
	}
	
	int aInt;
};

//Test parsing a shader defining a function refering to a shader variable

shader ShaderSimple
{
	float aFloat;
	
	float Compute01()
	{
		//float res = aFloat + 1;
		//return res;
		
		//aFloat;
		return 1.0;
	}
};

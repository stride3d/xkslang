//Test parsing a shader declaring some functions 
shader Shader01
{
	float4 functionDefinition(bool b)
	{
		return functionWithPrototypeThenBody();
	}
	
	float functionWithPrototypeThenBody();
	
	float functionWithPrototypeThenBody()
	{
		return float4(0, 0, 0, 0);
	}
	
	//Below codes will make SPIRV to crash
	//Need to check how to add undefined functions into SPIRV
	/*bool functionCallingAfunctionWithoutBody()
	{
		return functionWithoutBody();
	}
	
	void functionWithoutBody(){}*/
};

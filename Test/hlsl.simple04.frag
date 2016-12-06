//Test parsing a very simple shader "class", only defining a method

shader ShaderSimple
{
	int Compute()
	{
		return 1;
	}
	
	float4 BaseColor;
};

//TODO: duplicate function name (should name class_function ?)
//TODO: check function prototype
//TODO: what if members is declared after function body?
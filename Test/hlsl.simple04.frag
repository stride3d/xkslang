//Test parsing a very simple shader "class", only defining a method

shader ShaderSimple
{
	int Compute1()
	{
		return 1;
	}

	int Compute2()
	{
		return 1;
	}
};

//TODO: duplicate function name (should name class_function ?)
//should decorate function and class?
//TODO: check function prototype
//TODO: what if members is declared after function body?


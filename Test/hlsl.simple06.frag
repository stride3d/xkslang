/****************************************************************************************************
Test parsing 2 simple shaders "class" defining same function name and attributes
A same attribute name defined in different shader is not a problem: attributes belongs to the shader namespace.
However all functions are defined in the global namespace and so we need to differentiate them.
A shader function will be named: ShaderClass.functionName(params) --> "functionName(ShaderClass;params"
(to be consistent with SPIRV name structure)
****************************************************************************************************/

shader Shader01
{
	int toto;
	
	int Compute()
	{
		return 1;
	}
	
	int Compute(int i, float f)
	{
		return i;
	}
};

shader Shader02
{
	int toto;
	
	int Compute()
	{
		return 1;
	}
	
	int Compute(int i, float f)
	{
		return i;
	}
};


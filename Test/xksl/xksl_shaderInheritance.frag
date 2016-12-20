/****************************************************************
Test parsing shader inheriting from other shaders
We'll simply keep the inheritance information in the bytecode
Also test using declaration keywords before the shader
****************************************************************/

shader toto
{};

shader Child : Parent01, Parent02, toto
{
	int toto;
};

shader Parent01: Parent02
{
};

internal shader TestInternalShader
{
};

/**************
//Some invalid shader declaration
stage stream shader TestWrongShader
{
};

typedef shader typedefShader
{};
**************/

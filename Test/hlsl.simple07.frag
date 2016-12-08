/*******************************************************
Test parsing a shader inheriting from another shaders
*******************************************************/

shader toto
{};

shader Child : Parent01, Parent02, toto
{
	int toto;
};

shader Parent01: Parent02
{
};

//decorate function and class?
//TODO: check function prototype
//TODO: what if members is declared after function body?
//generics?


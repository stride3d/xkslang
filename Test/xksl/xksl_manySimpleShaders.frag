//Test parsing several basic shaders in a same file
//Normally not permitted in XKSL, but parser can handle it

shader Shader01
{
    float4 BaseColor;
};

shader Shader02
{
    float4 BaseColor;
};

shader Shader03
{
    float c0, c1;
	int i;
};

//Name collision
//shader Shader01
//{
//    float f;
//};
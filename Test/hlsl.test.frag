
//GLSL input
//D:\Prgms\glslang\source\Test\hlsl.test.frag --keep-uncalled -V -H -o D:\Prgms\glslang\source\Test\glslang.latest.spv

//HLSL input (-D)

/*void main (void)  
{     
   gl_FragColor = vec4(0.0, 1.0, 0.0, 1.0);  
}*/

//SpecialQualifier("gl_FragColor",        EvqFragColor,  EbvFragColor,        symbolTable);

/*struct PS_OUTPUT
{
	float4 Color : SV_Target;
};

PS_OUTPUT main()
{
    PS_OUTPUT Output;
    Output.Color = float4(0);
    return Output;
}*/

//VERTEX SHADER
/*void main()
{
	//Vertex shader output
	gl_Position = vec4(1.0, 0.0, 0.0, 0.0);
	gl_PointSize = 5f;
}*/

#version 140

//PIXEL SHADER
void main()
{    
	//pixel shader outputs
	gl_FragColor = vec4(0.0, 0.0, 1.0, 1.0);
	//gl_FragData[0] = vec4(0.0);  //can use either gl_FragColor or gl_FragData
	gl_FragDepth = 1;
	
	//pixel shader input
	//vec4 fragCoord = gl_FragCoord;
	//bool b = gl_FrontFacing;          //Unsupported builtin in HLSL: builtInFrontFacing
	//vec2 pointCoord = gl_PointCoord;  //Unsupported builtin in HLSL: builtInPointCoord
}

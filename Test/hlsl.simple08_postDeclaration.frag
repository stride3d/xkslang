/**************************************************************************************
Parse Methods and members' post declaration keyword
Accessibility: public, private, internal, protected: those will be ignored when parsed

Member: stage, stream, static, const, staging
Method: stage, override, abstract, clone
==> We just parse and add the qualifier attributes in the SPIR-X files (as decorate), to resolve things at compilation time

TODO: staging
TODO: Input, Output, Input2, Constants (for geometry and tessellation shaders)
TODO streams: but not for variable declaration
***************************************************************************************/
/*
shader ShaderTestAccessor
{
	public int i0;
	private int i1;
	protected int i2;
	internal int i3;
	
	public internal private protected float f;  //makes no sense, but to check if the parser can treat it
};

public shader ShaderPublic
{};
*/

/*
shader ShaderTestMember
{
	stage stream float4 ShadingPosition : SV_Position;
	
	//only check post-declaration keywords, parsing initialization values will be done later
	static const int constValue;
	const static int constValue01;
	private static const int constArray[5];
	
	static double aStatic;
	const double aConst;
	
	//That one probably don't make any sense
	private static const stage stream float mixWithEverything;
}
*/

shader TestShaderMethod
{
	// Declare Vertex shader main method
    stage void VSMain() {}

    // Declare Pixel shader main method
    //stage void PSMain() {}
	
	//virtual float aVirtualMethod();
}




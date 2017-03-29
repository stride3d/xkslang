// D:\Prgms\glslang\source\Test\hlsl.test.frag --keep-uncalled -D -V -o D:\Prgms\glslang\source\Test\hlsl.test.frag.latest.spv
//D:\Prgms\glslang\source\Test\hlsl.test.frag --keep-uncalled -D -V -H -o D:\Prgms\glslang\source\Test\hlsl.test.fragB.latest.spv.hr

struct DirectionalLightData
{
	float3 DirectionWS;
	//[Color]
	float3 Color;
};

void main()
{
	DirectionalLightData d = (DirectionalLightData)0;
}

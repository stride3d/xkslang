static struct
{
	float4 aFloat;
} ShaderSimple;
	
float Compute01() //ShaderSimple
{
	return ShaderSimple.aFloat.y;
}

static const float colorZ = 0.5;

static struct
{
	float2 Position : POSITION;
	float4 ColorTarget;
} streams;

float4 BaseColor;   

stage void VSMain() 
{
	streams.Position = float2(0.0f, 1.0f);
}

stage void PSMain() 
{
	streams.ColorTarget = float4(streams.Position.xy, colorZ, 1) + BaseColor;
}

Texture2D Texture0;
SamplerState Sampler;

int toto;

cbuffer PerDraw 
{
    float4x4 MatrixTransform_id72;
	Texture2D Texture1;
};

struct VS_INPUT 
{
    float2 TexCoord: TEXCOORD0;
};

float4 Shading(VS_INPUT input)
{
	return Texture0.Sample(Sampler, input.TexCoord);
}
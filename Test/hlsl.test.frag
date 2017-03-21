/*cbuffer Buf
{
	SamplerState Sampler0;
	Texture2D Texture0;
	TextureCube CubeMap;
	
	//Texture3D<uint2> LightClusters;
	//Buffer<uint> LightIndices;
	//Buffer<float4> PointLights;
};*/

SamplerState Sampler0;
Texture2D Texture0;
TextureCube CubeMap;
Texture3D text3D;
Texture3D<uint2> text3Du;
Texture2D TextureTabs[2];
Buffer<float4> PointLights;

void main()
{
	float2 uv2 = float2(0.5, 0.5);
	float4 colorA = Texture0.Sample(Sampler0, uv2);
	
	float3 direction = float3(1, 0, 0);
	float4 colorB = CubeMap.Sample(Sampler0, direction);
}

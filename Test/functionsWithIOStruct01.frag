
//struct VS_OUT 
//{
//    float2 Position;
//};

struct AStruct
{
    float2 Position;
    float4 Color;
};

void Compute_id0(inout AStruct s)
{
}

void PSMain()
{
	AStruct s;
	Compute_id0(s);
	Compute_id0(s);
}


cbuffer PerDraw
{
    float4x4 ShaderMain_BlendMatrixArray[5];
};

float4x4 ShaderMain_GetBlendMatrix(int index)
{
    return ShaderMain_BlendMatrixArray[index];
}

void vert_main()
{
    int param = 2;
    float4x4 f44 = ShaderMain_GetBlendMatrix(param);
}

void main()
{
    vert_main();
}

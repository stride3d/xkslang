struct globalStreams
{
    int aStream_0;
};

cbuffer PreDraw
{
    int ShaderBase_4_1__aVar;
};

static globalStreams globalStreams_var;

int ShaderBase_4_1__compute()
{
    return ShaderBase_4_1__aVar + 4;
}

int ShaderMain_7_4__compute()
{
    return ShaderBase_4_1__compute();
}

void vert_main()
{
    globalStreams_var.aStream_0 = ShaderMain_7_4__compute() + 11;
}

void main()
{
    vert_main();
}

struct globalStreams
{
    int aStream_0;
};

cbuffer PreDraw
{
    int ShaderBase_1__aVar;
};

static globalStreams globalStreams_var;

int ShaderBase_1__compute()
{
    return (ShaderBase_1__aVar + 1) + globalStreams_var.aStream_0;
}

int ShaderMain_1_1__compute()
{
    return ShaderBase_1__compute();
}

int vert_main()
{
    return ShaderMain_1_1__compute();
}

void main()
{
    vert_main();
}

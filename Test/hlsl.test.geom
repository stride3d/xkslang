struct PSInput
{
    float myfloat;
    int something;
};

[maxvertexcount(4)]
void main(triangle in uint VertexID[3],
          inout LineStream<PSInput> OutputStream)
{
    PSInput Vert;

    Vert.myfloat    = 0;
    Vert.something  = VertexID[0];

    OutputStream.Append(Vert);
    OutputStream.Append(Vert);
    OutputStream.RestartStrip();
}
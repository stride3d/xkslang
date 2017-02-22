#version 450

struct Base_streamBuffer
{
    int streamI;
    float streamF;
    bool streamB;
};

Base_streamBuffer Base_streamBuffer_var;

void main()
{
    Base_streamBuffer_var.streamI = 0;
}


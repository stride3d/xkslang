#version 450

struct ShaderA_streamBuffer
{
    int svar;
};

ShaderA_streamBuffer ShaderA_streamBuffer_var;

int main()
{
    int res = 0;
    res++;
    ShaderA_streamBuffer_var.svar = res;
    res++;
    ShaderA_streamBuffer_var.svar = res;
    return res;
}


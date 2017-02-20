#version 450

struct ShaderA_streamBuffer
{
    int ShaderA_svar;
};

ShaderA_streamBuffer ShaderA_streamBuffer_var;

int main()
{
    int res = 0;
    res++;
    ShaderA_streamBuffer_var.ShaderA_svar = res;
    res++;
    ShaderA_streamBuffer_var.ShaderA_svar = res;
    return res;
}


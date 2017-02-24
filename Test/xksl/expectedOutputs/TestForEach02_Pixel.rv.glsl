#version 450

struct globalStreams
{
    int svar_0;
};

globalStreams globalStreams_var;

int main()
{
    int res = 0;
    res++;
    globalStreams_var.svar_0 = res;
    res++;
    globalStreams_var.svar_0 = res;
    return res;
}


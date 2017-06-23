struct globalStreams
{
    int svar_0;
};

static globalStreams globalStreams_var;

int frag_main()
{
    int res = 0;
    res++;
    globalStreams_var.svar_0 = res;
    res++;
    globalStreams_var.svar_0 = res;
    return res;
}

void main()
{
    frag_main();
}

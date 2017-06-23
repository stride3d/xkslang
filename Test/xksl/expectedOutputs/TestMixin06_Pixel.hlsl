struct globalStreams
{
    int streamI_s0;
    float streamF_s1;
    bool streamB_2;
};

static globalStreams globalStreams_var;

void frag_main()
{
    globalStreams_var.streamI_s0 = 0;
}

void main()
{
    frag_main();
}

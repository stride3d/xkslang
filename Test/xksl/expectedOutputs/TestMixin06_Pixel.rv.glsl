#version 450

struct globalStreams
{
    int TOTO_s0;
    float streamF_s1;
    bool streamB_2;
};

globalStreams globalStreams_var;

void main()
{
    globalStreams_var.TOTO_s0 = 0;
}


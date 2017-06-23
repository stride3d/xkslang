int Base_Compute()
{
    return 1;
}

int frag_main(float param1, float param2)
{
    return Base_Compute();
}

void main()
{
    frag_main();
}

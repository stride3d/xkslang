static const int s1 = 5;

static struct S1
{
	static int s1;
} ss1;



int main()
{
	int i = s1;
	int j = ss1.s1;
	return i + j;
}
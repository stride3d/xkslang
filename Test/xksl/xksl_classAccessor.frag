/****************************************************************************************
Parse shader with class accessor: this, base, className
ClassName has to be known, otherwise the expression will be set as unresolved.

*****************************************************************************************/

shader S01
{
	int a;
};

shader Shader01: S01, S02
{
	int c;
	
	void f()
	{
		S01.a;
		S02.b;
		
		base.a;
		base.b;
		
		this.a;
		this.c;
	}
}

shader S02
{
	int b;
};
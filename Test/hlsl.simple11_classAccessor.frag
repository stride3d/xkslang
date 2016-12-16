/****************************************************************************************
Parse shader with class accessor: this, base, className
ClassName has to be known, otherwise the expression will be set as unresolved.

*****************************************************************************************/

static struct
{
	float4 prout;
} toto;

void main()
{
	float4 f = toto.prout.wxyz;
}

/*
shader S03
{
};

shader Shader01: S01, S02
{
	void f()
	{
		//this.aa;		
		//this.aInt = 5;
		//base.aInt = this.aInt;
		//base.aInt;
		
		//S01.aInt;
		//S02.aInt;
		//S03.aInt;
		
		unknownParent.unres03;
		unknownParent.unres03.xyz;
		
		int i = unres.asd + toto.prout * base.xyz.gloup.abc - 1;
		
		//streams.aInt;
		//base.toto;
		//base.unres02;		
		//Parent.toto;
		
		//unknownParent.unres03.xyz;
		
		//float4 color = base.Shading();
	}
}

shader S02
{
};*/
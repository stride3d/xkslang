/****************************************************************************************
Parse unresolved variable: check more complex situations

*****************************************************************************************/


shader Shader01
{
	int toto;
	
	void f()
	{
		int i = toto++ + 5;
		toto = 4 + 6;
	}
}
	
/*
shader Shader01: Base
{
	void f()
	{
		unres02 = 5;
		unres02++;
		
		//this.unres02 = 6;
		
		//base.unres02;
		
		//int i = Base.toto;
	}
}
*/
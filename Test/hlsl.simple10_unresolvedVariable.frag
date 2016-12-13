/****************************************************************************************
Parse unresolved variable

*****************************************************************************************/

shader Test
{
	void f01()
	{
		unres01;
		unres02 = 5;
		int i; i = unres03;
		int j = unres04 + 5;
		i = 5 + unres05;
		i = unres06 + unres07;
		i = unres08 + unres09 * 2 / unres10;
		unres11++;
		i = 5 + unres12++;
		i = --unres13 + ++unres14;
		
		i += unres15;
		i *= unres15;
		i /= unres15;
		i -= unres15;
	}
	
	//TODO: keep class info
	void f02()
	{
		//Base.k = 6;
		//i = 5 + Base.k;
		//int i = Base.F(2);
		//float4 f = float4(unres04, 0, 0, 1);
	}
	
}

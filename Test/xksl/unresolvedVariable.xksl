/****************************************************************************************
Parse unresolved variable
This shader is not valid anymore: we changed our initial plan and don't accept unresolved symbols anymore
Every symbol must be known when parsing a shader, either from the shader itself, or from its inherited parents
Parsing this shader will simply fail then...  :(
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
	
	void f02(int ind)
	{
		int i = 5;
		float4 f1 = float4(unres16, i, ind, 1);
		
		float4 f2 = unres17;
		
		unres19 = float4(1, 1, 1, 1) + unres18;
		
		int unres02 = 5 + 4;  //check name collision with unresolved unres02
	}
	
}


/*void main (void)  
{     
   gl_FragColor = vec4(0.0, 1.0, 0.0, 1.0);  
}*/

struct PS_OUTPUT
{
	float4 Color : SV_Target;
};

PS_OUTPUT main()
{
    PS_OUTPUT Output;
    Output.Color = float4(0);
    return Output;
}
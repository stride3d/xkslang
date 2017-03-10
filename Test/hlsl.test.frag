cbuffer PerLighting
{
	float BleedingFactor;
	float MinVariance;
};

float VSMain() 
{
	return BleedingFactor;
}

float PSMain() 
{
	return MinVariance;
}
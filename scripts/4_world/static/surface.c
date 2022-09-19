class Surface
{
	static float GetParamFloat(string surface_name, string param_name)
	{
		float value = GetGame().ConfigGetFloat( "CfgSurfaces " + surface_name + " " + param_name );
		return value;
	}
}
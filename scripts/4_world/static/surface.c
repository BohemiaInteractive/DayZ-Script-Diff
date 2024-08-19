class Surface
{
	static float GetParamFloat(string surface_name, string param_name)
	{
		return GetGame().ConfigGetFloat("CfgSurfaces " + surface_name + " " + param_name);
	}
	
	static bool AllowedWaterSurface(float pHeight, string pSurface, array<string> pAllowedSurfaceList)
	{
		if (pSurface)
		{
			pSurface.Replace("_ext", "");
			pSurface.Replace("_int", "");
		}

		bool isSeaCheck = false;
		
		foreach (string allowedSurface : pAllowedSurfaceList)
		{
			if (pSurface == "" && allowedSurface == UAWaterType.SEA)
				isSeaCheck = pHeight <= g_Game.SurfaceGetSeaLevel() + 0.001;

			if (isSeaCheck || allowedSurface == pSurface)
				return true;
		}
		
		return false;
	}
}
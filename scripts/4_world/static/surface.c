class Surface
{
	static int GetStepsParticleID(string surface_name) 
	{
		return SurfaceInfo.GetByName(surface_name).GetStepParticleId();
	}
	
	static int GetWheelParticleID(string surface_name) 
	{
		return SurfaceInfo.GetByName(surface_name).GetWheelParticleId();
	}
	
	static int GetParamInt(string surface_name, string param_name)
	{
		return GetGame().ConfigGetInt("CfgSurfaces " + surface_name + " " + param_name);
	}
	
	static float GetParamFloat(string surface_name, string param_name)
	{
		return GetGame().ConfigGetFloat("CfgSurfaces " + surface_name + " " + param_name);
	}
	
	static string GetParamText(string surfaceName, string paramName)
	{
		string output = "";
		GetGame().ConfigGetText("CfgSurfaces " + surfaceName + " " + paramName, output);
		return output;
		
	}
	
	static bool AllowedWaterSurface(float pHeight, string pSurface, array<string> pAllowedSurfaceList)
	{
		if (pSurface)
		{
			pSurface.Replace("_ext", "");
			pSurface.Replace("_int", "");
			pSurface.Replace("sakhal_", "");
		}

		bool isSeaCheck = false;
		
		foreach (string allowedSurface : pAllowedSurfaceList)
		{
			if (pSurface == "" && allowedSurface == UAWaterType.SEA)
				isSeaCheck = pHeight <= (g_Game.SurfaceGetSeaLevel() + 0.25); //MaxWave_default

			if (isSeaCheck || allowedSurface == pSurface)
				return true;
		}
		
		return false;
	}
	
	static bool CheckLiquidSource(float pHeight, string pSurface, int allowedWaterSourceMask)
	{
		bool success = false;
		if (pSurface == "")
		{
			if ( allowedWaterSourceMask & LIQUID_SALTWATER )
			{
				success = pHeight <= (g_Game.SurfaceGetSeaLevel() + 0.25); //MaxWave_default
			}
		}
		else
		{
			int liquidType = SurfaceInfo.GetByName(pSurface).GetLiquidType();

			success = allowedWaterSourceMask & liquidType;
		}

		return success;
	}
}
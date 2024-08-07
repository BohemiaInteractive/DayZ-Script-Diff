//! !!!! MAKE SURE TO REVERT THIS FILE BY 1.26 SCRIPT DIFF, USE 'SurfaceInfo', *.bisurf CAN'T BE MODDED !!!!
class SurfaceData
{
	string m_Name;
	bool m_HasConfig;
	int m_LiquidType;
	int m_StepParticleID;
	int m_WheelParticleID;
	
	
	void SurfaceData(string surfaceName)
	{
		m_Name = surfaceName;
		
		string cfgPath = "CfgSurfaces " + m_Name;
		m_HasConfig = GetGame().ConfigIsExisting(cfgPath);
		
		if (m_HasConfig)
		{
			SetStepsParticleIDFromConfig();
			SetWheelParticleIDFromConfig();
			SetLiquidTypeFromConfig();
		}
		else
		{
			m_StepParticleID = Surface.UNDEFINE_STEPS_PARTICLE_ID;
		}
	}
	
	bool HasConfig()
	{
		return m_HasConfig;
	}
	
	//Liquid type
	int SetLiquidTypeFromConfig()
	{
		string surfacePath = "CfgSurfaces " + m_Name + " liquidType";
		int liquidType;
		
		liquidType = GetGame().ConfigGetInt(surfacePath);
		
		return SetLiquidType(liquidType);
	}
	
	int SetLiquidType(int liquidType)
	{
		m_LiquidType = liquidType;
		return m_LiquidType;
	}
	
	int GetLiquidType()
	{
		return m_LiquidType;
	}
	
	//Step particle
	int SetStepsParticleIDFromConfig()
	{
		string surfacePath = "CfgSurfaces " + m_Name + " stepParticle";
		string stepParticle;
		
		GetGame().ConfigGetText(surfacePath, stepParticle);
		
		return SetStepsParticle(stepParticle);
	}
	
	int SetStepsParticle(string stepParticle)
	{
	 	int stepParticleID = -1;
		if (stepParticle)
		{
			stepParticleID = ParticleList.RegisterParticle(stepParticle);
		}
		return SetStepParticleID(stepParticleID);
	}
	
	int SetStepParticleID(int stepParticleID)
	{
		m_StepParticleID = stepParticleID;
		return m_StepParticleID;
	}
	
	int GetStepParticleID()
	{
		return m_StepParticleID;
	}
	
	//Wheel particle
	int SetWheelParticleIDFromConfig()
	{
		string surfacePath = "CfgSurfaces " + m_Name + " wheelParticle";
		string wheelParticle;
		
		GetGame().ConfigGetText(surfacePath, wheelParticle);
		
		return SetWheelParticle(wheelParticle);
	}
	
	int SetWheelParticle(string wheelParticle)
	{
	 	int wheelParticleID = -1;
		if (wheelParticle)
		{
			wheelParticleID = ParticleList.RegisterParticle(wheelParticle);
		}
		return SetWheelParticleID(wheelParticleID);
	}
	
	int SetWheelParticleID(int wheelParticleID)
	{
		m_WheelParticleID = wheelParticleID;
		return m_WheelParticleID;
	}
	
	int GetWheelParticleID()
	{
		return m_WheelParticleID;
	}
}

class Surface
{
	const int UNDEFINE_STEPS_PARTICLE_ID = -1;
	ref static map<string, ref SurfaceData> m_SurfaceDataMap;
	//ref static map<string, int> m_StepParticles;

	static void Init()
	{
		m_SurfaceDataMap = new map<string, ref SurfaceData>;
	}
		
	static SurfaceData GetSurfaceData(string surfaceName)
	{
		if (m_SurfaceDataMap.Contains(surfaceName))
			return m_SurfaceDataMap.Get(surfaceName);
		
		ref SurfaceData newSurfaceData = new SurfaceData(surfaceName);
		m_SurfaceDataMap.Insert(surfaceName, newSurfaceData);
		return newSurfaceData;
	}
	
	static int GetStepsParticleID(string surface_name) 
	{
		SurfaceData surfaceData = GetSurfaceData(surface_name);
		return surfaceData.GetStepParticleID();
	}
	
	static int SetStepsParticleIDFromConfig(string surface_name)
	{
		SurfaceData surfaceData = GetSurfaceData(surface_name);
		return surfaceData.SetStepsParticleIDFromConfig();
	}
	
	//Theoretically can be set virtual surfaces
	static int SetStepsParticle(string surface_name, string stepParticle)
	{
		SurfaceData surfaceData = GetSurfaceData(surface_name);
		return surfaceData.SetStepsParticle(stepParticle);
	}
	
	static int SetStepsParticleID(string surface_name, int stepParticleID)
	{
		SurfaceData surfaceData = GetSurfaceData(surface_name);
		return surfaceData.SetStepParticleID(stepParticleID);
	}
	
	static int GetWheelParticleID(string surface_name) 
	{
		SurfaceData surfaceData = GetSurfaceData(surface_name);
		return surfaceData.GetWheelParticleID();
	}
	
	static int SetWheelParticleIDFromConfig(string surface_name)
	{
		SurfaceData surfaceData = GetSurfaceData(surface_name);
		return surfaceData.SetWheelParticleIDFromConfig();
	}
	
	static int SetWheelParticle(string surface_name, string wheelParticle)
	{
		SurfaceData surfaceData = GetSurfaceData(surface_name);
		return surfaceData.SetWheelParticle(wheelParticle);
	}
	
	static int SetWheelParticleID(string surface_name, int wheelParticleID)
	{
		SurfaceData surfaceData = GetSurfaceData(surface_name);
		return surfaceData.SetWheelParticleID(wheelParticleID);
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
			int liquidType = GetSurfaceData(pSurface).GetLiquidType();

			success = allowedWaterSourceMask & liquidType;
		}

		return success;
	}
}
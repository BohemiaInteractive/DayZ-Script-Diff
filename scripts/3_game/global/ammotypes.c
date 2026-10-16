class ExplosiveEffectData
{
	int m_ParticleID;
	string m_SoundSetName;
	
	void ExplosiveEffectData(int particleID, string soundSetName = "")
	{
		m_ParticleID = particleID;
		m_SoundSetName = soundSetName;
	}
}

class AmmoTypesAPI
{
	static protected ref map<string, ref map<string, ref ExplosiveEffectData>> m_TypeToSurfaceParticleIDMap;
	
	private void AmmoTypesAPI() {}
	private void ~AmmoTypesAPI() {}
	
	//! Check if ammoType exists (registered in cfgAmmoTypes)
	static proto bool Exists(string ammoType);
	//! Get the default ammoType of a magazine
	static proto bool GetAmmoType(string magazineType, out string ammoType);
	
	//! Helper method
	static bool MagazineTypeToAmmoType(string magazineType, out string ammoType)
	{
		if ( !AmmoTypesAPI.Exists(magazineType) )
		{
			if ( !AmmoTypesAPI.GetAmmoType(magazineType, ammoType) )
			{
				ErrorEx(string.Format("%1 is not a magazineType or ammoType or has no default 'ammo' set up in config.", magazineType));
				return false;
			}
			return true;
		}
		else
		{
			// This is already an ammoType
			ammoType = magazineType;
			return true;
		}
	}
	
	static void Init()
	{
		#ifndef NO_GUI
		if(!m_TypeToSurfaceParticleIDMap)
		{
			m_TypeToSurfaceParticleIDMap = new map<string, ref map<string, ref ExplosiveEffectData>>();
			
			AddExplosionParticleEffect("Explosion_40mm_Ammo", "default", ParticleList.EXPLOSION_LANDMINE);
			AddExplosionParticleEffect("Explosion_40mm_Ammo", "Hit_Snow", ParticleList.EXPLOSION_GRENADE_SNOW);
			AddExplosionParticleEffect("Explosion_40mm_Ammo", "Hit_Ice", ParticleList.EXPLOSION_GRENADE_ICE);
			AddExplosionParticleEffect("Explosion_40mm_Ammo", "Hit_Desert_Sand", ParticleList.EXPLOSION_GRENADE_DESERT_SAND);
			AddExplosionParticleEffect("Explosion_40mm_Ammo", "Hit_Sand", ParticleList.EXPLOSION_GRENADE_SAND);
			
			AddExplosionParticleEffect("LandMineExplosion_Ammo", "Hit_Snow", ParticleList.EXPLOSION_GRENADE_SNOW);
			AddExplosionParticleEffect("LandMineExplosion_Ammo", "Hit_Ice", ParticleList.EXPLOSION_GRENADE_ICE);
			AddExplosionParticleEffect("LandMineExplosion_Ammo", "Hit_Desert_Sand", ParticleList.EXPLOSION_GRENADE_DESERT_SAND);
			AddExplosionParticleEffect("LandMineExplosion_Ammo", "Hit_Sand", ParticleList.EXPLOSION_GRENADE_SAND);
			
			
			AddExplosionParticleEffect("RGD5Grenade_Ammo", "Hit_Snow", ParticleList.EXPLOSION_GRENADE_SNOW);
			AddExplosionParticleEffect("RGD5Grenade_Ammo", "Hit_Ice", ParticleList.EXPLOSION_GRENADE_ICE);
			AddExplosionParticleEffect("RGD5Grenade_Ammo", "Hit_Desert_Sand", ParticleList.EXPLOSION_GRENADE_DESERT_SAND, "ExplosionDustEffect_SoundSet");
			AddExplosionParticleEffect("RGD5Grenade_Ammo", "Hit_Sand", ParticleList.EXPLOSION_GRENADE_SAND, "ExplosionDustEffect_SoundSet");
			
			AddExplosionParticleEffect("M67Grenade_Ammo", "Hit_Snow", ParticleList.EXPLOSION_GRENADE_SNOW);
			AddExplosionParticleEffect("M67Grenade_Ammo", "Hit_Ice", ParticleList.EXPLOSION_GRENADE_ICE);
			AddExplosionParticleEffect("M67Grenade_Ammo", "Hit_Desert_Sand", ParticleList.EXPLOSION_GRENADE_DESERT_SAND, "ExplosionDustEffect_SoundSet");
			AddExplosionParticleEffect("M67Grenade_Ammo", "Hit_Sand", ParticleList.EXPLOSION_GRENADE_SAND, "ExplosionDustEffect_SoundSet");
	
		}
		#endif
	}
	
	static void AddExplosionParticleEffect(string ammoName, string surfaceName, int particleID, string soundSetName = "")
	{
		map<string, ref ExplosiveEffectData>> surfaceNameToEffectIDMap;
		if (!m_TypeToSurfaceParticleIDMap.Find(ammoName, surfaceNameToEffectIDMap))
		{
			surfaceNameToEffectIDMap = new map<string, ref ExplosiveEffectData>>;
			m_TypeToSurfaceParticleIDMap.Insert(ammoName, surfaceNameToEffectIDMap);
		}
		
		surfaceNameToEffectIDMap.Insert(surfaceName, new ExplosiveEffectData(particleID, soundSetName));
	}
	
	static ExplosiveEffectData GetExplosiveEffectData(string ammoName, string surfaceName)
	{
		ExplosiveEffectData result;
		
		#ifdef NO_GUI
		return result;
		#endif
		
		string particleFileName;
		int particleID = ParticleList.NONE;
		map<string, ref ExplosiveEffectData>> surfaceNameToEffectIDMap = m_TypeToSurfaceParticleIDMap.Get(ammoName);
		
		if (surfaceNameToEffectIDMap)
		{
			if (!surfaceNameToEffectIDMap.Find(surfaceName, result))
			{
				if(!surfaceNameToEffectIDMap.Find("default", result))
				{
					g_Game.ConfigGetText(string.Format("cfgAmmo %1 particle", ammoName), particleFileName);
			
					if ( particleFileName != "" )
					{
						particleID = ParticleList.GetParticleIDByName(particleFileName);
					}
					AddExplosionParticleEffect(ammoName, "default", particleID);
					result = surfaceNameToEffectIDMap.Get("default");
				}
			}
		}
		else
		{
			g_Game.ConfigGetText(string.Format("cfgAmmo %1 particle", ammoName), particleFileName);
			
			if ( particleFileName != "" )
			{
				particleID = ParticleList.GetParticleIDByName(particleFileName);
			}
			AddExplosionParticleEffect(ammoName, "default", particleID);
			surfaceNameToEffectIDMap = m_TypeToSurfaceParticleIDMap.Get(ammoName);
			result = surfaceNameToEffectIDMap.Get("default");
		}
		return result;
	}
	
	static void Cleanup()
	{
		#ifndef NO_GUI
		if(!m_TypeToSurfaceParticleIDMap)
		{
			m_TypeToSurfaceParticleIDMap.Clear();
		}
		#endif
	}
}
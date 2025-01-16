class AmmoTypesAPI
{
	static protected ref map<string, ref map<string, int>> m_TypeToSurfaceParticleIDMap;
	
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
		if(!m_TypeToSurfaceParticleIDMap)
		{
			m_TypeToSurfaceParticleIDMap = new map<string, ref map<string, int>>();
			
			AddExplosionParticleEffect("Explosion_40mm_Ammo", "Hit_Snow", ParticleList.EXPLOSION_GRENADE_SNOW);
			AddExplosionParticleEffect("Explosion_40mm_Ammo", "Hit_Ice", ParticleList.EXPLOSION_GRENADE_ICE);
			AddExplosionParticleEffect("Explosion_40mm_Ammo", "default", ParticleList.EXPLOSION_LANDMINE);
		}
	}
	
	static void AddExplosionParticleEffect(string ammoName, string surfaceName, int particleID)
	{
		map<string, int>> surfaceNameToEffectIDMap;
		if (!m_TypeToSurfaceParticleIDMap.Find(ammoName, surfaceNameToEffectIDMap))
		{
			surfaceNameToEffectIDMap = new map<string, int>>;
			m_TypeToSurfaceParticleIDMap.Insert(ammoName, surfaceNameToEffectIDMap);
		}
		
		surfaceNameToEffectIDMap.Insert(surfaceName, particleID);
	}
	
	static int GetExplosionParticleID(string ammoName, string surfaceName)
	{
		int result;
		map<string, int>> surfaceNameToEffectIDMap = m_TypeToSurfaceParticleIDMap.Get(ammoName);
		if (surfaceNameToEffectIDMap)
		{
			if (!surfaceNameToEffectIDMap.Find(surfaceName, result))
			{
				if(!surfaceNameToEffectIDMap.Find("default", result))
				{
					result = -1;
				}
			}
		}
		return result;
	}
	
	static void Cleanup()
	{
		if(!m_TypeToSurfaceParticleIDMap)
		{
			m_TypeToSurfaceParticleIDMap.Clear();
		}
	}
}
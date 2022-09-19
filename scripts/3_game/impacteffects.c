enum ImpactTypes
{
	UNKNOWN;
	STOP;
	PENETRATE;
	RICOCHET;
	MELEE;
}

class ImpactMaterials
{
	ref static map<string, typename> 	m_ImpactEffect;
	static int 							m_LastRegisteredMaterial = 0;
	
	//! Map of ammo which will not spawn any impact effect
	ref static map<string, int>			m_IgnoredAmmo;
	static int 							m_LastRegisteredIgnoredAmmo = 0;
	
	/** \name  Surface effects
	*	Register all hit materials here!
	*	When modding, these can be unregistered with 'UnregisterSurface' if so desired
	*/
	//@{
	static int PLASTIC	 					= RegisterSurface("Hit_Plastic");
	static int SAND 						= RegisterSurface("Hit_Sand");
	static int TEXTILE 						= RegisterSurface("Hit_Textile");
	static int CONCRETE 					= RegisterSurface("Hit_Concrete");
	static int GRAVEL 						= RegisterSurface("Hit_Gravel");
	static int DIRT 						= RegisterSurface("Hit_Dirt");
	static int FOLIAGE 						= RegisterSurface("Hit_Foliage");
	static int GRASS 						= RegisterSurface("Hit_Grass");
	static int WOOD 						= RegisterSurface("Hit_Wood");
	static int METAL 						= RegisterSurface("Hit_Metal");
	static int GLASS 						= RegisterSurface("Hit_Glass");
	static int GLASS_THIN 					= RegisterSurface("Hit_Glass_Thin");
	static int WATER 						= RegisterSurface("Hit_Water");
	static int RUBBER 						= RegisterSurface("Hit_Rubber");
	static int PLASTER 						= RegisterSurface("Hit_Plaster");
	static int MEATBONES					= RegisterSurface("Hit_MeatBones");
	static int MEATBONES_SHOVEL				= RegisterSurface("Hit_MeatBones_MeleeShovel");
	static int MEATBONES_PIPEWRENCH			= RegisterSurface("Hit_MeatBones_MeleePipeWrench");
	static int MEATBONES_WRENCH				= RegisterSurface("Hit_MeatBones_MeleeWrench");
	static int MEATBONES_FIST				= RegisterSurface("Hit_MeatBones_MeleeFist");
	static int UNDEFINED 					= RegisterSurface("Hit_Undefined");
	static int ERROR_NO_MATERIAL			= RegisterSurface("Hit_ErrorNoMaterial");
	//@}
	
	/** \name  Ignored ammo
	*	Register ammo which will not spawn impact effects here
	*	When modding, these can be unregistered with 'UnregisterIgnoredAmmo' if so desired
	*/
	//@{
	static int FIST							= RegisterIgnoredAmmo("MeleeFist");
	static int FIST_HEAVY					= RegisterIgnoredAmmo("MeleeFist_Heavy");
	static int SOFT							= RegisterIgnoredAmmo("MeleeSoft");
	static int SOFT_HEAVY					= RegisterIgnoredAmmo("MeleeSoft_Heavy");
	//@}
	
	static int RegisterSurface(string surface)
	{
		if (!m_ImpactEffect)
			m_ImpactEffect = new map<string, typename>;
		
		m_ImpactEffect.Insert(surface, surface.ToType());
		 
		return ++m_LastRegisteredMaterial;
	}
	
	static bool UnregisterSurface(string surface)
	{
		if (m_ImpactEffect)
		{
			m_ImpactEffect.Remove(surface);
			return !m_ImpactEffect.Contains(surface);
		}
		
		return false;
	}
	
	static int RegisterIgnoredAmmo(string ammo)
	{
		if (!m_IgnoredAmmo)
			m_IgnoredAmmo = new map<string, int>;
		
		++m_LastRegisteredIgnoredAmmo;
		
		m_IgnoredAmmo.Insert(ammo, m_LastRegisteredIgnoredAmmo);
		 
		return m_LastRegisteredIgnoredAmmo;
	}
	
	static bool UnregisterIgnoredAmmo(string ammo)
	{
		if (m_IgnoredAmmo)
		{
			m_IgnoredAmmo.Remove(ammo);
			return !m_IgnoredAmmo.Contains(ammo);
		}
		
		return false;
	}
	
	static typename GetImpactEffect(string surface, string ammoType)
	{
		string key = string.Format("%1_%2", surface, ammoType);
		
		typename eff_type = m_ImpactEffect[key];
		
		if (eff_type)
			return eff_type;
		else
			return m_ImpactEffect[surface];
	}
	
	static void EvaluateImpactEffect(Object directHit, int componentIndex, string surface, vector pos, int impact_type, vector surfNormal, vector exitPos, vector inSpeed, vector outSpeed, bool deflected, string ammoType, bool isWater)
	{
		// No impact effects wanted for this ammo
		if (m_IgnoredAmmo.Contains(ammoType))
			return;
		
		if (impact_type == ImpactTypes.UNKNOWN)
			impact_type = ImpactTypes.STOP;
		
		if (deflected)
			impact_type = ImpactTypes.RICOCHET;
		else if (outSpeed)
			impact_type = ImpactTypes.PENETRATE;
		
		if (isWater)
			surface = "Hit_Water";
		
		EffBulletImpactBase eff = EffBulletImpactBase.Cast(GetImpactEffect(surface, ammoType).Spawn());
		
		if ( !eff && surface == "" ) // handle undefined surface
		{
			eff = EffBulletImpactBase.Cast( surface.ToType().Spawn() );
			
			if (eff)
			{
				RegisterSurface(surface);
				ErrorEx(string.Format("Unregistered surface for bullet impact effect (%1). Register this surface in ImpactMaterials (Script) for better performance.", surface), ErrorExSeverity.WARNING);
			}
			else
			{
				if (directHit)
				{
					string object_type = directHit.GetType();
					
					if ( object_type == "" )
						object_type = "OBJECT_WITHOUT_CONFIG_CLASS";

					ErrorEx(string.Format("Object '%1' with model file: %2.p3d has undefined 'Hit_...' material! Cannot play impact effect.", object_type, directHit.GetModelName()));
					eff = EffBulletImpactBase.Cast(GetImpactEffect("Hit_ErrorNoMaterial", ammoType).Spawn());
				}
			}
		}
		
		if ( !eff && surface != "" )
		{
			ErrorEx(string.Format("Unregistered surface impact material <%1>! Register this surface in ImpactMaterials (Script).", surface));
			eff = EffBulletImpactBase.Cast(GetImpactEffect("Hit_Undefined", ammoType).Spawn());
		}
		
		if (eff)
		{
			eff.EvaluateEffect(directHit, componentIndex, pos, impact_type, surfNormal, exitPos, inSpeed, outSpeed, ammoType);
			eff.SetAutodestroy(true);
			SEffectManager.PlayInWorld( eff, pos );
		}
	}
}
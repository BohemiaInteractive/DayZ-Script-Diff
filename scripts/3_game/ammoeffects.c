/**
\brief Static data holder for certain ammo config values
	\note Kept the names similar to what is in config, but it might be a little deceiving as this is mainly used for explosives
*/
class AmmoEffects
{	
	//! Key: Ammo class name; Data: ParticleList ID
	static ref map<string, int> m_AmmoParticles;
	
	//! Key: Ammo class name; Data: ParticleList ID
	static ref map<string, typename> m_AmmoEffects;
	
	
	
	/** \name Ammo particles
 		Methods regarding getting/playing ammo particle
	*/
	//@{
	
	//! Get the ParticleList ID for the particle for this ammoType
	static int GetAmmoParticleID(string ammoType)
	{
		int particleID;
		
		// Search for it in the static map
		if ( !m_AmmoParticles.Find(ammoType, particleID) )
		{
			// Load it in when we can't find it
			string particleFileName;
			GetGame().ConfigGetText(string.Format("cfgAmmo %1 particle", ammoType), particleFileName);
			
			// If we found a valid entry, try looking for it in ParticleList
			if ( particleFileName != "" )
			{
				particleID = ParticleList.GetParticleIDByName(particleFileName);
			}
			
			// Store it for next search
			m_AmmoParticles.Insert(ammoType, particleID);
		}
		
		return particleID;
	}
	
	//! Attempt to play the ammo particle at pos if found, returns true on success
	static bool PlayAmmoParticle(string ammoType, vector pos)
	{
		int particleID = GetAmmoParticleID(ammoType);
		
		if (ParticleList.IsValidId(particleID))
		{
			return ParticleManager.GetInstance().PlayInWorld(particleID, pos) != null;
		}
		
		return false;
	}
	
	//@}
	
	
	
	/** \name Ammo effects
 		Methods regarding getting/playing ammo effect
	*/
	//@{
	
	//! Get the typename for the effect for this ammoType
	static typename GetAmmoEffectTypename(string ammoType)
	{
		typename typeName;
		
		// Search for it in the static map
		if ( !m_AmmoEffects.Find(ammoType, typeName) )
		{
			// Load it in when we can't find it
			string effectName;
			GetGame().ConfigGetText(string.Format("cfgAmmo %1 effect", ammoType), effectName);
			
			// If we found a valid entry, try looking for it in ParticleList
			if ( effectName != "" )
			{
				typeName = effectName.ToType();
			}
			
			// Store it for next search
			m_AmmoEffects.Insert(ammoType, typeName);
		}
		
		return typeName;
	}
	
	//! Attempt to play the ammo effect at pos if found, returns true on success
	static bool PlayAmmoEffect(string ammoType, vector pos)
	{
		typename typeName = GetAmmoEffectTypename(ammoType);
		
		if ( typeName )
		{
			Effect eff = Effect.Cast(typeName.Spawn());
			
			if ( eff )
			{
				eff.SetAutodestroy(true);
				return SEffectManager.PlayInWorld( eff, pos );
			}
		}
		
		return false;
	}
	
	//@}
	
	
	
	/** \name Lifetime
 		Creation and cleanup
	*/
	//@{
	
	//! Initialize the containers: this is done this way, to have these not exist on server
	static void Init()
	{
		m_AmmoParticles = new map<string, int>();
		m_AmmoEffects = new map<string, typename>();
	}
	
	//! Clean up the data
	static void Cleanup()
	{
		/* These ain't containing no refs, so whatever
		m_AmmoParticles.Clear();
		m_AmmoEffects.Clear();
		*/
	}
	
	//@}
}
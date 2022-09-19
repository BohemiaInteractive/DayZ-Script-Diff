class Land_FuelStation_Feed extends FuelStation
{
	override void EEKilled(Object killer)
	{
		super.EEKilled(killer);

	 	Explode(DT_EXPLOSION, "LandFuelFeed_Ammo");
	}
	
	override void OnExplosionEffects(Object source, Object directHit, int componentIndex, string surface, vector pos, vector surfNormal, float energyFactor, float explosionFactor, bool isWater, string ammoType)
	{
		if ( !GetGame().IsDedicatedServer() )
		{	
			vector n = surfNormal.VectorToAngles() + "0 90 0";
			Particle p1 = ParticleManager.GetInstance().PlayInWorld(ParticleList.SMOKE_GENERIC_WRECK, pos);
			p1.SetOrientation(n);
		
			Particle p2 = ParticleManager.GetInstance().PlayInWorld(ParticleList.EXPLOSION_LANDMINE, pos);
			p2.SetOrientation(n);

			Particle p3 = ParticleManager.GetInstance().PlayInWorld(ParticleList.IMPACT_METAL_RICOCHET, pos);
			p3.SetOrientation(n);
		
			Particle p4 = ParticleManager.GetInstance().PlayInWorld(ParticleList.IMPACT_GRAVEL_RICOCHET, pos);
			p4.SetOrientation(n);
		}
	}
	
	//! Returns true if this stand is functional
	bool HasFuelToGive()
	{
		return !IsRuined();
	}
};
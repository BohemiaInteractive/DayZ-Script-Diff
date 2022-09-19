class Hit_Foliage : EffBulletImpactBase
{
	void Hit_Foliage()
	{
		SetEnterParticle(ParticleList.IMPACT_FOLIAGE_ENTER);
		SetExitParticle(ParticleList.IMPACT_FOLIAGE_EXIT);
		SetRicochetParticle(ParticleList.IMPACT_FOLIAGE_RICOCHET);
	}
	
	override float CalculateStoppingForce(float in_speedf, float out_speedf, string ammoType, float weight)
	{
		if ( m_ImpactType == ImpactTypes.MELEE )
		{
			return 500;
		}
		
		float projectile_weight_coef = weight / DEFAULT_PROJECTILE_WEIGHT;
		
		float stopping_force = in_speedf * projectile_weight_coef * 0.5;
				
		return stopping_force;
	}
}
class Hit_Water : EffBulletImpactBase
{
	void Hit_Water()
	{
		SetSingleParticle(ParticleList.IMPACT_WATER_SMALL_ENTER);
		
		m_AngledEnter = 10;
	}
	
	override void EvaluateEffect(Object directHit, int componentIndex, vector pos, int impact_type, vector surfNormal, vector exitPos, vector inSpeed, vector outSpeed, string ammoType)
	{
		super.EvaluateEffect(directHit, componentIndex, pos, impact_type, surfNormal, exitPos, inSpeed, outSpeed, ammoType);
		
		m_SurfNormal = "0 0 0";
	}
	
	override float CalculateStoppingForce(float in_speedf, float out_speedf, string ammoType, float weight)
	{
		float projectile_weight_coef = weight / DEFAULT_PROJECTILE_WEIGHT;
		
		float stopping_force = (in_speedf - out_speedf) * projectile_weight_coef;
		
		if ( stopping_force < 350 )
			SetEnterParticle(ParticleList.IMPACT_WATER_SMALL_ENTER);
		
		if ( stopping_force >= 350  &&  stopping_force < 750 )
			SetEnterParticle(ParticleList.IMPACT_WATER_MEDIUM_ENTER);
		
		if ( stopping_force >= 750 )
			SetEnterParticle(ParticleList.IMPACT_WATER_LARGE_ENTER);
		
		return stopping_force;
	}
	
	override void OnEnterCalculations( Particle p )
	{
		// no particle scaling
	}
	
	override void OnExitCalculations(Particle p, float outSpeedf)
	{
		// no particle scaling
	}
	
	override void OnRicochetCalculations(Particle p, float outspeedf)
	{
		// no particle scaling
	}
	
	override void OnEnterAngledCalculations(Particle p)
	{
		// no particle scaling
	}
}
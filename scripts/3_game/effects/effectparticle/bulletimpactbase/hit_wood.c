class Hit_Wood : EffBulletImpactBase
{
	void Hit_Wood()
	{
		SetEnterParticle(ParticleList.IMPACT_WOOD_ENTER);
		SetExitParticle(ParticleList.IMPACT_WOOD_EXIT);
		SetRicochetParticle(ParticleList.IMPACT_WOOD_RICOCHET);
	}
	
	override void OnEnterCalculations( Particle p )
	{
		// All values represent scale
		float velocity_min = MIN_SCALING_PARAM + (m_StoppingForce * m_EnterSplashCoef);
		float velocity_max = MIN_SCALING_PARAM + (m_StoppingForce * m_EnterSplashCoef);
		float size = MIN_SCALING_PARAM + ( m_StoppingForce * m_EnterSplashCoef)*0.5;
		float birth_rate = MIN_SCALING_PARAM + (m_StoppingForce * m_EnterSplashCoef)*0.5;
		
		if (velocity_min < MIN_SCALING_PARAM)
			velocity_min = MIN_SCALING_PARAM;
		
		if (size < MIN_SCALING_PARAM)
			size = MIN_SCALING_PARAM;
		
		if (birth_rate < MIN_SCALING_PARAM)
			birth_rate = MIN_SCALING_PARAM;
		
		
		
		p.ScaleParticleParam(EmitorParam.VELOCITY, velocity_min);
		p.ScaleParticleParam(EmitorParam.VELOCITY_RND, velocity_max);
		p.ScaleParticleParam(EmitorParam.SIZE, size);
		p.ScaleParticleParam(EmitorParam.BIRTH_RATE, birth_rate);
	}
	
	override void OnExitCalculations(Particle p, float outSpeedf)
	{
		float velocity_min = 1 + (outSpeedf * m_ExitSplashCoef);
		float velocity_max = 1 + (outSpeedf * m_ExitSplashCoef);
		float size = 1 + ( outSpeedf * m_ExitSplashCoef)*0.5;
		float birth_rate = 1 + (outSpeedf * m_ExitSplashCoef)*0.5;
		
		if (velocity_min < MIN_SCALING_PARAM)
			velocity_min = MIN_SCALING_PARAM;
		
		if (size < MIN_SCALING_PARAM)
			size = MIN_SCALING_PARAM;
		
		if (birth_rate < MIN_SCALING_PARAM)
			birth_rate = MIN_SCALING_PARAM;
		
		p.ScaleParticleParam(EmitorParam.VELOCITY, velocity_min);
		p.ScaleParticleParam(EmitorParam.VELOCITY_RND, velocity_max);
		p.ScaleParticleParam(EmitorParam.SIZE, size);
		p.ScaleParticleParam(EmitorParam.BIRTH_RATE, birth_rate);
	}
}
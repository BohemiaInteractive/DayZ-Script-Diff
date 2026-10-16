class EffVehicleDust : EffectParticle
{
	const float WHEEL_DUST_THRESHOLD = 3.0;
	
	override void ParticleParamsOverride(notnull ParticleParamsOverrideData data)
	{
		if (data.m_FloatModifiers.Count() < 2)
			return;

		Particle particle = Particle.Cast(GetParticle());
		if (!particle)
			return;
		
		float speed = data.m_FloatModifiers[0];
		float weight = data.m_FloatModifiers[1];

		particle.SetParticleParam(EmitorParam.LIFETIME, 2.5 * weight);
		particle.SetParticleParam(EmitorParam.REPEAT, true);
		particle.SetParticleParam(EmitorParam.WIND, true);
		particle.SetParticleParam(EmitorParam.VELOCITY, speed * weight);
		particle.SetParticleParam(EmitorParam.SIZE, speed * weight);
		particle.SetParticleParam(EmitorParam.AIR_RESISTANCE, 1.0 * weight);
	}
	
	override void SelectFromSurface(SurfaceInfo surfaceInfo)
	{
		int particleID = surfaceInfo.GetVehicleDustParticleId();
		
		if (m_ParticleID != particleID)
		{
			if (particleID != 0)
				SetParticleState(particleID);
			else
				Stop();
		}
	}
	
	override void SetParticleState(int state)
	{
		bool wasPlaying = IsPlaying();

		Stop();
		SetParticleID(state);
		
		if (wasPlaying)
			Start(); // resume effect
	}
}

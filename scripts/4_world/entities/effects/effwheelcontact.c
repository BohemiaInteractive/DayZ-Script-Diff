class EffWheelContact: EffectParticle
{
	const float SPEED_THRESHOLD = 3.0;
	
	override void ParticleParamsOverride(notnull ParticleParamsOverrideData data)
	{
		if (data.m_FloatModifiers.Count() < 2)
			return;

		Particle particle = Particle.Cast(GetParticle());
		if (!particle)
			return;
		
		float speed 	= data.m_FloatModifiers[0];
		float weight 	= data.m_FloatModifiers[1];

		particle.SetParticleParam(EmitorParam.LIFETIME, 1.0 * weight);
		particle.SetParticleParam(EmitorParam.SPRING, 15.0);
		particle.SetParticleParam(EmitorParam.REPEAT, false);
		particle.SetParticleParam(EmitorParam.WIND, false);
		particle.SetParticleParam(EmitorParam.SIZE, 1);
		particle.SetParticleParam(EmitorParam.AIR_RESISTANCE, 1.0);
	}
	
	override void SelectFromSurface(SurfaceInfo surfaceInfo)
	{
		int particleID = surfaceInfo.GetWheelContactParticleId();
		string soundSet = surfaceInfo.GetWheelContactSoundSetName();
		
		if (soundSet != "")
		{
			if (m_EffectSound == null || m_EffectSound.GetSoundSet() != soundSet)
			{
				EffectSound effectSound = SEffectManager.CreateSound(soundSet, vector.Zero, 0.0, 0.0, true);
				SetEffectSound(effectSound);
			}
		}
		else
		{
			if (m_EffectSound)
			{
				m_EffectSound.Stop();
				SetEffectSound(null);
			}
		}
		
		if (m_ParticleID != particleID)
		{
			if (particleID != 0)
				SetParticleState(particleID);
			else
			{
				Stop();
				m_EffectSound.Stop();
				SetEffectSound(null);
			}
		}
	}
	
	override void SetParticleState(int state)
	{
		bool wasPlaying = IsPlaying();

		Stop();
		SetParticleID(state);
		
		if (wasPlaying)
			Start();
	}
}
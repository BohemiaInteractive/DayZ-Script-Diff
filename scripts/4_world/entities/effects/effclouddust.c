class EffCloudDust : EffectParticle
{
	void EffCloudDust()
	{
		DisableParticleInUI(true);
	}

	void SetDefault()
	{
		int particleID = ParticleList.DUST_CLOUD;

		if (!m_EffectSound)
		{
			EffectSound effectSound = SEffectManager.CreateSound("DustWindGust_SoundSet", vector.Zero, 3, 3, true); 
			SetEffectSound(effectSound);
		}
		
		if (m_ParticleID != particleID)
		{
			if (particleID != 0)
			{
				SetParticleState(particleID);
			}
			else
			{
				Stop();
			}
		}
	}
	
	override void SelectFromSurface(SurfaceInfo surfaceInfo)
	{
		int particleID = surfaceInfo.GetCloudEffectParticleId();
		string soundSet = surfaceInfo.GetCloudEffectSoundSetName();
		EffectSound effectSound = null;
		
		if (soundSet != "")
		{
			if (m_EffectSound == null || m_EffectSound.GetSoundSet() != soundSet)
			{
				effectSound = SEffectManager.CreateSound(soundSet, vector.Zero, 3, 3, true);
				SetEffectSound(effectSound);
			}
		}
		else
		{
			if (m_EffectSound)
			{
				SetEffectSound(null);
			}
		}
		
		if (m_ParticleID != particleID)
		{
			if (particleID != 0)
			{
				SetParticleState(particleID);
			}
			else
			{
				Stop();
			}
		}
	}
	
	override void SetParticleState( int state )
	{
		bool wasPlaying = IsPlaying();

		Stop();
		SetParticleID(state);

		if (wasPlaying)
			Start(); // resume effect
	}
}

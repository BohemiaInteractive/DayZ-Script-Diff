class EffWheelSmoke : EffectParticle
{
	const float WHEEL_SMOKE_THRESHOLD = 5.0;
	
	void EffWheelSmoke()
	{
	}
	
	void SetSurface(string surface)
	{
		int particleID = Surface.GetWheelParticleID(surface);
		
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
	
	void SetParticleState( int state )
	{
		bool was_playing = IsPlaying();
	
		Stop();
		
		SetParticleID(state);
		
		if (was_playing)
		{
			Start(); // resume effect
		}
	}
}

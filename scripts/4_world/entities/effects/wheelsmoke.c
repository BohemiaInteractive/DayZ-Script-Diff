class EffWheelSmoke : EffectParticle
{
	const float WHEEL_SMOKE_THRESHOLD = 5.0;
	
	override void SelectFromSurface(SurfaceInfo surfaceInfo)
	{

		int particleID = surfaceInfo.GetWheelParticleId();
		
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
		{
			Start(); // resume effect
		}
	}
	
	[Obsolete("1.30: use SelectFromSurface instead")]
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
}

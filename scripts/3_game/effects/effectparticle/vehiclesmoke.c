class EffVehicleSmoke : EffectParticle
{
	void EffVehicleSmoke()
	{
		SetParticleStateLight();
	}
	
	
	void SetParticleStateLight()
	{
		SetParticleState( ParticleList.HATCHBACK_COOLANT_OVERHEATING );
	}
	
	
	void SetParticleStateHeavy()
	{
		SetParticleState( ParticleList.HATCHBACK_COOLANT_OVERHEATED );
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
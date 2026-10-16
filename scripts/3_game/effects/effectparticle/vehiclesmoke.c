class EffVehicleSmoke : EffectParticle
{
	override void SetParticleState(int state)
	{
		bool wasPlaying = IsPlaying();
	
		Stop();
		SetParticleID(state);
		
		if (wasPlaying)
			Start(); // resume effect
	}

	//! not used directly for cars	
	void SetParticleStateLight()
	{
		SetParticleState(ParticleList.HATCHBACK_COOLANT_OVERHEATING);
	}
	
	//! not used directly for cars
	void SetParticleStateHeavy()
	{
		SetParticleState(ParticleList.HATCHBACK_COOLANT_OVERHEATED);
	}
}
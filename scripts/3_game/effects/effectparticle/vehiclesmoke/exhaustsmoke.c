class EffExhaustSmoke : EffVehicleSmoke
{
	override void ParticleParamsOverride(notnull ParticleParamsOverrideData data)
	{
		if (data.m_FloatModifiers.Count() == 0)
			return;

		Particle particle = Particle.Cast(GetParticle());
		if (!particle)
			return;
		
		float speed = data.m_FloatModifiers[0];
		float lifetimeScale;
			
		if (speed < 100)
			lifetimeScale = 1 + (speed * 0.1);
		else
			lifetimeScale = 0.1;
			
		float birthrateScale = 1 + (speed * 0.1);
		
		particle.ScaleParticleParamFromOriginal(EmitorParam.LIFETIME, lifetimeScale);
		particle.ScaleParticleParamFromOriginal(EmitorParam.LIFETIME_RND, lifetimeScale);
		particle.ScaleParticleParamFromOriginal(EmitorParam.BIRTH_RATE, birthrateScale);
		particle.ScaleParticleParamFromOriginal(EmitorParam.BIRTH_RATE_RND, birthrateScale);
		particle.SetParticleParam(EmitorParam.SIZE, speed * 2);
		particle.SetParticleParam(EmitorParam.VELOCITY, speed * 2);
		particle.SetParticleParam(EmitorParam.AIR_RESISTANCE, 2.0);
	}

	// kept for possible backward compatibility
	override void SetParticleStateLight()
	{
		SetParticleState(ParticleList.HATCHBACK_EXHAUST_SMOKE);
		
		Car parent = Car.Cast(GetAttachmentParent());
		
		ParticleParamsOverrideData data = new ParticleParamsOverrideData();
		data.m_FloatModifiers.InsertAt(parent.GetSpeed().Length(), 0);
		ParticleParamsOverride(data);
	}
}
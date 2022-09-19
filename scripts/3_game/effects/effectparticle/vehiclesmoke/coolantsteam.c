class EffCoolantSteam : EffVehicleSmoke
{
	override void SetParticleStateLight()
	{
		SetParticleState( ParticleList.HATCHBACK_COOLANT_OVERHEATING );
		UpdateParticle();
	}
	
	override void SetParticleStateHeavy()
	{
		SetParticleState( ParticleList.HATCHBACK_COOLANT_OVERHEATED );
		UpdateParticle();
	}
	
	void UpdateParticle()
	{
		Car parent = Car.Cast( GetAttachmentParent() );
		Particle p = GetParticle();
		
		if (parent && p)
		{
			float speed = parent.GetSpeedometer();
				
			float lifetime_scale = (100 - speed) / 100;
			if (lifetime_scale < 0.1)
				lifetime_scale = 0.1;
				
			float birthrate_scale = 1 + (speed * 0.02 );
				
			float speed_scale = 1 + (speed * 0.2 );
			if (speed_scale > 10)
				speed_scale = 10;
				
			float size_scale = 1 + (speed * 0.05 );
				
			if (size_scale > 2.5)
				size_scale = 2.5;
				
			float gravity_add = speed * 0.005 ;
				
			p.ScaleParticleParamFromOriginal( EmitorParam.LIFETIME, lifetime_scale );
			p.ScaleParticleParamFromOriginal( EmitorParam.LIFETIME_RND, lifetime_scale );
			p.ScaleParticleParamFromOriginal( EmitorParam.BIRTH_RATE, birthrate_scale );
			p.ScaleParticleParamFromOriginal( EmitorParam.BIRTH_RATE_RND, birthrate_scale );
			p.ScaleParticleParamFromOriginal( EmitorParam.VELOCITY, speed_scale );
			p.ScaleParticleParamFromOriginal( EmitorParam.VELOCITY_RND, speed_scale );
			p.ScaleParticleParamFromOriginal( EmitorParam.SIZE, size_scale );
			p.SetParticleParam				( EmitorParam.GRAVITY_SCALE, gravity_add );
		}
	}
}
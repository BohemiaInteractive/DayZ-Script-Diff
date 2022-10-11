class EffExhaustSmoke : EffVehicleSmoke
{
	override void SetParticleStateLight()
	{
		SetParticleState( ParticleList.HATCHBACK_EXHAUST_SMOKE );
		
		Car parent = Car.Cast( GetAttachmentParent() );
		Particle p = GetParticle();
		
		if ( parent && p )
		{
			float speed = parent.GetSpeedometerAbsolute();
			float lifetime_scale;
				
			if (speed < 100)
				lifetime_scale = (100 - speed) / 100;
			else
				lifetime_scale = 0.1;
				
			float birthrate_scale = 1 + (speed * 0.1 );

			p.ScaleParticleParamFromOriginal( EmitorParam.LIFETIME, lifetime_scale );
			p.ScaleParticleParamFromOriginal( EmitorParam.LIFETIME_RND, lifetime_scale );
			p.ScaleParticleParamFromOriginal( EmitorParam.BIRTH_RATE, birthrate_scale );
			p.ScaleParticleParamFromOriginal( EmitorParam.BIRTH_RATE_RND, birthrate_scale );
		}
	}
}
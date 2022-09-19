class Hit_MeatBones : EffBulletImpactBase
{
	float m_ScalingByDistance;
	
	void Hit_MeatBones()
	{
		SetEnterParticle(ParticleList.IMPACT_MEATBONES_ENTER);
		SetExitParticle(ParticleList.IMPACT_MEATBONES_EXIT);
		SetRicochetParticle(ParticleList.IMPACT_MEATBONES_RICOCHET);
		
		m_AngledEnter = 10;
		m_EnterSplashCoef = 0.002;
		m_ExitSplashCoef = 0.006;
		m_ScalingByDistance = 0.05;
		
		MIN_SCALING_PARAM = 0.2;
	}
	
	override float CalculateStoppingForce(float in_speedf, float out_speedf, string ammoType, float weight)
	{
		if ( m_ImpactType == ImpactTypes.MELEE )
		{
			return 400;
		}
		
		float projectile_weight_coef = weight / DEFAULT_PROJECTILE_WEIGHT;
		
		float stopping_force = in_speedf * projectile_weight_coef;
		
		if (m_DirectHit)
		{
			/*
			// TO DO: Doesn't work because IsAlive() is false, even when it shouldn't be.
			if ( m_DirectHit.IsMan()  &&  m_componentIndex == SURVIVOR_HEAD  &&  m_DirectHit.IsAlive() ) // Is the victim a survivor?
			{
				stopping_force = stopping_force * 2;
			}
			*/
			
			/*
			// TO DO: Doesn't work. Need to recognize a zombie somehow
			else if ( m_DirectHit.IsInherited(DayZCreatureAIType)  &&  m_componentIndex == INFECTED_HEAD ) // Is the victim a survivor that's hit in the head?
			{
				stopping_force = stopping_force * 2;
			}*/
		}
		
		return stopping_force;
	}

	override void Event_OnStarted()
	{
		super.Event_OnStarted();
		
		if (m_ImpactType != ImpactTypes.MELEE)
		{
			vector in_speed = m_InSpeed * (-1); // Compiler demands to have this variable
			
			BloodSplatGround( GetPosition(), in_speed , 0.5 );
			
			if (m_OutSpeed.Length() > 0)
			{
				BloodSplatGround( m_ExitPos, m_OutSpeed, 0.8  );
			}
		}
	}
	
	void BloodSplatGround( vector start_pos, vector speed_vector, float decay_coef )
	{
		vector pos = start_pos;
		float power = m_StoppingForce;
		float upscale = 1;
		float rnd_offset = 0.5;
		float rnd_offset_2 = rnd_offset * 0.5;
		
		while (power > 200)
		{
			pos = pos + ( speed_vector * 0.001 );
			pos = pos + Vector( rnd_offset_2 - Math.RandomFloat( 0, rnd_offset ), 0, rnd_offset_2 - Math.RandomFloat( 0, rnd_offset ) );
			pos[1] = GetGame().SurfaceY(pos[0], pos[2]);
			
			EffectParticle eff = new BloodSplatter();
			eff.SetAutodestroy(true);
			SEffectManager.PlayInWorld(eff, pos);
			
			Particle blood = eff.GetParticle(); // TO DO: Handle particle changes inside the Effect instance itself! Not here!

			vector ori = GetGame().GetSurfaceOrientation(pos[0], pos[2]);
						
			blood.SetOrientation(ori);
			blood.ScaleParticleParam(EmitorParam.SIZE, upscale);
			
			Particle blood_chunks = ParticleManager.GetInstance().PlayInWorld(ParticleList.BLOOD_SURFACE_CHUNKS, pos);
			blood_chunks.SetOrientation(ori);
			
			power = power * decay_coef;
			upscale = upscale + Math.RandomFloat(0.1, 1);			
			
			BloodSplatWall();
		}
	}
	
	void BloodSplatWall()
	{
		// Commented out due to age rating process :(
		
		/*
		if (m_OutSpeed.Length() > 0)
		{
			Object projected_surface;
			vector hit_pos;
			vector hit_normal;
			float hit_fraction; 
			DayZPhysics.RayCastBullet(m_ExitPos, m_ExitPos + m_OutSpeed, PhxInteractionLayers.BUILDING, m_Object, projected_surface, hit_pos, hit_normal, hit_fraction);
			
			hit_normal = hit_normal.VectorToAngles();
			hit_normal[1] = hit_normal[1] + 90;
			
			EffectParticle eff = new BloodSplatter();
			eff.SetAutodestroy(true);
			SEffectManager.PlayInWorld(eff, hit_pos);
			Particle blood = eff.GetParticle();
			blood.SetOrientation(hit_normal);
		}
		*/
	}
	
	override void OnEnterCalculations( Particle p )
	{
		// Calculate particle's size based on bullet's speed
		float velocity_min = MIN_SCALING_PARAM + (m_StoppingForce * m_EnterSplashCoef);
		float velocity_max = MIN_SCALING_PARAM + (m_StoppingForce * m_EnterSplashCoef);
		float size = MIN_SCALING_PARAM + ( m_StoppingForce * m_EnterSplashCoef);
		float birth_rate = MIN_SCALING_PARAM + (m_StoppingForce * m_EnterSplashCoef/2);
		
		if ( m_AmmoType == "Bullet_12GaugePellets" )
		{
			birth_rate *= 0.5;
			velocity_min *= 2;
			velocity_max *= 2;
		}
		
		// Additional size increase by distance from camera
		vector camera_pos = GetGame().GetCurrentCameraPosition();
		float distance = vector.Distance(camera_pos, m_Pos);
		float scaling_by_distance = (distance*1.2) * m_ScalingByDistance;
		
		// Now scale down the above size increase by player's zoom-in value
		float current_FOV = Camera.GetCurrentFOV();
		float config_FOV = GetDayZGame().GetUserFOVFromConfig();
		float FOV_scale = current_FOV / config_FOV;
		scaling_by_distance = scaling_by_distance * FOV_scale;

		if (scaling_by_distance > 5)
			scaling_by_distance = 5;
		
		size = size + scaling_by_distance;
		velocity_min = velocity_min + scaling_by_distance;
		velocity_max = velocity_max + scaling_by_distance;
		
		if (velocity_min < MIN_SCALING_PARAM)
			velocity_min = MIN_SCALING_PARAM;
		
		if (velocity_max < MIN_SCALING_PARAM * 2)
			velocity_max = MIN_SCALING_PARAM * 2;
		
		if (size < MIN_SCALING_PARAM)
			size = MIN_SCALING_PARAM;
		
		if (birth_rate < MIN_SCALING_PARAM)
			birth_rate = MIN_SCALING_PARAM;
		
		p.ScaleParticleParam(EmitorParam.VELOCITY, velocity_min);
		p.ScaleParticleParam(EmitorParam.VELOCITY_RND, velocity_max);
		p.ScaleParticleParam(EmitorParam.SIZE, size);
		p.ScaleParticleParam(EmitorParam.BIRTH_RATE, birth_rate);
	}
	
	override void OnExitCalculations(Particle p, float outSpeedf)
	{
		float velocity_rnd = outSpeedf * m_ExitSplashCoef;
		float birth_rate_rnd_def = outSpeedf * m_ExitSplashCoef;
		float size = outSpeedf * m_ExitSplashCoef;
		
		if (velocity_rnd < MIN_SCALING_PARAM)
			velocity_rnd = MIN_SCALING_PARAM;
		
		if (size < MIN_SCALING_PARAM)
			size = MIN_SCALING_PARAM;
		
		if (size > 1)
			size = 1;
		
		if (birth_rate_rnd_def < MIN_SCALING_PARAM)
			birth_rate_rnd_def = MIN_SCALING_PARAM;
		
		p.ScaleParticleParam(EmitorParam.VELOCITY_RND, velocity_rnd);
		p.ScaleParticleParam(EmitorParam.BIRTH_RATE, birth_rate_rnd_def);
		p.ScaleParticleParam(EmitorParam.SIZE, size);
	}
}
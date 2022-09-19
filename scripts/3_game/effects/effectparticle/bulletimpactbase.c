class EffBulletImpactBase : EffectParticle
{
	static const int	SURVIVOR_HEAD = 0; // Head component
	static const int	INFECTED_HEAD = 3; // Head component
	static float		DEFAULT_PROJECTILE_WEIGHT = 0.015;
	
	float				MIN_SCALING_PARAM = 0.1;
	
	Object 				m_DirectHit;
	float 				m_StoppingForce;
	float 				m_Weight; // projectile weight in kg
	int 				m_ImpactType;
	int 				m_ComponentIndex;
	vector 				m_Pos;
	vector 				m_SurfNormal;
	vector 				m_ExitPos;
	vector 				m_InSpeed;
	vector 				m_OutSpeed;
	string 				m_AmmoType;
	
	static vector INVALID = "0 0 0";
	
	// Particle Effects
	int m_ParticleEnter = -1;
	int m_ParticleExit = -1;
	int m_ParticleRicochet = -1;
	
	// Calculations
	float m_EnterSplashCoef = 0.003;
	float m_ExitSplashCoef = 0.002;
	float m_RicochetSplashCoef = 0.002;
	float m_EnterAngledSplashCoef = 0.01;
	float m_AngledEnter = 0.40;
	
	void EffBulletImpactBase()
	{
		
	}
	
	override void OnCheckUpdate()
	{
		//DbgUI.Text( m_ammoType );
	}
	
	void SetEnterParticle(int id)
	{
		m_ParticleEnter = id;
	}
	
	void SetExitParticle(int id)
	{
		m_ParticleExit = id;
	}
	
	void SetRicochetParticle(int id)
	{
		m_ParticleRicochet = id;
	}
	
	void SetSingleParticle(int id)
	{
		SetEnterParticle(id);
		SetExitParticle(id);
		SetRicochetParticle(id);
	}
	
	void SetAngledEnterValue(float f)
	{
		m_AngledEnter = f;
	}
	
	void EvaluateEffect(Object directHit, int componentIndex, vector pos, int impact_type, vector surfNormal, vector exitPos, vector inSpeed, vector outSpeed, string ammoType)
	{
		m_DirectHit 		= directHit;
		m_Pos 				= pos;
		m_ImpactType 		= impact_type;
		m_ComponentIndex 	= componentIndex;
		m_SurfNormal 		= surfNormal;
		m_ExitPos 			= exitPos;
		m_InSpeed 			= inSpeed;
		m_OutSpeed 			= outSpeed;
		m_AmmoType 			= ammoType;
		m_Weight			= GetGame().ConfigGetFloat("CfgAmmo " + ammoType + " weight");
		
		m_StoppingForce 	= CalculateStoppingForce(m_InSpeed.Length(), m_OutSpeed.Length(), ammoType, m_Weight);
	}
	
	float CalculateStoppingForce(float in_speedf, float out_speedf, string ammoType, float weight)
	{
		if ( m_ImpactType == ImpactTypes.MELEE )
		{
			return Math.RandomFloat(50, 100);
		}
		
		float projectile_weight_coef = weight / DEFAULT_PROJECTILE_WEIGHT;
		
		float stopping_force = (in_speedf - out_speedf) * projectile_weight_coef;
		
		return stopping_force;
	}
	
	void OnEnterCalculations( Particle p )
	{
		// All values represent scale
		float velocity_min 		= (m_StoppingForce * m_EnterSplashCoef);
		float velocity_max 		= (m_StoppingForce * m_EnterSplashCoef);
		float size 				= (m_StoppingForce * m_EnterSplashCoef);
		float birth_rate 		= (m_StoppingForce * m_EnterSplashCoef);
		float air_resistance 	= velocity_min;
		float lifetime 			= (m_StoppingForce * m_EnterSplashCoef);
		float lifetime_rnd		= (m_StoppingForce * m_EnterSplashCoef);
		
		if ( m_AmmoType == "Bullet_12GaugePellets" )
		{
			birth_rate *= 0.5;
			velocity_min *= 2;
			velocity_max *= 2;
		}
		
		
		if (velocity_min < 0.75)
			velocity_min = 0.75;
		
		if (size < 0.75)
			size = 0.75;
		
		if (lifetime < 0.5)
			lifetime = 0.5;
		
		if (lifetime_rnd < 0.5)
			lifetime_rnd = 0.5;
		
		if (velocity_max < 1)
			velocity_max = 1;
		
		/*Print("===============");
		Print(velocity_min);
		Print(velocity_max);
		Print(size);
		Print(birth_rate);
		Print(air_resistance);
		Print(lifetime);
		Print(lifetime_rnd);*/
		
		p.ScaleParticleParam(EmitorParam.VELOCITY, velocity_min);
		p.ScaleParticleParam(EmitorParam.VELOCITY_RND, velocity_max);
		p.ScaleParticleParam(EmitorParam.SIZE, size);
		p.ScaleParticleParam(EmitorParam.BIRTH_RATE, birth_rate);
		p.ScaleParticleParam(EmitorParam.AIR_RESISTANCE, air_resistance);
		p.ScaleParticleParam(EmitorParam.AIR_RESISTANCE_RND, air_resistance);
		p.ScaleParticleParam(EmitorParam.LIFETIME, lifetime);
		p.ScaleParticleParam(EmitorParam.LIFETIME_RND, lifetime_rnd);
	}
	
	void OnExitCalculations(Particle p, float outSpeedf)
	{
		float velocity_min = 1 + (outSpeedf * m_ExitSplashCoef);
		float velocity_max = 1 + (outSpeedf * m_ExitSplashCoef);
		float size = 1 + ( outSpeedf * m_ExitSplashCoef);
		float birth_rate = 1 + (outSpeedf * m_ExitSplashCoef);
		
		if (velocity_min < MIN_SCALING_PARAM)
			velocity_min = MIN_SCALING_PARAM;
		
		if (size < MIN_SCALING_PARAM)
			size = MIN_SCALING_PARAM;
		
		if (birth_rate < MIN_SCALING_PARAM)
			birth_rate = MIN_SCALING_PARAM;
		
		p.ScaleParticleParam(EmitorParam.VELOCITY, velocity_min);
		p.ScaleParticleParam(EmitorParam.VELOCITY_RND, velocity_max);
		p.ScaleParticleParam(EmitorParam.SIZE, size);
		p.ScaleParticleParam(EmitorParam.BIRTH_RATE, birth_rate);
	}
	
	void OnRicochetCalculations(Particle p, float outspeedf)
	{
		float velocity_min = MIN_SCALING_PARAM + (m_StoppingForce * m_RicochetSplashCoef);
		float velocity_max = MIN_SCALING_PARAM + (m_StoppingForce * m_RicochetSplashCoef);
		float size = MIN_SCALING_PARAM + ( m_StoppingForce * m_RicochetSplashCoef);
		float birth_rate = MIN_SCALING_PARAM + (m_StoppingForce * m_RicochetSplashCoef);
		
		if (velocity_min < MIN_SCALING_PARAM)
			velocity_min = MIN_SCALING_PARAM;
		
		if (size < MIN_SCALING_PARAM)
			size = MIN_SCALING_PARAM;
		
		if (birth_rate < MIN_SCALING_PARAM)
			birth_rate = MIN_SCALING_PARAM;
		
		p.ScaleParticleParam(EmitorParam.VELOCITY, velocity_min);
		p.ScaleParticleParam(EmitorParam.VELOCITY_RND, velocity_max);
		p.ScaleParticleParam(EmitorParam.SIZE, size);
		p.ScaleParticleParam(EmitorParam.BIRTH_RATE, birth_rate);
	}
	
	void OnEnterAngledCalculations(Particle p)
	{
		float velocity_min = MIN_SCALING_PARAM + (m_StoppingForce * m_EnterAngledSplashCoef);
		float velocity_max = MIN_SCALING_PARAM + (m_StoppingForce * m_EnterAngledSplashCoef);
		float size = MIN_SCALING_PARAM + (m_StoppingForce * m_EnterAngledSplashCoef);
		float birth_rate = MIN_SCALING_PARAM + (m_StoppingForce * m_EnterAngledSplashCoef);
		
		if (velocity_min < MIN_SCALING_PARAM)
			velocity_min = MIN_SCALING_PARAM;
		
		if (size < MIN_SCALING_PARAM)
			size = MIN_SCALING_PARAM;
		
		if (birth_rate < MIN_SCALING_PARAM)
			birth_rate = MIN_SCALING_PARAM;
		
		p.ScaleParticleParam(EmitorParam.VELOCITY, velocity_min);
		p.ScaleParticleParam(EmitorParam.VELOCITY_RND, velocity_max);
		p.ScaleParticleParam(EmitorParam.SIZE, size);
		p.ScaleParticleParam(EmitorParam.BIRTH_RATE, birth_rate);
	}
	
	override void Event_OnStarted()
	{
		Particle p;
		vector particle_orientation;
		float outSpeedf = m_OutSpeed.Length();
		
		ParticleManager gPM = ParticleManager.GetInstance();
		
		if ( m_ImpactType == ImpactTypes.RICOCHET )
		{
			p = gPM.PlayInWorld(m_ParticleRicochet, m_Pos);
			
			if (p)
			{
				particle_orientation = m_OutSpeed.VectorToAngles();
				particle_orientation = particle_orientation + "0 -90 0";
				p.SetOrientation(particle_orientation);
				
				OnRicochetCalculations(p, outSpeedf);
			}
		}
		else
		{
			p = gPM.PlayInWorld(m_ParticleEnter, m_Pos );
			
			if (p)
			{
				if (m_SurfNormal != INVALID)
				{
					particle_orientation = m_SurfNormal.VectorToAngles();
					particle_orientation = particle_orientation + "0 270 0";
				}
				else
				{
					particle_orientation = "0 0 0"; // This vector is in angles
				}
				
				p.SetOrientation(particle_orientation);
			
				OnEnterCalculations(p);
			}
			
			if (outSpeedf > 0  &&  m_SurfNormal != INVALID)
			{
				p = gPM.PlayInWorld(m_ParticleExit, m_ExitPos);
				
				if (p)
				{
					particle_orientation = m_OutSpeed.VectorToAngles();
					particle_orientation = particle_orientation + "0 -90 0";
					p.SetOrientation(particle_orientation);
					
					OnExitCalculations(p, outSpeedf);
				}
			}
			else
			{
				if (m_SurfNormal != INVALID)
				{
					vector surfNormalN = m_SurfNormal.Normalized();
					vector inSpeedN = m_InSpeed.Normalized();
					vector bounce_ori = surfNormalN + inSpeedN;
					
					float dot = vector.Dot(bounce_ori, surfNormalN);
					
					if ( dot > m_AngledEnter )
					{
						p = gPM.PlayInWorld(m_ParticleRicochet, m_Pos);
			
						if (p)
						{
							particle_orientation = bounce_ori.VectorToAngles();
							particle_orientation = particle_orientation + "0 -90 0";
							p.SetOrientation(particle_orientation);
							
							OnEnterAngledCalculations(p);
						}
					}
				}
			}
		}
		
		if (p)
		{
			SetParticle(p);
		}
		
		
		// Additional impact particle over long ranges. It shows players where their bullets land
		
		if ( Type() != Hit_MeatBones )
		{
			vector camera_pos = GetGame().GetCurrentCameraPosition();
			float distance = vector.Distance(camera_pos, m_Pos);
						
			// Additional size increase by distance from camera
			float scaling_by_distance = distance * 0.01;
			
			// Now scale down the above size increase by player's zoom-in value
			float current_FOV = Camera.GetCurrentFOV();
			float config_FOV = GetDayZGame().GetUserFOVFromConfig();
			float FOV_scale = current_FOV / config_FOV;
			scaling_by_distance = 1 + scaling_by_distance * FOV_scale;
			
			if (scaling_by_distance > 1.1)
			{
				Particle p_distant = gPM.PlayInWorld(ParticleList.IMPACT_DISTANT_DUST, m_Pos);
				
				particle_orientation = m_SurfNormal.VectorToAngles();
				particle_orientation[1] = particle_orientation[1] + 270;
				p_distant.SetOrientation(particle_orientation);
				
				p_distant.ScaleParticleParam(EmitorParam.SIZE, scaling_by_distance - 0.5);
				p_distant.ScaleParticleParam(EmitorParam.BIRTH_RATE, scaling_by_distance * 0.1);
				p_distant.ScaleParticleParam(EmitorParam.BIRTH_RATE_RND, scaling_by_distance * 0.1);
				p_distant.ScaleParticleParam(EmitorParam.LIFETIME, scaling_by_distance * 0.3);
				p_distant.ScaleParticleParam(EmitorParam.LIFETIME_RND, scaling_by_distance * 0.3);
			}
		}
	}
}
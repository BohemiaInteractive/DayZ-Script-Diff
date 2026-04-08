class FlareSimulation : Managed
{
	protected Particle 			m_ParMainFire;
	protected EffectSound 		m_BurningSound;
	protected FlareLight		m_FlareLight;
	const static float			MAX_FARLIGHT_DIST = 40;
	const static float			MIN_FARLIGHT_DIST = 5; 
	
	static ref NoiseParams 		m_NoisePar; // Contains the noise data ( template and strength )
	float 						m_LastNoiseTime = -1;
	float 						m_NoiseTimer = 0;
	const float 				NOISE_DELAY = 5; // How much time between two consecutive noise pings
	
	// flare effect rotation
	protected const float		FLARE_SPIN_RATE = 1.15;		// in degrees per simul tick
	protected const float 		FLARE_SPIN_RADIUS = 0.18;	// radius of circling motion
	protected Entity			m_Flare;
	protected vector			m_RotationPoint;
	protected vector			m_FlarePosition;
	protected float 			m_RotationDegrees;
	
	static protected typename 	m_ScriptedLight;
	static protected int 		m_ParticleId;
	
	void FlareSimulation()
	{
		m_ScriptedLight	 = FlareLight;
		m_ParticleId	 = ParticleList.FLAREPROJ_ACTIVATE;
	}
		
	void OnActivation(Entity flare)
	{
		if ( !g_Game.IsServer() || !g_Game.IsMultiplayer() )
		{
			m_FlareLight = FlareLight.Cast( ScriptedLightBase.CreateLight( m_ScriptedLight, Vector(0,0,0) ) );
			if ( m_FlareLight )
				m_FlareLight.AttachOnObject( flare );
			
			if (m_ParMainFire)
				m_ParMainFire.Stop();

			m_Flare = flare;
			m_RotationPoint = m_Flare.GetOrigin();
			m_FlarePosition = m_Flare.GetOrigin() + Vector(FLARE_SPIN_RADIUS,0,FLARE_SPIN_RADIUS);
			
			m_ParMainFire = ParticleManager.GetInstance().PlayInWorld( m_ParticleId, m_FlarePosition );
			
			flare.PlaySoundSetLoop( m_BurningSound, "roadflareLoop_SoundSet", 0, 0 );
		}
		
		if ( g_Game.IsServer() )
		{
			// Create and load noise parameters
			m_NoisePar = new NoiseParams();
			m_NoisePar.LoadFromPath("cfgWeapons Flaregun_Base NoiseFlare");
		}
	}
	
	void OnTermination(Entity flare)
	{
		//MiscGameplayFunctions.GenerateAINoiseAtPosition(flare.GetPosition(), NOISE_DELAY, m_NoisePar);
	}
	
	void OnFire( Entity flare)
	{
		//m_ParMainFire = ParticleManager.GetInstance().PlayOnObject( ParticleList.FLAREPROJ_FIRE, flare);
		//m_ParMainFire.SetWiggle( 7, 0.3);
	}
	
	void Simulate( Entity flare )
	{	
		DayZPlayer player = g_Game.GetPlayer();
		if ( player )
			vector playerPos = player.GetPosition();

		float dist = vector.DistanceSq(flare.GetPosition(), playerPos);
			
		if ( ( dist <= MAX_FARLIGHT_DIST * MAX_FARLIGHT_DIST ) && ( dist > MIN_FARLIGHT_DIST * MIN_FARLIGHT_DIST ) )
			m_ParMainFire.SetParameter( 0, EmitorParam.SIZE, MiscGameplayFunctions.Normalize(dist, MAX_FARLIGHT_DIST * MAX_FARLIGHT_DIST) );
			
		if ( dist <= MIN_FARLIGHT_DIST * MIN_FARLIGHT_DIST )
			TurnOffDistantLight();

		//CastFlareAINoise( flare.GetPosition() );
		FlareParticleUpdate();
	}
	
	// Rotate flare particle and set its new position
	protected void FlareParticleUpdate()
	{
		m_RotationDegrees += FLARE_SPIN_RATE;
		if (m_RotationDegrees > 360)
			m_RotationDegrees = 0;
		
		float angleRad = m_RotationDegrees * Math.DEG2RAD;
		float sin = Math.Sin(angleRad);
		float cos = Math.Cos(angleRad);
		
		vector newFlarePos = m_ParMainFire.GetOrigin();
		float surfacePos = g_Game.SurfaceY(newFlarePos[0], newFlarePos[2]);
		
		if (newFlarePos[1] - surfacePos < 1) // reached ground
		{
			if (m_Flare.GetOrigin()[1] <= surfacePos)	// actual flare pos might not match and height could be underground
				m_ParMainFire.SetPosition(Vector(newFlarePos[0], surfacePos, newFlarePos[2]));
			else 
				m_ParMainFire.SetPosition(Vector(newFlarePos[0], m_Flare.GetOrigin()[1] ,newFlarePos[2]));
			
			return;
		}

		// rotate vector around point
		float xRotated = ((m_FlarePosition[0] - m_RotationPoint[0]) * cos) - ((m_FlarePosition[2] - m_RotationPoint[2]) * sin) + m_RotationPoint[0];
		float yRotated = ((m_FlarePosition[0] - m_RotationPoint[0]) * sin) + ((m_FlarePosition[2] - m_RotationPoint[2]) * cos) + m_RotationPoint[2];
		
		newFlarePos[0] = xRotated;
		newFlarePos[1] = m_Flare.GetOrigin()[1];
		newFlarePos[2] = yRotated;
		
		m_ParMainFire.SetPosition(newFlarePos);	

	}
	
	void CastFlareAINoise( vector position )
	{
		int currentTime = g_Game.GetTime();
		float timeAdjusted = currentTime * 0.0033;
		if ( m_LastNoiseTime < 0 )
			m_LastNoiseTime = timeAdjusted;

		float delta_time = timeAdjusted - m_LastNoiseTime;
		m_LastNoiseTime = timeAdjusted;
		
		m_NoiseTimer += delta_time;

		if ( m_NoiseTimer >= NOISE_DELAY )
		{
			MiscGameplayFunctions.GenerateAINoiseAtPosition(position, NOISE_DELAY, m_NoisePar);
		
			m_NoiseTimer = 0;
		}
	}
	
	void TurnOffDistantLight()
	{
		if (m_ParMainFire)
		{
			m_ParMainFire.SetParameter(0, EmitorParam.LIFETIME, 0);
			m_ParMainFire.SetParameter(0, EmitorParam.LIFETIME_RND, 0);
			m_ParMainFire.SetParameter(0, EmitorParam.REPEAT, 0);
			m_ParMainFire.SetParameter(0, EmitorParam.SIZE, 0);
		}
	}
	
	void ~FlareSimulation()
	{
		if (m_ParMainFire)
			m_ParMainFire.Stop();
		
		if (m_BurningSound)
			m_BurningSound.SoundStop();
		
		if (m_FlareLight)
			m_FlareLight.FadeOut();
	}
}

class FlareSimulation_Red : FlareSimulation
{
	void FlareSimulation_Red()
	{
		m_ScriptedLight	 = FlareLightRed;
		m_ParticleId	 = ParticleList.FLAREPROJ_ACTIVATE_RED;
	}
}

class FlareSimulation_Green : FlareSimulation
{
	void FlareSimulation_Green()
	{
		m_ScriptedLight	 = FlareLightGreen;
		m_ParticleId	 = ParticleList.FLAREPROJ_ACTIVATE_GREEN;
	}
}

class FlareSimulation_Blue : FlareSimulation
{
	void FlareSimulation_Blue()
	{
		m_ScriptedLight	 = FlareLightBlue;
		m_ParticleId	 = ParticleList.FLAREPROJ_ACTIVATE_BLUE;
	}
}
class FlareSimulation : Entity
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
	
	static protected typename 	m_ScriptedLight;
	static protected int 		m_ParticleId;
	
	void FlareSimulation()
	{
		m_ScriptedLight	 = FlareLight;
		m_ParticleId	 = ParticleList.FLAREPROJ_ACTIVATE;
	}
	
	void OnActivation(Entity flare)
	{
		if ( !GetGame().IsServer() || !GetGame().IsMultiplayer() )
		{
			m_FlareLight = FlareLight.Cast( ScriptedLightBase.CreateLight( m_ScriptedLight, Vector(0,0,0) ) );
			if ( m_FlareLight )
				m_FlareLight.AttachOnObject( flare );
			
			if (m_ParMainFire)
				m_ParMainFire.Stop();
	
			m_ParMainFire = ParticleManager.GetInstance().PlayOnObject( m_ParticleId, flare );
			m_ParMainFire.SetWiggle( 7, 0.3 );
			
			flare.PlaySoundSetLoop( m_BurningSound, "roadflareLoop_SoundSet", 0, 0 );
		}
		
		if ( GetGame().IsServer() )
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
		DayZPlayer player = GetGame().GetPlayer();
		if ( player )
			vector playerPos = player.GetPosition();

		float dist = vector.DistanceSq(flare.GetPosition(), playerPos);
			
		if ( ( dist <= MAX_FARLIGHT_DIST * MAX_FARLIGHT_DIST ) && ( dist > MIN_FARLIGHT_DIST * MIN_FARLIGHT_DIST ) )
			m_ParMainFire.SetParameter( 0, EmitorParam.SIZE, MiscGameplayFunctions.Normalize(dist, MAX_FARLIGHT_DIST * MAX_FARLIGHT_DIST) );
			
		if ( dist <= MIN_FARLIGHT_DIST * MIN_FARLIGHT_DIST )
			TurnOffDistantLight();

		//CastFlareAINoise( flare.GetPosition() );
	}
	
	void CastFlareAINoise( vector position )
	{
		if ( m_LastNoiseTime < 0 )
			m_LastNoiseTime = GetGame().GetTime() * 0.0033;

		float delta_time = GetGame().GetTime() * 0.0033 - m_LastNoiseTime;
		m_LastNoiseTime = GetGame().GetTime() * 0.0033;
		
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
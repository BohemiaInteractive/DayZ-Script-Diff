class ShockHandler
{	
	protected float 						m_Shock;
	protected float 						m_ShockValueMax;
	protected float 						m_ShockValueThreshold;
	protected PlayerBase					m_Player;
	
	protected const float 					UPDATE_THRESHOLD = 3; //NOTE : The lower, the more precise but the more synchronization
	const float 							VALUE_CHECK_INTERVAL = 0.95; //in seconds 
	protected float 						m_CumulatedShock;
	private float							m_TimeSinceLastTick = VALUE_CHECK_INTERVAL + 1;
	private float							m_ShockMultiplier = 1;
	private float 							m_PrevVignette; //Previous vignette shock value
	private	float 							m_LerpRes; //Lerp result
	
	private const int						LIGHT_SHOCK_HIT = 33;
	private const int						MID_SHOCK_HIT = 67;
	private const int						HEAVY_SHOCK_HIT = 100;
	private const int 						INTENSITY_FACTOR = 1; //How intense the vignette effect will be, the higher the value, the stronger the effect
	
	//Pulsing effect
	private const float 					PULSE_PERIOD = 0.5; //The time it takes for pulse to do a full cycle
	private const float 					PULSE_AMPLITUDE = 0.05; //This is a multiplier, keep below 1 or expect the unexpected
	private 	  float 					m_PulseTimer;
	
	protected ref Param1<float> 			m_Param;
	
	void ShockHandler(PlayerBase player)
	{
		m_Player = player;
		m_Player.m_CurrentShock = m_Player.GetMaxHealth("", "Shock");
		m_PrevVignette = m_Player.m_CurrentShock * 0.01; //Equivalent to divided by 100
		m_ShockValueMax = m_Player.GetMaxHealth("", "Shock");
		m_ShockValueThreshold = m_ShockValueMax * 0.95;
		m_Param = new Param1<float>(0);
	}

	void Update(float deltaT)
	{
		m_TimeSinceLastTick += deltaT;
		
		m_PulseTimer += deltaT;
		
		if ( GetGame().IsClient() )
		{
			//Deactivate tunnel vignette when player falls unconscious
			if ( m_Player.IsUnconscious() )
			{
				PPERequesterBank.GetRequester(PPERequester_TunnelVisionEffects).Stop();
				return;
			}
			
			//Deactivate if above visible threshold (also stops "zero bobbing" being sent all the time
			if ( m_Player.m_CurrentShock >= m_ShockValueThreshold)
			{
				PPERequesterBank.GetRequester(PPERequester_TunnelVisionEffects).Stop();
				return;
			}
			
			//Add bobbing to create pulsing effect
			float val = 0.0;
			if ( m_Player.m_CurrentShock > m_ShockValueMax * 0.8)
				val = MiscGameplayFunctions.Bobbing( PULSE_PERIOD, PULSE_AMPLITUDE, m_PulseTimer );
			float val_adjusted;
			
			if ( m_Player.m_CurrentShock != (m_PrevVignette * 100) )
			{
				//Interpolate between previous level and currently synchronized shock level
				m_LerpRes = LerpVignette( m_PrevVignette, NormalizeShockVal(m_Player.m_CurrentShock), m_TimeSinceLastTick );
				
				val_adjusted = 1 - Easing.EaseInQuart(m_LerpRes) + val;
			}
			else
			{
				val_adjusted = 1 - Easing.EaseInQuart( NormalizeShockVal(m_Player.m_CurrentShock)) + val;
			}
			
			m_Param.param1 = val_adjusted;
			PPERequesterBank.GetRequester(PPERequester_TunnelVisionEffects).Start(m_Param);
		}
		
		if ( m_TimeSinceLastTick > VALUE_CHECK_INTERVAL )
		{
			//Play the shock hit event (multiply prevVignette by 100 to "Un-Normalize" value)
			if ( GetGame().IsClient() )
			{
				ShockHitEffect( m_PrevVignette * 100 );
				m_PrevVignette = m_Player.m_CurrentShock * 0.01;
			}
			
			CheckValue( false );
			m_TimeSinceLastTick = 0;
		}
	}
	
	float GetCurrentShock()
	{
		return m_Player.m_CurrentShock;
	}
	
	float GetShock()
	{
		return m_Shock;
	}
	
	void SetShock( float dealtShock )
	{
		m_Shock += dealtShock;
		CheckValue( false );
	}
	
	//Inflict shock damage
	private void DealShock()
	{
		if ( GetGame().IsServer() )
			m_Player.GiveShock( -m_CumulatedShock );
	}
	
	//Passing a value of FALSE will only check the values, a value of TRUE will force a synchronization (don't use too often)
	void CheckValue(bool forceUpdate)
	{
		m_CumulatedShock += m_Shock; // increment on both client and server
		
		if ( forceUpdate )
			m_PrevVignette = NormalizeShockVal( m_Player.m_CurrentShock );
		
		if ( GetGame().IsServer() )
		{	
			m_Player.m_CurrentShock = m_Player.GetHealth("", "Shock");
			
			/*
			if (m_Player.m_CurrentShock <= 0)
				m_Player.SetHealthMax("", "Shock");
			*/
			if ( m_CumulatedShock >= UPDATE_THRESHOLD || forceUpdate )
			{
				m_CumulatedShock *= m_ShockMultiplier;
				DealShock();
				m_CumulatedShock = 0;
				m_Shock = 0;
			
				Synchronize();
			}
		}
	}
	
	protected void Synchronize()
	{
		DayZPlayerSyncJunctures.SendShock( m_Player, m_Player.m_CurrentShock );
	}
	
	float SetMultiplier(float mult)
	{
		m_ShockMultiplier = mult;
		return m_ShockMultiplier;
	}
	
	private float NormalizeShockVal( float shock )
	{
		float base = m_Player.GetMaxHealth("", "Shock") * INTENSITY_FACTOR;
		float normShock = shock / base;
		return normShock;
	}
	
	private float LerpVignette( float x, float y, float deltaT )
	{
		float output;
		output = Math.Lerp( x, y, deltaT );
		return output;
	}
	
	//ONLY CLIENT SIDE
	private void ShockHitEffect( float compareBase )
	{
		float shockDifference = compareBase - m_Player.m_CurrentShock;
		//Print(shockDifference);
		if ( shockDifference >= UPDATE_THRESHOLD )
		{
			if ( m_CumulatedShock < 25 )
				m_Player.SpawnShockEffect( MiscGameplayFunctions.Normalize( LIGHT_SHOCK_HIT, 100 ) ); 
			else if ( m_CumulatedShock < 50 )
				m_Player.SpawnShockEffect( MiscGameplayFunctions.Normalize( MID_SHOCK_HIT, 100 ) );
			else
				m_Player.SpawnShockEffect( MiscGameplayFunctions.Normalize( HEAVY_SHOCK_HIT, 100 ) );
		}
	}
};
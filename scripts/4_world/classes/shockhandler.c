class ShockHandler
{	
	protected const float 					UPDATE_THRESHOLD = 3; //NOTE : The lower, the more precise but the more synchronization
	const float 							VALUE_CHECK_INTERVAL = 0.95; //in seconds
	private const int 						INTENSITY_FACTOR = 1; //How intense the vignette effect will be, the higher the value, the stronger the effect
	private const int 						VIGNETTE_INTENSITY_MAX = 1; //Max BASE intensity of the vignette effect (w/o. bobbing). 0..2, where 2 is full screen coverage
	private const int 						VIGNETTE_INTENSITY_MAX_TOTAL = 2; //Max TOTAL intensity of the vignette effect (w. bobbing). 0..2, where 2 is full screen coverage
	private const float						SMOOTHING_MAX_INCR = 0.05; //max smoothing change INCREASE per update
	private const float						SMOOTHING_MAX_DECR = 0.01; //max smoothing change DECREASE per update
	//bobbing constants
	private const float 					PULSE_PERIOD = 0.5; //The time it takes for pulse to do a full cycle
	private const float 					PULSE_AMPLITUDE = 0.05; //This is a multiplier, keep below 1 or expect the unexpected
	
	protected float 						m_Shock;
	protected float 						m_LastEffectIntensityValue; //for interpolation only
	protected float 						m_ShockValueMax;
	protected float 						m_CumulatedShock; //! works ok on server, but does nothing for client. See deprecated stuff.
	private float							m_TimeSinceLastTick = VALUE_CHECK_INTERVAL + 1;
	private float							m_ShockMultiplier = 1;
	private float 							m_PrevVignette; //Previous shock-adjecent value (some normalization required). Client sets it only on regular ShockHandler update!
	
	//bobbing effect
	private float 							m_PulseTimer;
	
	//PPE
	PPERequester_TunnelVisionEffects 		m_Requester;
	protected ref Param1<float> 			m_Param;
	
	protected PlayerBase					m_Player;
	
	void ShockHandler(PlayerBase player)
	{
		m_Player = player;
		m_Player.m_CurrentShock = m_Player.GetMaxHealth("", "Shock");
		m_ShockValueMax = m_Player.GetMaxHealth("", "Shock");
		m_PrevVignette = m_Player.m_CurrentShock / m_ShockValueMax; //Equivalent to divided by 100
		m_Requester = PPERequester_TunnelVisionEffects.Cast(PPERequesterBank.GetRequester(PPERequester_TunnelVisionEffects));
		m_Param = new Param1<float>(0);
		
		//loegacy stuff
		m_ShockValueThreshold = m_ShockValueMax * 0.95;
	}

	void Update(float deltaT)
	{
		m_TimeSinceLastTick += deltaT;
		m_PulseTimer += deltaT;
		
		//periodical update, just in case
		if (m_TimeSinceLastTick > VALUE_CHECK_INTERVAL)
		{
			if (g_Game.IsClient())
			{
				//ShockHitEffect(m_PrevVignette * m_ShockValueMax);
				m_PrevVignette = m_Player.m_CurrentShock / m_ShockValueMax;
			}
			
			CheckValue(false);
			m_TimeSinceLastTick = 0;
		}
		
		if (g_Game.IsClient())
		{
			float valAdjusted = BaseEffectIntensityCalc();
			
			if (valAdjusted <= 0)
			{
				if (m_Requester.IsRequesterRunning())
					m_Requester.Stop();
				
				return;
			}
			
			//Print("dbgShock | valAdjusted: " + valAdjusted);
			
			//Add bobbing to create pulsing effect
			valAdjusted = AddEffectBobbing(valAdjusted);
			
			m_Param.param1 = valAdjusted;
			m_Requester.Start(m_Param);
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
	
	void SetShock(float dealtShock)
	{
		m_Shock += dealtShock;
		CheckValue(false);
	}
	
	//Inflict shock damage
	private void DealShock()
	{
		if (g_Game.IsServer())
			m_Player.GiveShock(-m_CumulatedShock);
	}
	
	//Passing a value of FALSE will only check the values, a value of TRUE will force a synchronization (don't use too often)
	void CheckValue(bool forceUpdate)
	{
		m_CumulatedShock += m_Shock; // increment on both client and server
		
		if (forceUpdate)
			m_PrevVignette = NormalizeShockVal(m_Player.m_CurrentShock);
		
		if (g_Game.IsServer())
		{
			if (m_CumulatedShock >= UPDATE_THRESHOLD || forceUpdate)
			{
				m_CumulatedShock *= m_ShockMultiplier;
				DealShock();
				m_CumulatedShock = 0;
				m_Shock = 0;
			
				Synchronize();
			}
			
			m_Player.m_CurrentShock = m_Player.GetHealth("", "Shock");
		}
	}
	
	protected void Synchronize()
	{
		DayZPlayerSyncJunctures.SendShock(m_Player, m_Player.m_CurrentShock);
	}
	
	protected float BaseEffectIntensityCalc()
	{
		float effectIntensity = 1 - Easing.EaseInQuart(NormalizeShockVal(m_Player.m_CurrentShock));
		
		//smoothing
		if (effectIntensity != m_LastEffectIntensityValue)
			effectIntensity = Math.Clamp(effectIntensity,m_LastEffectIntensityValue - SMOOTHING_MAX_DECR, m_LastEffectIntensityValue + SMOOTHING_MAX_INCR);
		
		m_LastEffectIntensityValue = effectIntensity;
		
		return effectIntensity;
	}
	
	//! adds bobbing, also clamps to valid range
	protected float AddEffectBobbing(float baseVal)
	{
		float ret = baseVal;
		float bobbingVal = 0.0;
		
		if (m_Player.m_CurrentShock > m_ShockValueMax * 0.8)
			bobbingVal = MiscGameplayFunctions.Bobbing(PULSE_PERIOD, PULSE_AMPLITUDE, m_PulseTimer);
		ret += bobbingVal;
		ret = Math.Clamp(ret,0,VIGNETTE_INTENSITY_MAX_TOTAL);
		
		return ret;
	}
	
	float SetMultiplier(float mult)
	{
		m_ShockMultiplier = mult;
		return m_ShockMultiplier;
	}
	
	private float NormalizeShockVal(float shock)
	{
		float base = m_ShockValueMax * INTENSITY_FACTOR;
		float normShock = shock / base;
		return normShock;
	}
	
	private float LerpVignette(float x, float y, float deltaT)
	{
		float output;
		output = Math.Lerp(x, y, deltaT);
		return output;
	}
	
	///////////////////////////
	//Deprecated stuff below//
	/////////////////////////
	private const int						LIGHT_SHOCK_HIT = 33; //!Deprecated
	private const int						MID_SHOCK_HIT = 67; //!Deprecated
	private const int						HEAVY_SHOCK_HIT = 100; //!Deprecated
	protected float 						m_ShockValueThreshold; //!Deprecated
	private	float 							m_LerpRes; //!Deprecated
	
	//!Deprecated
	private void ShockHitEffect(float compareBase)
	{
		float shockDifference = compareBase - m_Player.m_CurrentShock;
		if (shockDifference >= UPDATE_THRESHOLD)
		{
			if (m_CumulatedShock < 25)
				m_Player.SpawnShockEffect(MiscGameplayFunctions.Normalize(LIGHT_SHOCK_HIT, 100)); 
			else if (m_CumulatedShock < 50)
				m_Player.SpawnShockEffect(MiscGameplayFunctions.Normalize(MID_SHOCK_HIT, 100));
			else
				m_Player.SpawnShockEffect(MiscGameplayFunctions.Normalize(HEAVY_SHOCK_HIT, 100));
		}
	}
};
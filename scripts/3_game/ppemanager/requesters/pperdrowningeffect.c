class PPERequester_Drowning extends PPERequester_GameplayBase
{
	private float m_EffectPhase = 0;
	private float m_Magnitude = 0;
	private float m_MinMagnitude = 0.3;
	private float m_MaxMagnitude = 0.3;												// Actual Maximum is Min+Max
	private float m_Frequency = 5;
	//private float m_Health01;
	private float m_Stamina01;
	
	override protected void OnStart(Param par = null)
	{
		super.OnStart(par);
		
		m_EffectPhase = 0;
		m_Magnitude = 0;
	}
	
	override protected void OnUpdate(float delta)
	{			
		super.OnUpdate(delta);
		
		if (!m_IsRunning)
			return;
		
		m_EffectPhase += delta * m_Frequency / (m_Stamina01 /*+ m_Health01*/ + 1);	// Start with lower frequency and increase it as it gets closer to danger
		
		float currentVal = (Math.Sin(m_EffectPhase) +1)/2; 							// Makes current val oscillate between 0 and 1 (normlization)
		currentVal = Easing.EaseInExpo(currentVal);									// Ease it to tweak the effect
		currentVal *= currentVal * m_MaxMagnitude;									// Scale the normalized value to make it proportional to the Max Magnitude
		currentVal += m_MinMagnitude;
		m_Magnitude = Math.Lerp(m_Magnitude, currentVal, delta * m_Frequency);		// Learp to smooth the changes
		
		SetTargetValueFloat(PostProcessEffectType.Glow,PPEGlow.PARAM_VIGNETTE,true, m_Magnitude,PPEGodRays.L_0_GLASSES,PPOperators.ADD);
		SetTargetValueColor(PostProcessEffectType.Glow,PPEGlow.PARAM_VIGNETTECOLOR,{0.0,0.025,0.04, 0.0},PPEGlow.L_23_GLASSES,PPOperators.ADD);
	}
	
	/*
	void SetHealth01(float health01)
	{
		m_Health01 = health01;
	}*/
	
	void SetStamina01(float stamina01)
	{
		m_Stamina01 = stamina01;
	}
}
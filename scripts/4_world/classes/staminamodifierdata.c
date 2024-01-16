//supplemental data struct for 'StaminaModifier' class
class SMDataBase
{
	bool m_InUse = false;
	int m_Type;
	
	void SMDataBase()
	{
		Init();
	}
	
	//! Used to set values for the type
	void Init();
	
	int GetType() { return m_Type; }
	
	bool IsInUse() { return m_InUse; }
	void SetInUse(bool val) { m_InUse = val; }
}

class SMDataExponential : SMDataBase
{
	float m_BaseValue;
	float m_Exponent;
	float m_Multiplier;
	float m_Cooldown;
	float m_StartTimeAdjustment;
	float m_Duration;
	
	override void Init()
	{
		m_BaseValue = GameConstants.STAMINA_DRAIN_HOLD_BREATH_START;
		m_Exponent = GameConstants.STAMINA_DRAIN_HOLD_BREATH_EXPONENT;
		m_Multiplier = CfgGameplayHandler.GetHoldBreathStaminaModifier();
		m_Cooldown = GameConstants.STAMINA_REGEN_COOLDOWN_DEPLETION;
		m_StartTimeAdjustment = PlayerSwayConstants.SWAY_TIME_IN + PlayerSwayConstants.SWAY_TIME_STABLE;
		m_Duration = GameConstants.STAMINA_DRAIN_HOLD_BREATH_DURATION;
	}
}

class SMDataHoldBreath : SMDataExponential
{
	
}
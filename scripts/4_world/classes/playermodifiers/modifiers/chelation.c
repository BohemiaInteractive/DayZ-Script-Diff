class ChelationMdfr : ModifierBase
{
	const int CHELATION_LIFETIME 	= 300;
	const float CHELATION_STRENGTH 	= 1;
	
	private float m_RegenTime;
	
	override void Init()
	{
		m_TrackActivatedTime 	= true;
		m_AnalyticsStatsEnabled = true;
		m_IsPersistent 			= true;
		m_ID 					= eModifiers.MDF_CHELATION;
		m_TickIntervalInactive 	= DEFAULT_TICK_TIME_INACTIVE;
		m_TickIntervalActive 	= 3;

		m_RegenTime = CHELATION_LIFETIME;
		
		DisableActivateCheck();
	}

	override bool ActivateCondition(PlayerBase player)
	{
		return false;
	}
	
	override void OnReconnect(PlayerBase player)
	{
		OnActivate(player);
	}
	
	override string GetDebugText()
	{
		return (CHELATION_LIFETIME - GetAttachedTime()).ToString();
	}

	override string GetDebugTextSimple()
	{
	return (CHELATION_LIFETIME - GetAttachedTime()).ToString();
	}
	
	override void OnActivate(PlayerBase player)
	{
		player.IncreaseHealingsCount();
		player.AddMedicalDrugsInUse(EMedicalDrugsType.CHELATION);
	}
	
	override void OnDeactivate(PlayerBase player)
	{
		player.DecreaseHealingsCount();
		player.RemoveMedicalDrugsInUse(EMedicalDrugsType.CHELATION);
	}
	
	override bool DeactivateCondition(PlayerBase player)
	{
		float attachedTime = GetAttachedTime();
		
		if (attachedTime >= m_RegenTime)
			return true;

		return false;
	}

	override void OnTick(PlayerBase player, float deltaT)
	{
		player.m_AgentPool.DrugsAttack(EMedicalDrugsType.CHELATION, CHELATION_STRENGTH * deltaT);
	}
}

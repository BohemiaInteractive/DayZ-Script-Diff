class AntibioticsMdfr : ModifierBase
{
	const int ANTIBIOTICS_LIFETIME 		= 300;
	const float ANTIBIOTICS_STRENGTH 	= 1;

	float m_RegenTime;
	
	override void Init()
	{
		m_TrackActivatedTime 	= true;
		m_IsPersistent 			= true;
		m_ID 					= eModifiers.MDF_ANTIBIOTICS;
		m_TickIntervalInactive 	= DEFAULT_TICK_TIME_INACTIVE;
		m_TickIntervalActive 	= 3;

		m_RegenTime = ANTIBIOTICS_LIFETIME;
		
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
		return (ANTIBIOTICS_LIFETIME - GetAttachedTime()).ToString();
	}

	override string GetDebugTextSimple()
	{
		return (ANTIBIOTICS_LIFETIME - GetAttachedTime()).ToString();
	}
	
	override void OnActivate(PlayerBase player)
	{
		player.IncreaseHealingsCount();
		player.AddMedicalDrugsInUse(EMedicalDrugsType.ANTIBIOTICS);
	}
	
	override void OnDeactivate(PlayerBase player)
	{
		player.DecreaseHealingsCount();
		player.RemoveMedicalDrugsInUse(EMedicalDrugsType.ANTIBIOTICS);
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
		player.m_AgentPool.DrugsAttack(EMedicalDrugsType.ANTIBIOTICS, ANTIBIOTICS_STRENGTH * deltaT);
	}
}

class AntibioticsMdfr: ModifierBase
{
	float m_RegenTime;
	const int ANTIBIOTICS_LIFETIME = 300;
	const float ANTIBIOTICS_STRENGTH = 1;
	
	override void Init()
	{
		m_TrackActivatedTime = true;
		m_IsPersistent = true;
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
		player.IncreaseAntibioticsCount();
		/*
		if ( player.GetNotifiersManager() )
			player.GetNotifiersManager().ActivateByType(eNotifiers.NTF_PILLS);
		*/
	}
	
	override void OnDeactivate(PlayerBase player)
	{
		player.DecreaseHealingsCount();
		player.DecreaseAntibioticsCount();
		/*
		if ( player.GetNotifiersManager() )
			player.GetNotifiersManager().DeactivateByType(eNotifiers.NTF_PILLS);
		*/
	}
	
	override bool DeactivateCondition(PlayerBase player)
	{
		float attached_time = GetAttachedTime();
		
		if ( attached_time >= m_RegenTime )
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	override void OnTick(PlayerBase player, float deltaT)
	{
		player.AntibioticsAttack( ANTIBIOTICS_STRENGTH * deltaT );
	}
};
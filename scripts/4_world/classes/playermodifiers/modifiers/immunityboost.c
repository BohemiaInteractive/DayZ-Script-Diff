class ImmunityBoost: ModifierBase
{
	float 			m_LastWaterLevel;
	ref HumanMovementState		m_MovementState	= new HumanMovementState();
	float m_RegenTime;
	
	override void Init()
	{
		m_TrackActivatedTime = true;
		m_IsPersistent = true;
		m_ID 					= eModifiers.MDF_IMMUNITYBOOST;
		m_TickIntervalInactive 	= DEFAULT_TICK_TIME_INACTIVE;
		m_TickIntervalActive 	= 1;
		m_RegenTime = PlayerConstants.VITAMINS_LIFETIME_SECS;
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
		return (m_RegenTime - GetAttachedTime()).ToString();
	}
	
	override void OnActivate(PlayerBase player)
	{
		player.SetImmunityBoosted(true);
		player.IncreaseHealingsCount();
		/*
		if( player.GetNotifiersManager() )
			player.GetNotifiersManager().ActivateByType(eNotifiers.NTF_PILLS);
		*/
	}
	
	override void OnDeactivate(PlayerBase player)
	{
		player.SetImmunityBoosted(false);
		player.DecreaseHealingsCount();
		/*
		if( player.GetNotifiersManager() )
			player.GetNotifiersManager().DeactivateByType(eNotifiers.NTF_PILLS);
		*/
	}
	
	override bool DeactivateCondition(PlayerBase player)
	{
		float attached_time = GetAttachedTime();
		
		if( attached_time >= m_RegenTime )
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
		
	}
};
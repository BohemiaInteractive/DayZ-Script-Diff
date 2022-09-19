class SalineMdfr: ModifierBase
{
	float m_RegenTime;
	override void Init()
	{
		m_TrackActivatedTime = true;
		m_IsPersistent = true;
		m_ID 					= eModifiers.MDF_SALINE;
		m_TickIntervalInactive 	= DEFAULT_TICK_TIME_INACTIVE;
		m_TickIntervalActive 	= DEFAULT_TICK_TIME_ACTIVE;
		m_RegenTime = CalculateRegenTime();
		DisableActivateCheck();
	}

	override bool ActivateCondition(PlayerBase player)
	{
		return false;
	}

	override bool DeactivateCondition(PlayerBase player)
	{
		float attached_time = GetAttachedTime();

		if ( attached_time > m_RegenTime )
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	override void OnReconnect(PlayerBase player)
	{
		OnActivate(player);
	}
	
	override void OnActivate(PlayerBase player)
	{
		player.IncreaseHealingsCount();
		/*
		if( player.GetNotifiersManager() )
			player.GetNotifiersManager().ActivateByType(eNotifiers.NTF_PILLS);
		*/
	}
	
	override void OnDeactivate(PlayerBase player)
	{
		player.DecreaseHealingsCount();
		/*
		if( player.GetNotifiersManager() )
			player.GetNotifiersManager().DeactivateByType(eNotifiers.NTF_PILLS);
		*/
	}

	override void OnTick(PlayerBase player, float deltaT)
	{
		player.AddHealth("", "Blood", PlayerConstants.SALINE_BLOOD_REGEN_PER_SEC * deltaT);
		player.GetStatWater().Add( PlayerConstants.SALINE_WATER_REGEN_PER_SEC * deltaT );
	}
	
	float CalculateRegenTime()
	{
		float regen_time = PlayerConstants.SALINE_LIQUID_AMOUNT / PlayerConstants.SALINE_BLOOD_REGEN_PER_SEC;
		return regen_time;
	}
};
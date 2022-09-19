class PainKillersMdfr: ModifierBase
{
	const int LIFETIME = 240;
	const int ACTIVATION_DELAY = 15;
	override void Init()
	{
		m_TrackActivatedTime = true;
		m_IsPersistent = true;
		m_ID 					= eModifiers.MDF_PAINKILLERS;
		m_TickIntervalInactive 	= DEFAULT_TICK_TIME_INACTIVE;
		m_TickIntervalActive 	= 1;
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
		return (LIFETIME - GetAttachedTime()).ToString();
	}

	
	override void OnActivate(PlayerBase player)
	{
		if (player.GetBrokenLegs() != eBrokenLegs.NO_BROKEN_LEGS)
			player.m_ShockHandler.SetMultiplier(0.5);//was 0.75 //Switch the shock multiplier NEED A CONST
		player.IncreaseHealingsCount();
		/*
		if( player.GetNotifiersManager() ) 
			player.GetNotifiersManager().ActivateByType(eNotifiers.NTF_PILLS);
		*/
		m_Player.m_InjuryHandler.m_ForceInjuryAnimMask = m_Player.m_InjuryHandler.m_ForceInjuryAnimMask | eInjuryOverrides.PAIN_KILLERS_LVL0;
	}
	
	override void OnDeactivate(PlayerBase player)
	{
		if (player.GetBrokenLegs() != eBrokenLegs.NO_BROKEN_LEGS)
			player.m_ShockHandler.SetMultiplier(1); //Reset the shock multiplier when modifier stops
		player.DecreaseHealingsCount();
		/*
		if( player.GetNotifiersManager() ) 
			player.GetNotifiersManager().DeactivateByType(eNotifiers.NTF_PILLS);
		*/
		m_Player.m_InjuryHandler.m_ForceInjuryAnimMask = m_Player.m_InjuryHandler.m_ForceInjuryAnimMask & ~eInjuryOverrides.PAIN_KILLERS_LVL0;
		m_Player.m_InjuryHandler.m_ForceInjuryAnimMask = m_Player.m_InjuryHandler.m_ForceInjuryAnimMask & ~eInjuryOverrides.PAIN_KILLERS_LVL1;
	}
	
	override bool DeactivateCondition(PlayerBase player)
	{
		float attached_time = GetAttachedTime();
		
		if( attached_time >= LIFETIME )
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
		if(GetAttachedTime() > ACTIVATION_DELAY)
		{
			m_Player.m_InjuryHandler.m_ForceInjuryAnimMask = m_Player.m_InjuryHandler.m_ForceInjuryAnimMask | eInjuryOverrides.PAIN_KILLERS_LVL1;
		}
	}
};
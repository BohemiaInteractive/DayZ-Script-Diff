class CharcoalMdfr: ModifierBase
{
	float m_Killrate;
	const int CHARCOAL_LIFETIME = 300;
	const int CHARCOAL_EFFECT_TIME = 100;
	
	override void Init()
	{
		m_TrackActivatedTime = true;
		m_IsPersistent = true;
		m_ID 					= eModifiers.MDF_CHARCOAL;
		m_TickIntervalInactive 	= DEFAULT_TICK_TIME_INACTIVE;
		m_TickIntervalActive 	= 3;
		m_Killrate = 2.85;	// # of killed agents per sec
		DisableActivateCheck();
	}

	override bool ActivateCondition(PlayerBase player)
	{
		return false;
	}
	
	override void OnReconnect(PlayerBase player)
	{
		OnActivate( player );
	}
	
	/*
	override string GetDebugText()
	{
		
		string text;
		float time_till_effective = CHARCOAL_LIFETIME - CHARCOAL_EFFECT_TIME  - GetAttachedTime();
		
		text = "time till effective: " + time_till_effective.ToString() + "\n";
		
		if(time_till_effective > 0)
			text += "killing agents:" + "NO";
		else
			text += "killing agents:" + "YES";
		
		
		return text;
		
	}
		*/

	
	override string GetDebugTextSimple()
	{
		return ( (int)(CHARCOAL_LIFETIME - GetAttachedTime()) ).ToString() + " | " + (((int)(CHARCOAL_LIFETIME - CHARCOAL_EFFECT_TIME  - GetAttachedTime())) * -1).ToString();
	}
	
	override void OnActivate(PlayerBase player)
	{
		player.IncreaseHealingsCount();

	}
	
	override void OnDeactivate(PlayerBase player)
	{
		player.DecreaseHealingsCount();

	}
	
	override bool DeactivateCondition(PlayerBase player)
	{
		float attached_time = GetAttachedTime();
		
		if ( attached_time >= CHARCOAL_LIFETIME )
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
		if ( GetAttachedTime() > ( CHARCOAL_LIFETIME - CHARCOAL_EFFECT_TIME ) )
		{		
			player.m_AgentPool.AddAgent(eAgents.SALMONELLA, -m_Killrate * deltaT);
		}
	}
};
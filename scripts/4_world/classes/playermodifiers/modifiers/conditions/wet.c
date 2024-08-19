class WetMdfr: ModifierBase
{
	override void Init()
	{
		m_TrackActivatedTime 	= false;
		m_ID 					= eModifiers.MDF_WETNESS;
		m_TickIntervalInactive 	= DEFAULT_TICK_TIME_INACTIVE;
		m_TickIntervalActive 	= DEFAULT_TICK_TIME_ACTIVE;
	}
	override bool ActivateCondition(PlayerBase player)
	{
		if (player.GetStatWet().Get() == player.GetStatWet().GetMax())
			return true;
		
		return false;
	}
	
	override bool DeactivateCondition(PlayerBase player)
	{
		if (player.GetStatWet().Get() == player.GetStatWet().GetMin())
			return true;
		
		return false;
	}


	override void OnActivate(PlayerBase player)
	{
		if( player.m_NotifiersManager ) player.m_NotifiersManager.ActivateByType(eNotifiers.NTF_WETNESS);
	}

	override void OnReconnect(PlayerBase player)
	{
		this.OnActivate(player);
	}


	override void OnDeactivate(PlayerBase player)
	{
		if( player.m_NotifiersManager ) player.m_NotifiersManager.DeactivateByType(eNotifiers.NTF_WETNESS);
	}
	
	override string GetDebugText()
	{
		string wetness = m_Player.GetStatWet().Get().ToString();
		string wetness_min = m_Player.GetStatWet().GetMin().ToString();
		return "wetness:"+wetness+" | " +"wetness min:"+wetness_min;
	}
	
	override string GetDebugTextSimple()
	{
		return "";
	}
};
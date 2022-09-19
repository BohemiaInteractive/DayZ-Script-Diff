class ImmuneSystemMdfr: ModifierBase
{
	bool m_HasDisease;
	bool m_HasHealings;
	override void Init()
	{
		m_TrackActivatedTime = false;
		m_ID 					= eModifiers.MDF_IMMUNE_SYSTEM;
		m_TickIntervalInactive 	= DEFAULT_TICK_TIME_INACTIVE;
		m_TickIntervalActive 	= DEFAULT_TICK_TIME_ACTIVE;
		
		DisableDeactivateCheck();
	}
	
	override bool ActivateCondition(PlayerBase player)
	{
		return true;
	}

	override void OnActivate(PlayerBase player)
	{
	}
	
	override void OnReconnect(PlayerBase player)
	{

	}

	override bool DeactivateCondition(PlayerBase player)
	{
		return false;
	}

	override void OnTick(PlayerBase player, float deltaT)
	{
//		Debug.Log("ticking immune system", "agent");
		float result 	= player.GetImmunity() * deltaT;
		player.ImmuneSystemTick(result, deltaT);
//		Debug.Log("result: "+result.ToString(), "agent");
		
		if( m_HasDisease != player.HasDisease() )
		{
			if(player.HasDisease())
			{
				if( player.GetNotifiersManager() ) player.GetNotifiersManager().ActivateByType(eNotifiers.NTF_SICK);
			}
			else
			{
				if( player.GetNotifiersManager() ) player.GetNotifiersManager().DeactivateByType(eNotifiers.NTF_SICK);
			}
			m_HasDisease = player.HasDisease();
		}	

			
		if( m_HasHealings != player.HasHealings() )
		{
		
			if( player.HasHealings() )
			{
				if( player.GetNotifiersManager() )
					player.GetNotifiersManager().ActivateByType(eNotifiers.NTF_PILLS);
			}
			else
			{
				if( player.GetNotifiersManager() )
					player.GetNotifiersManager().DeactivateByType(eNotifiers.NTF_PILLS);
			}
			m_HasHealings = player.HasHealings();
		}
		
	}
	
};

class CommonColdMdfr: ModifierBase
{
	const int AGENT_THRESHOLD_ACTIVATE = 200;
	const int AGENT_THRESHOLD_DEACTIVATE = 100;
	override void Init()
	{
		m_TrackActivatedTime	= false;
		m_ID 					= eModifiers.MDF_COMMON_COLD;
		m_TickIntervalInactive 	= DEFAULT_TICK_TIME_INACTIVE;
		m_TickIntervalActive 	= DEFAULT_TICK_TIME_ACTIVE;
	}
	
	override string GetDebugText()
	{
		return ("Activate threshold: "+AGENT_THRESHOLD_ACTIVATE + "| " +"Deativate threshold: "+AGENT_THRESHOLD_DEACTIVATE);
	}
	
	override bool ActivateCondition(PlayerBase player)
	{
		if(player.GetSingleAgentCount(eAgents.INFLUENZA) >= AGENT_THRESHOLD_ACTIVATE) 
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	override protected void OnActivate(PlayerBase player)
	{
		//if( player.m_NotifiersManager ) player.m_NotifiersManager.ActivateByType(eNotifiers.NTF_SICK);
		player.IncreaseDiseaseCount();
	}
	
	override protected void OnDeactivate(PlayerBase player)
	{

		player.DecreaseDiseaseCount();
	}


	override protected bool DeactivateCondition(PlayerBase player)
	{
		return (player.GetSingleAgentCount(eAgents.INFLUENZA) <= AGENT_THRESHOLD_DEACTIVATE);
	}

	override protected void OnTick(PlayerBase player, float deltaT)
	{
		float chance_of_sneeze = player.GetSingleAgentCountNormalized(eAgents.INFLUENZA);
		
		if( Math.RandomFloat01() < chance_of_sneeze / Math.RandomInt(15,20) )
		{
			player.GetSymptomManager().QueueUpPrimarySymptom(SymptomIDs.SYMPTOM_SNEEZE);
		}
	}
};
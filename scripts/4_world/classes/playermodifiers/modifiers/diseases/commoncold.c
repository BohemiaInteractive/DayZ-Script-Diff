class CommonColdMdfr : ModifierBase
{
	const int AGENT_THRESHOLD_ACTIVATE 		= 200;
	const int AGENT_THRESHOLD_DEACTIVATE 	= 0;
	
	private const int SNEEZE_RND_DIVIDER_NORMAL_MIN 	= 15;
	private const int SNEEZE_RND_DIVIDER_NORMAL_MAX 	= 20;
	private const int SNEEZE_RND_DIVIDER_SUPPRESSED_MIN = 30;
	private const int SNEEZE_RND_DIVIDER_SUPPRESSED_MAX = 40;
	
	private const int TEMPORARY_RESISTANCE_TIME = 600;
	
	private ModifiersManager m_ModifiersManager;

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
		if (player.GetSingleAgentCount(eAgents.INFLUENZA) >= AGENT_THRESHOLD_ACTIVATE) 
			return true;

		return false;
	}

	override protected void OnActivate(PlayerBase player)
	{
		player.IncreaseDiseaseCount();

		m_ModifiersManager = player.GetModifiersManager();
		player.GetSymptomManager().QueueUpPrimarySymptom(SymptomIDs.SYMPTOM_SNEEZE);
	}
	
	override protected void OnDeactivate(PlayerBase player)
	{
		player.DecreaseDiseaseCount();

		player.SetTemporaryResistanceToAgent(eAgents.INFLUENZA, TEMPORARY_RESISTANCE_TIME);
	}

	override protected bool DeactivateCondition(PlayerBase player)
	{
		return (player.GetSingleAgentCount(eAgents.INFLUENZA) <= AGENT_THRESHOLD_DEACTIVATE);
	}

	override protected void OnTick(PlayerBase player, float deltaT)
	{
		float chanceOfSneeze = player.GetSingleAgentCountNormalized(eAgents.INFLUENZA);
		
		float randomDivider = Math.RandomInt(SNEEZE_RND_DIVIDER_NORMAL_MIN, SNEEZE_RND_DIVIDER_NORMAL_MAX);
		if (m_ModifiersManager.IsModifierActive(eModifiers.MDF_PAINKILLERS) || m_ModifiersManager.IsModifierActive(eModifiers.MDF_MORPHINE))
			randomDivider = Math.RandomInt(SNEEZE_RND_DIVIDER_SUPPRESSED_MIN, SNEEZE_RND_DIVIDER_SUPPRESSED_MAX);
		
		if (Math.RandomFloat01() < chanceOfSneeze / randomDivider)
		{
			player.GetSymptomManager().QueueUpPrimarySymptom(SymptomIDs.SYMPTOM_SNEEZE);
		}
	}
}

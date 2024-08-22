class PneumoniaMdfr : ModifierBase
{
	const int AGENT_THRESHOLD_ACTIVATE = 1000;
	const int AGENT_THRESHOLD_DEACTIVATE = 700;
	
	const float STAMINA_DEPLETION_MULTIPLIER = 1.3;
	const float STAMINA_RECOVERY_MULTIPLIER = 0.5;
	
	private const int TEMPORARY_RESISTANCE_TIME = 600;
	
	override void Init()
	{
		m_TrackActivatedTime	= false;
		m_ID 					= eModifiers.MDF_PNEUMONIA;
		m_TickIntervalInactive 	= DEFAULT_TICK_TIME_INACTIVE;
		m_TickIntervalActive 	= DEFAULT_TICK_TIME_ACTIVE;
	}
	
	override string GetDebugText()
	{
		return ("Activate threshold: "+AGENT_THRESHOLD_ACTIVATE + "| " +"Deativate threshold: "+AGENT_THRESHOLD_DEACTIVATE);
	}
	
	override protected bool ActivateCondition(PlayerBase player)
	{
		if (player.GetSingleAgentCount(eAgents.INFLUENZA) >= AGENT_THRESHOLD_ACTIVATE) 
			return true;

		return false;
	}

	override protected void OnActivate(PlayerBase player)
	{
		player.IncreaseDiseaseCount();

		player.GetStaminaHandler().ActivateRecoveryModifier(EStaminaMultiplierTypes.DISEASE_PNEUMONIA);
		player.GetStaminaHandler().ActivateDepletionModifier(EStaminaMultiplierTypes.DISEASE_PNEUMONIA);
	}

	override protected void OnDeactivate(PlayerBase player)
	{
		player.DecreaseDiseaseCount();

		player.GetStaminaHandler().DeactivateRecoveryModifier(EStaminaMultiplierTypes.DISEASE_PNEUMONIA);
		player.GetStaminaHandler().DeactivateDepletionModifier(EStaminaMultiplierTypes.DISEASE_PNEUMONIA);

		player.SetTemporaryResistanceToAgent(eAgents.INFLUENZA, TEMPORARY_RESISTANCE_TIME);
	}

	override protected bool DeactivateCondition(PlayerBase player)
	{
		return (player.GetSingleAgentCount(eAgents.INFLUENZA) <= AGENT_THRESHOLD_DEACTIVATE);
	}

	override protected void OnTick(PlayerBase player, float deltaT)
	{
		float heavyBreathChance = player.GetSingleAgentCountNormalized(eAgents.INFLUENZA);
		
		if ( Math.RandomFloat01() < heavyBreathChance / Math.RandomInt(5,30) )
		{
			player.RequestSoundEventEx(EPlayerSoundEventID.SYMPTOM_GASP, true, EPlayerSoundEventParam.HIGHEST_PRIORITY);
		}
	}
};
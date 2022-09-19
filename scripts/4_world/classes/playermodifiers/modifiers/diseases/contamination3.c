class ContaminationStage3Mdfr: ModifierBase
{
	static const int AGENT_THRESHOLD_ACTIVATE = 400;
	static const int AGENT_THRESHOLD_DEACTIVATE = AGENT_THRESHOLD_ACTIVATE - 1;
	static const int AGENT_THRESHOLD_FEVER = 250;
	static const float DAMAGE_PER_SEC = -3;

	override void Init()
	{
		m_ID 					= eModifiers.MDF_CONTAMINATION3;
		m_TickIntervalInactive 	= DEFAULT_TICK_TIME_INACTIVE;
		m_TickIntervalActive 	= DEFAULT_TICK_TIME_ACTIVE_SHORT;
		m_SyncID				= eModifierSyncIDs.MODIFIER_SYNC_CONTAMINATION2;//using the same sync as stage 2 for now
		m_TrackActivatedTime	= true;
	}
	
	override string GetDebugText()
	{
		return ("Activate threshold: "+AGENT_THRESHOLD_ACTIVATE + "| " +"Deativate threshold: "+AGENT_THRESHOLD_DEACTIVATE);
	}
	
	override protected bool ActivateCondition(PlayerBase player)
	{
		return ( player.GetSingleAgentCount(eAgents.CHEMICAL_POISON) >= AGENT_THRESHOLD_ACTIVATE );

	}

	override protected void OnActivate(PlayerBase player)
	{
		player.IncreaseDiseaseCount();
		player.GetSymptomManager().QueueUpPrimarySymptom(SymptomIDs.SYMPTOM_VOMIT);
	}

	override protected void OnDeactivate(PlayerBase player)
	{
		player.DecreaseDiseaseCount();
	}

	override protected bool DeactivateCondition(PlayerBase player)
	{
		return !ActivateCondition(player);
	}

	override protected void OnTick(PlayerBase player, float deltaT)
	{
		if(GetAttachedTime() > 4 && (!player.GetSymptomManager().GetCurrentPrimaryActiveSymptom() || (player.GetSymptomManager().GetCurrentPrimaryActiveSymptom() &&  player.GetSymptomManager().GetCurrentPrimaryActiveSymptom().GetType() != SymptomIDs.SYMPTOM_VOMIT)) )
		{
			player.AddHealth("","Shock", -100);
		}
		
		if (player.IsUnconscious())
		{
			player.AddHealth("","",DAMAGE_PER_SEC * deltaT);
		}

	}
};
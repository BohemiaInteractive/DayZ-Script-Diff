class VomitStuffedMdfr : ModifierBase
{
	override void Init()
	{
		m_TrackActivatedTime 	= false;
		m_ID 					= eModifiers.MDF_VOMITSTUFFED;
		m_TickIntervalInactive 	= DEFAULT_TICK_TIME_INACTIVE;
		m_TickIntervalActive 	= DEFAULT_TICK_TIME_ACTIVE;
	}
	
	override bool ActivateCondition(PlayerBase player)
	{
		float stomach = m_Player.m_PlayerStomach.GetStomachVolume();
		return stomach >= PlayerConstants.VOMIT_THRESHOLD;
	}

	override bool DeactivateCondition(PlayerBase player)
	{
		return !ActivateCondition(player);
	}

	override void OnActivate(PlayerBase player)
	{
		SymptomBase symptom = player.GetSymptomManager().QueueUpPrimarySymptom(SymptomIDs.SYMPTOM_VOMIT);
		if (symptom)
		{
			CachedObjectsParams.PARAM1_FLOAT.param1 = 50.0;
			symptom.SetParam(CachedObjectsParams.PARAM1_FLOAT);
		}
	}
}

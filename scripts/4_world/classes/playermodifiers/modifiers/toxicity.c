class ToxicityMdfr: ModifierBase
{
	private const float	 	TOXICITY_CLEANUP_PER_SEC = 1;
	private const float 	VOMIT_THRESHOLD = 70;
	
	override void Init()
	{
		m_TrackActivatedTime = false;
		m_ID 					= eModifiers.MDF_TOXICITY;
		m_TickIntervalInactive 	= DEFAULT_TICK_TIME_INACTIVE;
		m_TickIntervalActive 	= DEFAULT_TICK_TIME_ACTIVE;
		DisableDeactivateCheck();
	}	

	override bool ActivateCondition(PlayerBase player)
	{
		return true;
	}

	override bool DeactivateCondition(PlayerBase player)
	{
		return false;
	}

	override void OnTick(PlayerBase player, float deltaT)
	{	
		player.GetStatToxicity().Add( -TOXICITY_CLEANUP_PER_SEC * deltaT );
		if( player.GetStatToxicity().Get() > VOMIT_THRESHOLD )
		{
			SymptomBase symptom = player.GetSymptomManager().QueueUpPrimarySymptom(SymptomIDs.SYMPTOM_VOMIT);
		
			if( symptom )
			{ 
				symptom.SetDuration(Math.RandomIntInclusive(4,8));
			}
		}
	}
	
	override void OnReconnect(PlayerBase player)
	{

	}
	
	override void OnActivate(PlayerBase player)
	{
		
	}
};
class TestDiseaseMdfr: ModifierBase
{
	float m_Interval;
	ref Param1<float> m_SomeValue;

	override void Init()
	{
		m_TrackActivatedTime			= false;
		m_ID 							= 9999;
		m_TickIntervalInactive 	= DEFAULT_TICK_TIME_INACTIVE;
		m_TickIntervalActive 	= 5;
		m_SomeValue		= new Param1<float>(0);
		MakeParamObjectPersistent(m_SomeValue);
		
		DisableDeactivateCheck();
		DisableActivateCheck();
	}
	
	override bool ActivateCondition(PlayerBase player)
	{
		return false;
	}

	override void OnActivate(PlayerBase player)
	{
		//player.GetBleedingManagerServer().AttemptAddBleedingSource(Math.RandomInt(0, 100));
	}
	
	override void OnReconnect(PlayerBase player)
	{
		//OnActivate(player);
		//player.GetSymptomManager().QueueUpSecondaryState( SymptomIDs.SYMPTOM_BLINDNESS );
	}

	override void OnDeactivate(PlayerBase player)
	{
	}

	override bool DeactivateCondition(PlayerBase player)
	{
		return false;
	}

	override void OnTick(PlayerBase player, float deltaT)
	{
		player.GetSymptomManager().QueueUpPrimarySymptom(SymptomIDs.SYMPTOM_LAUGHTER);
	}
};
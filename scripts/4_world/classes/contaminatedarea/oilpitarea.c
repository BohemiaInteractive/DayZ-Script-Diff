enum EOilPitState
{
	INACTIVE	= 0,
	BURNING		= 1
}

class OilPitArea : EffectArea
{
	// used for start randomization
	protected const int 	PRE_BURNING_DURATION_MIN 	= 3; 	// delay before burning activates - min (sec)
	protected const int 	PRE_BURNING_DURATION_MAX 	= 10; 	// delay before burning activates - max (sec)

	protected int 			m_RandomizedBurningStart;	// randomized burning start
	protected OilPitTrigger m_OilPitTrigger;

	override void DeferredInit()
	{
		super.DeferredInit();

		InitZone();
	}

	override void EEDelete( EntityAI parent )
	{
		g_Game.GetCallQueue(CALL_CATEGORY_SYSTEM).Remove(StartBurning);

		if (m_OilPitTrigger)
			m_OilPitTrigger.StopEffects();

		super.EEDelete(parent);
	}

	override void InitZoneServer()
	{
		super.InitZoneServer();

		if (m_TriggerType != "")
		{
			CreateTrigger(m_PositionTrigger, m_Radius);
			m_OilPitTrigger = OilPitTrigger.Cast(m_Trigger);
			
			if (m_OilPitTrigger)
			{
				RandomizeIntervals();
		
				g_Game.GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(StartBurning, m_RandomizedBurningStart * 1000, false);
			}
		}

	}
	
	protected void RandomizeIntervals()
	{
		m_RandomizedBurningStart = Math.RandomInt(PRE_BURNING_DURATION_MIN, PRE_BURNING_DURATION_MAX);
	}
	
	protected void StartBurning()
	{
		if (m_OilPitTrigger)
			m_OilPitTrigger.AddState(EOilPitState.BURNING);
	}
}

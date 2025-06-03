enum EGeyserState
{
	DORMANT 			= 0,
	ERUPTION_SOON 		= 1,	// bubbling
	ERUPTING_PRIMARY	= 2,	// main particle
	ERUPTING_SECONDARY	= 4		// secondary tall particle
}

class GeyserArea : EffectArea
{
	protected const int 	UPDATE_RATE 			= 1000; // ms
	protected const float 	PRE_ERUPTION_DURATION 	= 10; 	// delay before the geysier activates (sec)
	protected const float 	ERUPTION_TALL_DURATION 	= 3; 	// lenght of secondary eruption (sec)
	protected const float 	ERUPTION_TALL_DELAY 	= 3; 	// min delay between secondary eruptions (sec)
	
	protected bool 			m_SecondaryActive;	
	
	protected int 			m_TimeElapsed;			// seconds
	protected int 			m_TimeSecondaryElapsed;	// seconds
	protected float 		m_RandomizedInterval;	// randomized interval to 80% - 120% of the set value
	protected float 		m_RandomizedDuration;	// randomized duration to 80% - 120% of the set value
	protected GeyserTrigger m_GeyserTrigger;
	
	override void DeferredInit()
	{
		super.DeferredInit();
		
		InitZone();		
	}
	
	override void EEDelete( EntityAI parent )
	{		
		if (GetGame().IsClient() && m_GeyserTrigger)
			m_GeyserTrigger.StopEffects();
		
		super.EEDelete( parent );
	}
	
	override void InitZoneServer()
	{
		super.InitZoneServer();
				
		if ( m_TriggerType != "" )
		{
			CreateTrigger(m_PositionTrigger, m_Radius);
			m_GeyserTrigger = GeyserTrigger.Cast(m_Trigger);
		}
		
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(TickState, UPDATE_RATE, true);
		
		RandomizeIntervals();
	}	
		
	void TickState()
	{
		m_TimeElapsed += UPDATE_RATE * 0.001;
	
		if (m_GeyserTrigger.CheckGeyserState(EGeyserState.DORMANT))
		{
			if (m_TimeElapsed > PRE_ERUPTION_DURATION)
			{
				#ifdef ENABLE_LOGGING
				Debug.Log(m_Name + ": ERUPTION_SOON, interval: " + m_RandomizedInterval + " sec");
				#endif
				
				m_GeyserTrigger.AddGeyserState(EGeyserState.ERUPTION_SOON);
				
				m_TimeElapsed = 0;
			}
		}
		else if (m_GeyserTrigger.CheckGeyserState(EGeyserState.ERUPTION_SOON))
		{
			if (m_TimeElapsed > m_RandomizedInterval)
			{
				#ifdef ENABLE_LOGGING
				Debug.Log(m_Name + ": ERUPTING_PRIMARY, interval: " + m_RandomizedDuration + " sec");
				#endif
				
				m_GeyserTrigger.RemoveGeyserState(EGeyserState.ERUPTION_SOON);
				m_GeyserTrigger.AddGeyserState(EGeyserState.ERUPTING_PRIMARY);
				
				KillEntitiesInArea();
				
				m_TimeSecondaryElapsed = -1;
				m_SecondaryActive = false;
				m_TimeElapsed = 0;
			}
		}
		else if (m_GeyserTrigger.CheckGeyserState(EGeyserState.ERUPTING_PRIMARY))
		{
			if (m_TimeElapsed > m_RandomizedDuration)
			{	
				RandomizeIntervals();
				
				#ifdef ENABLE_LOGGING
				Debug.Log(m_Name + ": ERUPTION_SOON, interval: " + m_RandomizedInterval + " sec");
				#endif
				
				m_GeyserTrigger.RemoveGeyserState(EGeyserState.ERUPTING_PRIMARY);
				m_GeyserTrigger.RemoveGeyserState(EGeyserState.ERUPTING_SECONDARY);
				m_GeyserTrigger.AddGeyserState(EGeyserState.ERUPTION_SOON);

				m_TimeElapsed = 0;
			}
			else if (Math.IsInRange(m_TimeElapsed, ERUPTION_TALL_DELAY, m_RandomizedDuration - ERUPTION_TALL_DURATION)) 	// Ensure burst do not overlap with state transitions
			{
				if (!m_SecondaryActive && m_TimeSecondaryElapsed < 0) 
				{
					if (Math.RandomBool())	// 50% chance to start secondary eruption every update
					{
						m_GeyserTrigger.AddGeyserState(EGeyserState.ERUPTING_SECONDARY);
						
						m_TimeSecondaryElapsed = 0;
						m_SecondaryActive = true;
					}
				}
				else if (m_TimeSecondaryElapsed >= 0) 
				{	
					m_TimeSecondaryElapsed += UPDATE_RATE * 0.001;
					
					if (m_SecondaryActive && m_TimeSecondaryElapsed > ERUPTION_TALL_DURATION)
					{
						m_GeyserTrigger.RemoveGeyserState(EGeyserState.ERUPTING_SECONDARY);
						m_SecondaryActive = false;
					}
					else if (m_TimeSecondaryElapsed > (ERUPTION_TALL_DURATION + ERUPTION_TALL_DELAY))
					{
						m_TimeSecondaryElapsed = -1;
					}
				}
			}
		}	
	}
	
	private void RandomizeIntervals()
	{
		m_RandomizedInterval = Math.RandomInt(m_EffectInterval * 0.8, m_EffectInterval * 1.2);
		m_RandomizedDuration = Math.RandomInt(m_EffectDuration * 0.8, m_EffectDuration * 1.2);
	}
	
	void KillEntitiesInArea()
	{
		array<Object> nearestObjects = new array<Object>();
		GetGame().GetObjectsAtPosition(m_Position, m_Radius, nearestObjects, null);
		
		foreach (Object obj : nearestObjects)
		{
			EntityAI entity = EntityAI.Cast(obj);
			if (entity)
				entity.ProcessDirectDamage(DamageType.CUSTOM, this, "", "HeatDamage", "0 0 0", 1000);
		}
	}
}
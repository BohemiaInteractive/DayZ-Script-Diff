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
	protected const float 	PRE_ERUPTION_DURATION 	= 5; 	// length of the pre eruption phase in seconds
	
	protected int 			m_TimeElapsed;			// seconds
	protected float 		m_RandomizedInterval;	// randomized interval to 80% - 120% of the set value
	protected float 		m_RandomizedDuration;	// randomized duration to 80% - 120% of the set value
	protected GeyserTrigger m_GeyserTrigger;
	
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
			CreateTrigger( m_Position, m_Radius );
			m_GeyserTrigger = GeyserTrigger.Cast(m_Trigger);
		}
		
		GetGame().GetCallQueue(CALL_CATEGORY_SYSTEM).CallLater(TickState, UPDATE_RATE, true);
		
		m_RandomizedInterval = Math.RandomInt(m_EffectInterval * 0.8, m_EffectInterval * 1.2);
	}	
		
	void TickState()
	{
		m_TimeElapsed += UPDATE_RATE * 0.001;
				
		if (m_GeyserTrigger.GetGeyserState() == EGeyserState.DORMANT)
		{
			if (m_TimeElapsed > m_RandomizedInterval)
			{
				m_TimeElapsed = 0;
				m_RandomizedDuration = Math.RandomInt(m_EffectDuration * 0.8, m_EffectDuration * 1.2);
				m_GeyserTrigger.AddGeyserState(EGeyserState.ERUPTION_SOON);
			}
		}
		else 
		{
			if (m_GeyserTrigger.GetGeyserState() & EGeyserState.ERUPTION_SOON)
			{
				if (m_TimeElapsed > PRE_ERUPTION_DURATION)
				{
					m_GeyserTrigger.RemoveGeyserState(EGeyserState.ERUPTION_SOON);
					m_GeyserTrigger.AddGeyserState(EGeyserState.ERUPTING_PRIMARY);
					KillEntitiesInArea();
					return;
				}
			}
			
			if (m_GeyserTrigger.GetGeyserState() & EGeyserState.ERUPTING_PRIMARY)
			{
				if (m_TimeElapsed > m_RandomizedDuration)
				{
					m_TimeElapsed = 0;
					m_RandomizedInterval = Math.RandomInt(m_EffectInterval * 0.8, m_EffectInterval * 1.2);
					m_GeyserTrigger.RemoveGeyserState(EGeyserState.ERUPTING_PRIMARY | EGeyserState.ERUPTING_SECONDARY);
					return;
				}
				
				if (m_GeyserTrigger.GetGeyserState() & EGeyserState.ERUPTING_SECONDARY)
				{
					if (Math.RandomBool())	// 50% chance to end secondary eruption every update
						m_GeyserTrigger.RemoveGeyserState(EGeyserState.ERUPTING_SECONDARY);
				}
				else if (Math.RandomBool())	// 50% chance to start secondary eruption every update
					m_GeyserTrigger.AddGeyserState(EGeyserState.ERUPTING_SECONDARY);
			}
		}
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
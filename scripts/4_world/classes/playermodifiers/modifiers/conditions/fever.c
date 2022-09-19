class FeverMdfr: ModifierBase
{
	private float m_Time;
	private float m_NextEvent;
	
	static const float EVENT_INTERVAL_MIN = 12;
	static const float EVENT_INTERVAL_MAX = 18;
	
	
	override void Init()
	{
		m_TrackActivatedTime = false;
		m_ID 					= eModifiers.MDF_FEVER;
		m_TickIntervalInactive 	= DEFAULT_TICK_TIME_INACTIVE;
		m_TickIntervalActive 	= DEFAULT_TICK_TIME_ACTIVE_SHORT;
	}
	override bool ActivateCondition(PlayerBase player)
	{
		return (player.GetModifiersManager().IsModifierActive(eModifiers.MDF_CHOLERA) || player.GetModifiersManager().IsModifierActive(eModifiers.MDF_INFLUENZA) || player.GetModifiersManager().IsModifierActive(eModifiers.MDF_WOUND_INFECTION2));
	}

	override void OnActivate(PlayerBase player)
	{
		if( player.m_NotifiersManager ) 
			player.m_NotifiersManager.ActivateByType(eNotifiers.NTF_FEVERISH);
		
		player.GetSymptomManager().QueueUpSecondarySymptom(SymptomIDs.SYMPTOM_FEVERBLUR);
	}

	override void OnReconnect(PlayerBase player)
	{
		this.OnActivate(player);
	}


	override void OnDeactivate(PlayerBase player)
	{
		if( player.m_NotifiersManager ) 
			player.m_NotifiersManager.DeactivateByType(eNotifiers.NTF_FEVERISH);
		player.GetSymptomManager().RemoveSecondarySymptom(SymptomIDs.SYMPTOM_FEVERBLUR);
	}


	override bool DeactivateCondition(PlayerBase player)
	{
		return !ActivateCondition(player);
	}

	override void OnTick(PlayerBase player, float deltaT)
	{
		float water_loss = deltaT * PlayerConstants.WATER_LOSS_FEVER;
		player.GetStatWater().Add(-water_loss);
		
		m_Time += deltaT;
	
		if ( m_Time >= m_NextEvent )
		{
			m_Time = 0;
			m_NextEvent = Math.RandomFloatInclusive( EVENT_INTERVAL_MIN, EVENT_INTERVAL_MAX );
			
			player.GetSymptomManager().QueueUpPrimarySymptom(SymptomIDs.SYMPTOM_HOT);
		}
	}
};
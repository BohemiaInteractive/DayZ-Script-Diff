// In this file you will find both Static and Dynamic contaminated area triggers
class ContaminatedTrigger extends EffectTrigger
{
	const float DAMAGE_TICK_RATE = 10;//deal damage every n-th second
	
	// ----------------------------------------------
	// 				TRIGGER EVENTS
	// ----------------------------------------------
	
	override void OnEnterServerEvent( TriggerInsider insider )
	{
		super.OnEnterServerEvent( insider );
		if ( insider )
		{
			PlayerBase playerInsider = PlayerBase.Cast( insider.GetObject() );
			
			
			if ( playerInsider )
			{
				playerInsider.GetModifiersManager().ActivateModifier( eModifiers.MDF_AREAEXPOSURE );
			}
			else
			{		
				DayZCreatureAI creature = DayZCreatureAI.Cast( insider.GetObject() );
				
				if(creature)
					creature.IncreaseEffectTriggerCount();
			}
		}
		
	}

	override void OnLeaveServerEvent( TriggerInsider insider )
	{
		super.OnLeaveServerEvent( insider );
		
		if ( insider )
		{
			PlayerBase playerInsider = PlayerBase.Cast( insider.GetObject() );
			
			if ( playerInsider && !m_Manager.IsPlayerInTriggerType( playerInsider, this ) )
			{
				playerInsider.GetModifiersManager().DeactivateModifier( eModifiers.MDF_AREAEXPOSURE );
			}
			else
			{
				DayZCreatureAI creature = DayZCreatureAI.Cast( insider.GetObject() );
				
				if(creature)
					creature.DecreaseEffectTriggerCount();
			}
		}
	}
	
	override void OnEnterClientEvent( TriggerInsider insider )
	{
		super.OnEnterClientEvent( insider );
	}
	
	override void OnLeaveClientEvent( TriggerInsider insider )
	{
		super.OnLeaveClientEvent( insider );
	}
	
	
	override void OnStayStartServerEvent(int nrOfInsiders)
	{
		m_TimeAccuStay += m_DeltaTime;
		if (m_TimeAccuStay > DAMAGE_TICK_RATE)
		{
			m_DealDamageFlag = true;
			//this is where we would normally reset the m_TimeAccuStay, but we need the value as deltaT when dealing damage to the insiders, so we reset it only after the insider update in OnStayFinishServerEvent
		}

	}
	
	override void OnStayFinishServerEvent()
	{
		if(m_DealDamageFlag)//the flag was previously set to true, the insiders have been updated at this point, reset the flag and the timer as well
		{
			m_TimeAccuStay = 0;
			m_DealDamageFlag = false;
		}
	}

	override void OnStayServerEvent(TriggerInsider insider, float deltaTime) 
	{
		if ( m_DealDamageFlag )
		{
			DayZCreatureAI creature = DayZCreatureAI.Cast( insider.GetObject());
			if(creature && creature.m_EffectTriggerCount != 0)
				creature.DecreaseHealth("", "", GameConstants.AI_CONTAMINATION_DMG_PER_SEC * m_TimeAccuStay / creature.m_EffectTriggerCount);// we devide by m_EffectTriggerCount for multiple trigger presence(overlapping triggers)
		}
	}
	
	override string GetAmbientSoundsetName()
	{
		return "ContaminatedArea_SoundSet";
	}

	
}

class ContaminatedTrigger_Dynamic : ContaminatedTrigger
{
	protected int m_AreaState; // Used to get the state from linked area and update effects
	
	void ContaminatedTrigger_Dynamic()
	{
		RegisterNetSyncVariableInt("m_AreaState");
		
		// Register in the trigger effect manager
		m_Manager = TriggerEffectManager.GetInstance();
		m_Manager.RegisterTriggerType( this );
	}
	
	void SetAreaState( int state )
	{
		m_AreaState = state;
		SetSynchDirty();
	}
	
	override void OnEnterClientEvent( TriggerInsider insider )
	{
		if ( insider.GetObject().IsInherited( PlayerBase ) )
		{
			PlayerBase playerInsider = PlayerBase.Cast( insider.GetObject() );
			
			// We will only handle the controlled player, as effects are only relevant to this player instance
			if ( playerInsider.IsControlledPlayer() )
			{
				// If it is first entrance, we fire the following
				if ( !m_Manager.IsPlayerInTriggerType( playerInsider, this ) )
				{
					// We check if we are not in the standard LIVE state
					bool nonDefaultState = m_AreaState > eAreaDecayStage.LIVE;
					if ( nonDefaultState )
					{
						// if not we check the specific state and update local effect values
						int localPartBirthRate;
						if ( m_AreaState == eAreaDecayStage.DECAY_START )
							localPartBirthRate = 20;
						else
							localPartBirthRate = 10;
						
						playerInsider.SetContaminatedEffectEx( true, m_PPERequester, m_AroundPartId, m_TinyPartId, GetAmbientSoundsetName(), nonDefaultState, localPartBirthRate );
					}
					else
						playerInsider.SetContaminatedEffectEx( true, m_PPERequester, m_AroundPartId, m_TinyPartId, GetAmbientSoundsetName() );
				}
				
				// We then handle the update of player trigger state in manager
				m_Manager.OnPlayerEnter( playerInsider, this );
			}
		}
	}
	
	override void OnVariablesSynchronized()
	{
		super.OnVariablesSynchronized();
		
		// Is the controlled player inside when trigger is synchronized
		for ( int i = 0; i < GetInsiders().Count(); i++ )
		{
			PlayerBase playerInsider = PlayerBase.Cast( m_insiders.Get( i ).GetObject() );
			if ( playerInsider )
			{
				if ( playerInsider.IsControlledPlayer() )
				{
					// Update Local particle effects
					bool nonDefaultState = m_AreaState > eAreaDecayStage.LIVE;
					if ( nonDefaultState )
					{
						int localPartBirthRate;
						if ( m_AreaState == eAreaDecayStage.DECAY_START )
							localPartBirthRate = 20;
						else
							localPartBirthRate = 10;
						
						// Update the local effects
						playerInsider.SetContaminatedEffectEx( true, m_PPERequester, m_AroundPartId, m_TinyPartId, GetAmbientSoundsetName(), nonDefaultState, localPartBirthRate );
					}
				}
			}
		}
	}
	
}
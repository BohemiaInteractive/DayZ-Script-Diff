enum eAreaDecayStage
{
	INIT 			= 1, // The dynamic area is initializing
	START 			= 2, // The dynamic area is starting
	LIVE 			= 3, // The dynamic area is live
	DECAY_START 	= 4, // The dynamic area decay has started
	DECAY_END 		= 5, // The dynamic area will soon be deleted
}

class ContaminatedArea_DynamicBase : ContaminatedArea_Base
{
	protected int 			m_DecayState 				= eAreaDecayStage.INIT; // The current state in which the area is
	
	// Constants used for dissapearing events
	const float				DECAY_START_PART_SIZE 		= 32;
	const int				DECAY_START_PART_BIRTH_RATE = 1;
	const float				DECAY_END_PART_SIZE 		= 17;
	const int				DECAY_END_PART_BIRTH_RATE 	= 1;
	const float 			START_DECAY_LIFETIME		= 900;
	const float 			FINISH_DECAY_LIFETIME		= 300;

	void ContaminatedArea_DynamicBase()
	{
		m_Type = eZoneType.DYNAMIC;

		RegisterNetSyncVariableInt("m_DecayState");
	}

	
	float GetRemainingTime()
	{
		return GetLifetime();
	}
	
	float GetStartDecayLifetime()
	{
		return START_DECAY_LIFETIME;
	}
	
	float GetFinishDecayLifetime()
	{
		return FINISH_DECAY_LIFETIME;
	}
	
	// Set the new state of the Area
	void SetDecayState(int newState)
	{
		if (m_DecayState != newState)
		{
			m_DecayState = newState;
		
			// We update the trigger state values as we also want to update player bound effects
			if ( m_Trigger )
				ContaminatedTrigger_Dynamic.Cast( m_Trigger ).SetAreaState( m_DecayState );
			
			SetSynchDirty();
		}
	}

	// We spawn particles and setup trigger
	override void InitZone()
	{		
		SetDecayState(eAreaDecayStage.LIVE);
		
		super.InitZone();
	}
	
	override void InitZoneClient()
	{
		super.InitZoneClient();

		// We spawn VFX on client
		PlaceParticles(m_Position, m_Radius, m_InnerRings, m_InnerSpacing, m_OuterRingToggle, m_OuterSpacing, m_OuterRingOffset, m_ParticleID);		
	}
	
	override void InitZoneServer()
	{
		super.InitZoneServer();
		
		// We create the trigger on server
		if (m_TriggerType != "")
			CreateTrigger(m_PositionTrigger, m_Radius);
	}
	
	override void OnParticleAllocation(ParticleManager pm, array<ParticleSource> particles)
	{
		super.OnParticleAllocation(pm, particles);
		
		if (m_DecayState > eAreaDecayStage.LIVE)
		{
			foreach (ParticleSource p : particles)
			{
				if (m_DecayState == eAreaDecayStage.DECAY_END)
				{
					p.SetParameter(0, EmitorParam.BIRTH_RATE, DECAY_END_PART_BIRTH_RATE);
					p.SetParameter(0, EmitorParam.SIZE, DECAY_END_PART_SIZE);
				}
				else
				{
					p.SetParameter(0, EmitorParam.BIRTH_RATE, DECAY_START_PART_BIRTH_RATE);
					p.SetParameter(0, EmitorParam.SIZE, DECAY_START_PART_SIZE);
				}
			}
		}
	}
	
	override void CreateTrigger(vector pos, int radius)
	{
		super.CreateTrigger(pos, radius);
		
		// This handles the specific case of dynamic triggers as some additionnal parameters are present
		ContaminatedTrigger_Dynamic dynaTrigger = ContaminatedTrigger_Dynamic.Cast(m_Trigger);
		if (dynaTrigger)
		{
			dynaTrigger.SetLocalEffects(m_AroundParticleID, m_TinyParticleID, m_PPERequesterIdx);
			dynaTrigger.SetAreaState(m_DecayState);
		}
	}
	
	override void OnVariablesSynchronized()
	{
		super.OnVariablesSynchronized();
		
		if (!m_ToxicClouds)
			m_ToxicClouds = new array<Particle>();
		
		switch (m_DecayState)
		{
			case eAreaDecayStage.LIVE:
				InitZoneClient();
				break;

			case eAreaDecayStage.DECAY_START:
				// We go through all the particles bound to this area and update relevant parameters
				//Debug.Log("We start decay");
				foreach ( Particle p : m_ToxicClouds )
				{
					p.SetParameter( 0, EmitorParam.BIRTH_RATE, DECAY_START_PART_BIRTH_RATE );
					p.SetParameter( 0, EmitorParam.SIZE, DECAY_START_PART_SIZE );
				}
				
				break;

			case eAreaDecayStage.DECAY_END:
				// We go through all the particles bound to this area and update relevant parameters
				//Debug.Log("We finish decay");
				foreach ( Particle prt : m_ToxicClouds )
				{
					prt.SetParameter( 0, EmitorParam.BIRTH_RATE, DECAY_END_PART_BIRTH_RATE );
					prt.SetParameter( 0, EmitorParam.SIZE, DECAY_END_PART_SIZE );
				}
				
				break;

			default:
				break;
		}
	}
}
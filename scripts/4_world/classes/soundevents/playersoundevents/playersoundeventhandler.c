enum EPlayerSoundEventID
{
	HOLD_BREATH = 1,
	EXHAUSTED_BREATH,
	RELEASE_BREATH,
	STAMINA_DOWN_LIGHT,
	STAMINA_DOWN_HEAVY,
	STAMINA_UP_LIGHT,
	STAMINA_UP_HEAVY,
	STAMINA_UP_END,
	STAMINA_NORMAL_DUMMY,
	TAKING_DMG_LIGHT,
	TAKING_DMG_HEAVY,
	SYMPTOM_COUGH,
	SYMPTOM_LAUGHTER,
	SYMPTOM_SNEEZE,
	JUMP,
	MELEE_ATTACK_LIGHT,
	MELEE_ATTACK_HEAVY
	INJURED_LIGHT,
	INJURED_MEDIUM,
	INJURED_HIGH,
	FREEZING,
	HOT,
	SYMPTOM_FATIGUE,
	STAMINA_LOW_FILTER_UPPER,
	STAMINA_LOW_FILTER_MID,
	STAMINA_LOW_FILTER_LOWER,
	DROWNING_BREATH,
	DROWNING_PAIN,
	//--------------
	// Count bellow, put enums above
	//--------------
	ENUM_COUNT,
}

class PlayerSoundEventHandler extends SoundEventHandler
{
	PlayerBase m_Player;
	const int SOUND_EVENTS_MAX = EPlayerSoundEventID.ENUM_COUNT;
	static ref PlayerSoundEventBase m_AvailableStates[SOUND_EVENTS_MAX];
	static ref map<int,int> m_ConfigIDToScriptIDmapping = new ref map<int,int> ;
	ref PlayerSoundEventBase m_CurrentState;
	ref Timer m_UpdateTimer;
	
	
	void PlayerSoundEventHandler(PlayerBase player)
	{
		m_Player = player;
		
		if(!m_UpdateTimer && !m_Player.IsControlledPlayer())
		{
			m_UpdateTimer = new Timer();
			m_UpdateTimer.Run(1, this, "OnTick", null, true);//ticking for remotes, the controlled player is ticking on command handler at higher intervals
		}
		
		RegisterState(new HoldBreathSoundEvent());
		RegisterState(new ExhaustedBreathSoundEvent());
		RegisterState(new FatigueSoundEvent());
		RegisterState(new ReleaseBreathSoundEvent());
		RegisterState(new StaminaDownLight());
		RegisterState(new StaminaDownHeavy());
		RegisterState(new StaminaUpLight());
		RegisterState(new StaminaUpHeavy());
		RegisterState(new StaminaUpEnd());
		RegisterState(new DamageLightSoundEvent());
		RegisterState(new DamageHeavySoundEvent());
		RegisterState(new SneezeSoundEvent());
		RegisterState(new LaugherSoundEvent());
		RegisterState(new CoughSoundEvent());
		RegisterState(new JumpSoundEvent());
		RegisterState(new MeleeAttackLightEvent());
		RegisterState(new MeleeAttackHeavyEvent());
		RegisterState(new StaminaNormalDummy());
		RegisterState(new InjuryLightSoundEvent());
		RegisterState(new InjuryMediumSoundEvent());
		RegisterState(new InjuryHeavySoundEvent());
		RegisterState(new FreezingSoundEvent());
		RegisterState(new HotSoundEvent());
		RegisterState(new StaminaLowFilterUpper());
		RegisterState(new StaminaLowFilterMid());
		RegisterState(new StaminaLowFilterLower());
		RegisterState(new DrowningEvent1());
		RegisterState(new DrowningEvent2());

	}
	
	void RegisterState(PlayerSoundEventBase state)
	{
		int index = state.GetSoundEventID();
		m_AvailableStates[index] = state;
		m_ConfigIDToScriptIDmapping.Insert(state.GetSoundVoiceAnimEventClassID(),index);
	}
	
	
	void OnTick(float delta_time)
	{
		if (m_CurrentState)
		{ 
			if( m_CurrentState.IsFinished())
				m_CurrentState = null;
			else
				m_CurrentState.OnTick(delta_time);
		}
	}
	
	int ConvertAnimIDtoEventID(int anim_id)
	{
		return m_ConfigIDToScriptIDmapping.Get(anim_id);
	}
	
	override static EPlayerSoundEventType GetSoundEventType(int id)
	{
		return m_AvailableStates[id].GetSoundEventType();
	}
	
	override int GetCurrentStateEventID()
	{
		if(m_CurrentState)
		{
			return m_CurrentState.GetSoundEventID();
		}
		return -1;
	}
	
	override int GetCurrentStateEventType()
	{
		if(m_CurrentState)
		{
			return m_CurrentState.GetSoundEventType();
		}
		return -1;
	}

	override bool PlayRequestEx(EPlayerSoundEventID id, bool sent_from_server = false, int param = 0)
	{
		if (id < 0 || id > (SOUND_EVENTS_MAX - 1))
		{
			Error("EPlayerSoundEventID out of bounds");
		}

		PlayerSoundEventBase requested_state = m_AvailableStates[id];
		if ( sent_from_server && (param & EPlayerSoundEventParam.SKIP_CONTROLLED_PLAYER) && m_Player.GetInstanceType() == DayZPlayerInstanceType.INSTANCETYPE_CLIENT )
		{
			return false;
		}
			
		if ( !requested_state.CanPlay(m_Player) )
		{
			return false;
		}

		if (m_CurrentState)
		{
			if (param & EPlayerSoundEventParam.HIGHEST_PRIORITY)
			{
				m_CurrentState.Stop();
			}
			else
			{
				int current_type =  m_CurrentState.GetSoundEventType();
				//int requested_type = requested_state.GetSoundEventType();
				
				if ( (requested_state.GetPriorityOverTypes() & current_type) == 0 )
				{
					return false;
				}
				if (!requested_state.HasPriorityOverCurrent(m_Player, id, current_type) )
				{
					return false;
				}
	
				m_CurrentState.Stop();
			}
		}
		m_CurrentState = PlayerSoundEventBase.Cast(requested_state.ClassName().ToType().Spawn());
		m_CurrentState.InitEx(m_Player, param);
		if (m_CurrentState.Play())
		{
			m_CurrentState.OnPlay(m_Player);
		}
		return true;
	}
	
	
	override bool PlayRequest(EPlayerSoundEventID id, bool sent_from_server = false)
	{
		return PlayRequestEx(id, sent_from_server);
	}
		
}
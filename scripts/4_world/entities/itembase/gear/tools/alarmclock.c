class AlarmClock_ColorBase: ClockBase
{
	const string		RINGING_SOUND 				= "AlarmClock_Ring_Loop_SoundSet";
	const string		TURN_TOGGLE_SOUND 			= "AlarmClock_Turn_Off_SoundSet";
	const string		DESTROYED_SOUND 			= "AlarmClock_Destroyed_SoundSet";
	const string		HIT_SOUND 					= "AlarmClock_Hit_SoundSet";

	
	static ref NoiseParams 	m_NoisePar;
	static NoiseSystem		m_NoiseSystem;
	
	override void Init()
	{
		super.Init();
		if ( GetGame().IsServer() )
		{
			m_NoiseSystem = GetGame().GetNoiseSystem();
			if ( m_NoiseSystem && !m_NoisePar )
			{
				// Create and load noise parameters
				m_NoisePar = new NoiseParams;
				m_NoisePar.LoadFromPath("cfgVehicles " + GetType() + " NoiseAlarmClock");
			}
		}
	}

	void ~AlarmClock_ColorBase()
	{
		#ifndef SERVER
		OnRingingStopClient();
		#endif
	}
	
	
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionSetAlarmClock);
		AddAction(ActionTurnOnAlarmClock);
		AddAction(ActionTurnOffAlarmClock);
	}

	override string GetToggleSound()
	{
		return TURN_TOGGLE_SOUND;
	}
	
	override string GetRingingSound()
	{
		return RINGING_SOUND;
	}
	
	override string GetDestroyedSound()
	{
		return DESTROYED_SOUND;
	}
	
	override string GetHitSound()
	{
		return HIT_SOUND;
	}
	
	override void GetDebugButtonNames(out string button1, out string button2, out string button3, out string button4)
	{
		button1 = "SetAlarmAhead1Min";
	}
	
	override string GetExplosiveTriggerSlotName()
	{
		return "TriggerAlarmClock";
	}
	
	override void OnDebugButtonPressServer(int button_index)
	{
		SetAlarmInXMins(1);
	}

	override string GetDebugText()
	{
		string debug_output;
		
		if( GetGame().IsDedicatedServer())
		{
			debug_output = "alarm in: " + GetAlarmInMin().ToString() + " mins" + "\n";
			debug_output +=  "current state: " +  typename.EnumToString(EAlarmClockState, m_State) + "\n";;
			debug_output +=  "ringing for " + m_RingingDuration.ToString()+ " secs" + "\n";
			debug_output +=  "ringing max " + GetRingingDurationMax().ToString()+ " secs" + "\n";
		}
		else
		{
			debug_output = "this is client";
		}
		return debug_output;
	}
	
	void OnUpdate()
	{
		if ( IsAlarmOn() )
		{
			//due to variable server time flow(day-night time accel), it's not possible to simply set a timer for X secs without some convoluted math/code, so we need to check at regular intervals
			int alarm_hand_in_minutes = ConvertAlarmHand01ToMins12h(m_AlarmTime01);
		
			int pass, hour, minute;
			GetGame().GetWorld().GetDate(pass, pass, pass, hour, minute);
			
			int curr_time_in_minutes = ConvertTimeToMins12h(hour, minute);
			
			//Print(GetAlarmInMin());
			
			if ( alarm_hand_in_minutes == curr_time_in_minutes )
			{
				MakeRingingStart();
			}
		}
		
		if ( IsRinging())
		{
			m_RingingDuration += UPDATE_TICK_RATE;
			
			if (m_RingingDuration >= GetRingingDurationMax())
			{
				TurnOff();
			}
			else if ( m_NoiseSystem )
			{
				m_NoiseSystem.AddNoiseTarget( GetPosition(), UPDATE_TICK_RATE, m_NoisePar);
			}
		}
	}

	protected void AnimateAlarmHand(float value)
	{
		SetAnimationPhaseNow("ClockAlarm", value);
	}

	
	override bool OnStoreLoad( ParamsReadContext ctx, int version )
	{
		if (!super.OnStoreLoad(ctx, version))
			return false;
		
		if (version < 126)
		{
			return true;
		}

		EAlarmClockState state;		
		
		if ( !ctx.Read( state ) )
		{
			return false;
		}

		float time;
		
		if ( !ctx.Read( time ) )
		{
			return false;
		}

		SetAlarmTimeServer(time);
		SetState(state);
		if ( state == EAlarmClockState.SET )
		{
			TurnOn();
		}
		else if (state == EAlarmClockState.RINGING )
		{
			MakeRingingStart();
		}
		
		return true;
	}
	
	override void OnStoreSave(ParamsWriteContext ctx)
	{
		super.OnStoreSave(ctx);
		
		ctx.Write(m_State);
		ctx.Write(m_AlarmTime01);
	}
	
	override void OnDebugSpawn()
	{
		TurnOn();
		MakeRingingStart();
	}

};
	
class AlarmClock_Red : AlarmClock_ColorBase {};
class AlarmClock_Blue : AlarmClock_ColorBase {};
class AlarmClock_Green : AlarmClock_ColorBase {};
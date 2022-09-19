class KitchenTimer: ClockBase
{
	const string		RINGING_SOUND 				= "KitchenTimer_Ring_Loop_SoundSet";
	const string		DESTROYED_SOUND 			= "AlarmClock_Destroyed_SoundSet";
	const string		HIT_SOUND 					= "AlarmClock_Hit_SoundSet";
	const string		WORKING_SOUND				= "KitchenTimer_Ticking_Loop_SoundSet";
	
	EffectSound 		m_RingingStopSound;
	static ref NoiseParams 	m_NoisePar;
	static NoiseSystem		m_NoiseSystem;

	int 				m_AlarmInSecs;
	
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
				m_NoisePar.LoadFromPath("cfgVehicles " + GetType() + " NoiseKitchenTimer");
			}
		}
	}
	
	override void SetActions()
	{
		super.SetActions();
		AddAction(ActionSetKitchenTimer);
		AddAction(ActionResetKitchenTimer);
	}

	override string GetExplosiveTriggerSlotName()
	{
		return "TriggerKitchenTimer";
	}
	
	override string GetToggleSound()
	{
		return "";
	};
	
	override string GetRingingSound()
	{
		return RINGING_SOUND;
	}
	
	string GetRingingStopSound()
	{
		return "KitchenTimer_Ring_End_SoundSet";
	}
	
	override string GetDestroyedSound()
	{
		return DESTROYED_SOUND;
	}
	
	override string GetHitSound()
	{
		return HIT_SOUND;
	}
	
	override string GetWorkingSound()
	{
		return WORKING_SOUND;
	}
	
	int GetMinutesMax()
	{
		return 45;
	}
	
	int Time01ToSeconds(float time01)
	{
		return Math.Lerp(0,GetMinutesMax() * 60, time01);
	}
	
	
	float SecondsTo01(int seconds)
	{
		return Math.InverseLerp(0,GetMinutesMax() * 60, seconds);
	}
	
	override float GetRingingDurationMax()
	{
		return 60;
	}
	
	override void TurnOff()
	{
		super.TurnOff();
		SEffectManager.DestroyEffect(m_WorkingSound);
	}
		
	void OnUpdate()
	{
		if (m_AlarmInSecs > 0)
		{
			m_AlarmInSecs -= UPDATE_TICK_RATE;
			float time01 = SecondsTo01(m_AlarmInSecs);
			SetAnimationPhaseNow("ClockAlarm", time01);
			if (IsRinging())
			{
				MakeRingingStop();
			}
		}
		else if (!IsRinging())
		{
			MakeRingingStart();
		}
		
		if (IsRinging())
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
	
	override protected void Disarm()
	{
		super.Disarm();
		SetAlarmTimeServerSecs(0);
	}

	override protected void OnRingingStopClient()
	{
		if (m_RingingSoundLoop)
			PlaySoundSet( m_RingingStopSound, GetRingingStopSound(), 0, 0);
		super.OnRingingStopClient();
		
	}
	
	override bool OnStoreLoad( ParamsReadContext ctx, int version )
	{
		if (!super.OnStoreLoad(ctx, version))
			return false;

		if (version < 128)
		{
			return true;
		}

		EAlarmClockState state;		
		
		if ( !ctx.Read( state ) )
		{
			return false;
		}

		int time;
		
		if (!ctx.Read( time ))
		{
			return false;
		}

		SetState(state);
		
		if (state == EAlarmClockState.SET)
		{
			SetAlarmTimeServerSecs(time);
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
		ctx.Write(m_AlarmInSecs);

	}
	
	
	
	//---------------------------------------------------------------------------------------------------------
	//---------------------------------------------- Public methods -------------------------------------------
	//---------------------------------------------------------------------------------------------------------
	
	override void SetAlarmTimeServer(float time01)
	{
		SetAnimationPhaseNow("ClockAlarm", time01);
		m_AlarmInSecs = Time01ToSeconds(time01);
		
		if (m_AlarmInSecs > 0 )
		{
			TurnOn();
		}
	}
	
	void SetAlarmTimeServerSecs(int inSecs)
	{
		SetAlarmTimeServer(SecondsTo01(inSecs));
	}
	
	
	//----------------------------------
	//------------- DEBUG --------------
	//----------------------------------
	
	override void GetDebugButtonNames(out string button1, out string button2, out string button3, out string button4)
	{
		button1 = "SetAlarmAhead";
	}
	
	override void OnDebugButtonPressServer(int button_index)
	{
		if (button_index == 1)
		{
			SetAlarmTimeServerSecs(10);
		}
	}

	override string GetDebugText()
	{
		string debug_output;
		
		if( GetGame().IsDedicatedServer())
		{
			debug_output = "alarm in: " + m_AlarmInSecs.ToString() + " secs" + "\n";
			debug_output +=  "current state: " +  typename.EnumToString(EAlarmClockState, m_State) + "\n";
			debug_output +=  "ringing for " + m_RingingDuration.ToString()+ " secs" + "\n";
			debug_output +=  "ringing max " + GetRingingDurationMax().ToString()+ " secs" + "\n";
		}
		else
		{
			debug_output = "this is client";
		}
		return debug_output;
	}
	
};
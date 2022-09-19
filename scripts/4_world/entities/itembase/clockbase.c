enum EAlarmClockState
{
	UNSET,
	SET,
	RINGING,
	//-----------
	COUNT
}

class ClockBase : Inventory_Base
{
	float 				m_AlarmTime01;
	int 				m_State = EAlarmClockState.UNSET;
	static const float 	UPDATE_TICK_RATE 			= 1; // Clock update tick frequency
	ref Timer 			m_TimerUpdate;
	float				m_RingingDuration;
	int 				m_StatePrev = -1;
	
	EffectSound 		m_RingingSoundLoop;
	EffectSound 		m_TurnOnSound;
	EffectSound 		m_DestoryedSound;
	EffectSound 		m_HitSound;
	EffectSound 		m_WorkingSound;

	const float 		RINGING_DURATION_MAX 		= 60;//in secs, at or past this value, the clock stops ringing

	void ClockBase()
	{
		Init();
	}
	
	void ~ClockBase()
	{
		SEffectManager.DestroyEffect(m_WorkingSound);
		SEffectManager.DestroyEffect(m_HitSound);
		SEffectManager.DestroyEffect(m_DestoryedSound);
		SEffectManager.DestroyEffect(m_TurnOnSound);
		SEffectManager.DestroyEffect(m_RingingSoundLoop);
	}
	
	void Init() 
	{
		RegisterNetSyncVariableInt("m_State",0, EAlarmClockState.COUNT - 1);
	}
	
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionAttachExplosivesTrigger);
	}
	
	protected int GetAlarmInMin()
	{
		int alarm_hand_in_mins = ConvertAlarmHand01ToMins12h(m_AlarmTime01);
		
		int pass, hour, minute;
		GetGame().GetWorld().GetDate(pass, pass, pass, hour, minute);
		
		int curr_time_in_minutes = ConvertTimeToMins12h(hour, minute);
		int ring_in_mins = GetTimeDiffInMins12h(curr_time_in_minutes, alarm_hand_in_mins);
		return ring_in_mins;
	}
	
	static int ConvertAlarmHand01ToMins12h(float time01)
	{
		return Math.Lerp(0,12*60,time01);
	}
	
	static int ConvertAlarmHand01ToMins(float time01, int mins_max/*what's the upper range in minutes we are trying to map the time01 to*/)
	{
		return Math.Lerp(0,mins_max,time01);
	}
	
	static float ConvertMins12hToAlarmHand01(int mins)
	{
		return Math.InverseLerp(0,12*60,mins);
	}

	
	static int ConvertTimeToMins12h(int hour, int minute)
	{
		if (hour >= 12)
			hour -= 12;  
		return hour * 60 + minute;
	}
	
	static int GetTimeDiffInMins12h(int from_mins, int to_mins)
	{
		if (to_mins > from_mins)
		{
			return to_mins - from_mins;
		}
		else if (to_mins < from_mins)
		{
			return ((12 * 60) - from_mins) + to_mins;
		}
		else return 0;
	}
	
	string GetToggleSound()
	{
		return "";
	}
	string GetRingingSound()
	{
		return "";
	}
	string GetHitSound()
	{
		return "";
	}
	string GetDestroyedSound()
	{
		return "";
	}
	string GetWorkingSound()
	{
		return "";
	}
	
	override void EEKilled(Object killer)
	{
		super.EEKilled(killer);
		TurnOff();
	}
	
	override void EEHitByRemote(int damageType, EntityAI source, int component, string dmgZone, string ammo, vector modelPos)
	{
		super.EEHitByRemote(damageType, source, component, dmgZone, ammo, modelPos);
		PlaySoundSet( m_HitSound, GetHitSound(), 0, 0 );
	}
	
	override void OnDamageDestroyed(int oldLevel)
	{
		super.OnDamageDestroyed(oldLevel);
		
		if (GetGame().IsClient())
		{
			OnRingingStopClient();

			if (oldLevel != -1)
			{
				PlaySoundSet(m_DestoryedSound, GetDestroyedSound(), 0, 0);
			}
		}
	}

	protected void OnRingingStartServer()
	{
		ActivateParent();
	}

	protected void OnRingingStartClient()
	{
		PlaySoundSetLoop( m_RingingSoundLoop, GetRingingSound(), 0, 0 );
		if (m_WorkingSound)
		{
			SEffectManager.DestroyEffect(m_WorkingSound);
		}
	}
	
	protected void OnRingingStopServer();
	
	protected void OnRingingStopClient()
	{
		SEffectManager.DestroyEffect(m_RingingSoundLoop);
	}
	
	void SetAlarmInXMins(int in_mins)
	{
		int pass, hour, minute;
		GetGame().GetWorld().GetDate(pass, pass, pass, hour, minute);
		int mins12h = ConvertTimeToMins12h(hour, minute) + in_mins;
		float time01 = ConvertMins12hToAlarmHand01(mins12h);
		SetAlarmTimeServer(time01);
		Arm();
	}
	
	float GetRingingDurationMax()
	{
		return RINGING_DURATION_MAX;
	}
	
	
	protected void SetupTimerServer()
	{
		m_TimerUpdate = new Timer;
		m_TimerUpdate.Run(UPDATE_TICK_RATE , this, "OnUpdate", null, true);
	}
	
	protected void SetState(EAlarmClockState state)
	{
		m_State = state;
		SetSynchDirty();
	}

	protected void Disarm()
	{
		SetState(EAlarmClockState.UNSET);
	}
	
	protected void Arm()
	{
		SetupTimerServer();
		SetState(EAlarmClockState.SET);
		m_RingingDuration = 0;
	}

	protected void ActivateParent()
	{
		if (GetHierarchyParent())
		{
			ItemBase parent = ItemBase.Cast(GetHierarchyParent());
			if (parent)
			{
				parent.OnActivatedByItem(this);
			}
		}
	}
	
	protected void MakeRingingStart()
	{
		if (!m_TimerUpdate)
			SetupTimerServer();
		SetState(EAlarmClockState.RINGING);
		
		OnRingingStartServer();
	}
	
	protected void MakeRingingStop()
	{
		SetState(EAlarmClockState.UNSET);
		
		OnRingingStopServer();
	}

	void TurnOnClient();

	void TurnOffClient();
	
	override void OnVariablesSynchronized()
	{
		super.OnVariablesSynchronized();
		
		if (m_State != m_StatePrev)//state changed
		{
			if (m_StatePrev == EAlarmClockState.RINGING)
			{
				OnRingingStopClient();
			}
			else if (m_State == EAlarmClockState.RINGING)
			{
				OnRingingStartClient();
			}
			if (m_State == EAlarmClockState.SET)
			{
				if (m_StatePrev != -1 || IsInitialized())
				{	
					PlaySoundSet( m_TurnOnSound, GetToggleSound(), 0, 0 );
				}
				if (GetWorkingSound())
				{
					PlaySoundSet( m_WorkingSound, GetWorkingSound(), 0, 0, true );
				}
			}
			else if (m_State == EAlarmClockState.UNSET)
			{
				if (m_StatePrev == EAlarmClockState.SET)
				{
					if (m_WorkingSound)
					{
						SEffectManager.DestroyEffect(m_WorkingSound);
					}
					if (m_StatePrev != -1 || IsInitialized())
					{	
						PlaySoundSet( m_TurnOnSound, GetToggleSound(), 0, 0 );
					}
				}
			}
			m_StatePrev = m_State;
		}
	}
	
	
	//---------------------------------------------------------------------------------------------------------
	//---------------------------------------------- Public methods -------------------------------------------
	//---------------------------------------------------------------------------------------------------------
	
	bool IsRinging()
	{
		return (m_State == EAlarmClockState.RINGING);
	}
	
	bool IsAlarmOn()
	{
		return (m_State == EAlarmClockState.SET);
	}
	
	void TurnOn()
	{
		Arm();
	}

	void TurnOff()
	{
		if ( IsRinging() )
		{
			MakeRingingStop();
		}
		else
		{
			Disarm();
		}
		
		m_TimerUpdate = null;
	}

	void SetAlarmTimeServer(float time01)
	{
		SetAnimationPhaseNow("ClockAlarm", time01);
		m_AlarmTime01 = time01;
	}
}
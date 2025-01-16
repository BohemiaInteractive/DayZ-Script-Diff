class CAContinuousRepeatFishing : CAContinuousRepeat
{
	protected const int NO_SIGNAL = -1;
	
	bool 	m_SignalTriggered = false;
	float	m_SignalStartTime;
	float	m_SignalEndTime;
	float	m_SignalDuration;
	
	void CAContinuousRepeatFishing( float time_to_complete_action )
	{
		m_DefaultTimeToComplete = time_to_complete_action;
	}
	
	override int Execute( ActionData action_data )
	{
		if (!action_data.m_Player)
		{
			return UA_ERROR;
		}
		
		if (m_TimeElpased < m_TimeToComplete)
		{
			m_TimeElpased += action_data.m_Player.GetDeltaT();
			m_TotalTimeElpased += action_data.m_Player.GetDeltaT();
			
			//handle signal triggers
			if (m_SignalStartTime != NO_SIGNAL && m_SignalDuration != NO_SIGNAL)
			{
				if (m_TimeElpased >= m_SignalStartTime)
				{
					if (!m_SignalTriggered && m_TimeElpased < m_SignalEndTime)
					{
						m_SignalTriggered = true;
						OnSignalStart(action_data);
					}
					else if (m_SignalTriggered && m_TimeElpased >= m_SignalEndTime)
					{
						m_SignalTriggered = false;
						OnSignalEnd(action_data);
					}
				}
			}
		}
		else
		{
			m_TimeElpased = m_TimeToComplete - m_TimeElpased;
			OnCompletePogress(action_data);
			
			m_SignalTriggered = false;
		}
		
		FishingActionData fad = FishingActionData.Cast(action_data);
		if (fad.m_FishingResult != -1)
		{
			return UA_FINISHED;
		}
		else
		{
			return UA_PROCESSING;
		}
	}
	
	void SetNewSignalData(float startTime, float durationTime)
	{
		if (startTime == NO_SIGNAL && durationTime == NO_SIGNAL)
		{
			m_SignalStartTime = startTime;
			m_SignalDuration = durationTime;
		}
		else
		{
			//float maximalUsableDuration = Math.Clamp(m_TimeToComplete - UAFishingConstants.SIGNAL_DEADZONE_END_DURATION - UAFishingConstants.SIGNAL_DEADZONE_START_DURATION,UAFishingConstants.SIGNAL_DURATION_MIN_BASE,UAFishingConstants.SIGNAL_DURATION_MAX_BASE);
			float maximalUsableDuration = m_TimeToComplete - UAFishingConstants.SIGNAL_DEADZONE_END_DURATION - UAFishingConstants.SIGNAL_DEADZONE_START_DURATION;
			if (maximalUsableDuration < UAFishingConstants.SIGNAL_DURATION_MIN_BASE)
			{
				if (maximalUsableDuration <= 0)
				{
					float deadzonesTime = UAFishingConstants.SIGNAL_DEADZONE_END_DURATION + UAFishingConstants.SIGNAL_DEADZONE_START_DURATION;
					#ifdef DEVELOPER
					if (IsCLIParam("catchingErrors"))
						ErrorEx("Cycle m_TimeToComplete: " + m_TimeToComplete + " shorter than combined deadzone times: " + deadzonesTime + "! No time left for any signal, skipping!");
					else
					#endif
					Debug.Log("Cycle m_TimeToComplete: " + m_TimeToComplete + " shorter than combined deadzone times: " + deadzonesTime + "! No time left for any signal, skipping!","FishingCA");
				}
				else
				{
					#ifdef DEVELOPER
					if (IsCLIParam("catchingErrors"))
						ErrorEx("maximalUsableDuration " + maximalUsableDuration + " is shorter than UAFishingConstants.SIGNAL_DURATION_MIN_BASE, skipping signal!");
					else
					#endif
					Debug.Log("maximalUsableDuration " + maximalUsableDuration + " is shorter than UAFishingConstants.SIGNAL_DURATION_MIN_BASE, skipping signal!","FishingCA");
				}
				
				m_SignalStartTime = NO_SIGNAL;
				m_SignalDuration = NO_SIGNAL;
				return;
			}
			
			m_SignalDuration = Math.Clamp(durationTime,UAFishingConstants.SIGNAL_DURATION_MIN_BASE,Math.Min(maximalUsableDuration,UAFishingConstants.SIGNAL_DURATION_MAX_BASE)); //scaled down if needed, guaranteed at least SIGNAL_DURATION_MIN_BASE
			m_SignalStartTime = Math.Clamp(startTime,UAFishingConstants.SIGNAL_DEADZONE_START_DURATION,(m_TimeToComplete - m_SignalDuration - UAFishingConstants.SIGNAL_DEADZONE_END_DURATION));
			m_SignalEndTime = m_SignalStartTime + m_SignalDuration;
			
			if (durationTime != m_SignalDuration)
			{
				#ifdef DEVELOPER
				Debug.Log("Signal durationTime adjusted from " + durationTime + " to " + m_SignalDuration + ". maximalUsableDuration: " + maximalUsableDuration + ", m_TimeToComplete: " + m_TimeToComplete,"FishingCA");
				#endif
			}
			if (startTime != m_SignalStartTime)
			{
				#ifdef DEVELOPER
				Debug.Log("Signal startTime adjusted from " + startTime + " to " + m_SignalStartTime + ". m_SignalDuration: " + m_SignalDuration + ", m_TimeToComplete: " + m_TimeToComplete,"FishingCA");
				#endif
			}
		}
	}
	
	protected void OnSignalStart(ActionData action_data)
	{
		FishingActionData data;
		if (Class.CastTo(data,action_data))
		{
			ActionFishingNew action;
			if (Class.CastTo(action,data.m_Action))
				action.OnSignalStart(data);
		}
	}
	
	protected void OnSignalEnd(ActionData action_data)
	{
		FishingActionData data;
		if (Class.CastTo(data,action_data))
		{
			ActionFishingNew action;
			if (Class.CastTo(action,data.m_Action))
				action.OnSignalEnd(data);
		}
	}
};
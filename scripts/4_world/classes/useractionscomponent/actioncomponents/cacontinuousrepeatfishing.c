class CAContinuousRepeatFishing : CAContinuousRepeat
{
	bool m_SignalTriggered = false;
	float	m_SignalStartTime;
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
			if (m_SignalStartTime != -1 && m_SignalDuration != -1)
			{
				if (m_TimeElpased >= m_SignalStartTime)
				{
					float signalEndTime = m_SignalStartTime + m_SignalDuration;
					if (!m_SignalTriggered && m_TimeElpased < signalEndTime)
					{
						m_SignalTriggered = true;
						OnSignalStart(action_data);
					}
					else if (m_SignalTriggered && m_TimeElpased >= signalEndTime)
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
		if (startTime == -1 && durationTime == -1)
		{
			m_SignalStartTime = startTime;
			m_SignalDuration = durationTime;
		}
		else
		{
			float maximalUsableDuration = m_TimeToComplete - UAFishingConstants.SIGNAL_DEADZONE_END_DURATION - UAFishingConstants.SIGNAL_DEADZONE_START_DURATION;
			m_SignalStartTime = Math.Clamp(startTime,UAFishingConstants.SIGNAL_DEADZONE_START_DURATION,(m_TimeToComplete - UAFishingConstants.SIGNAL_DURATION_MIN_BASE - UAFishingConstants.SIGNAL_DEADZONE_END_DURATION));
			m_SignalDuration = Math.Clamp(durationTime,UAFishingConstants.SIGNAL_DURATION_MIN_BASE,Math.Min(maximalUsableDuration,UAFishingConstants.SIGNAL_DURATION_MAX_BASE)); //scaled down if needed
			
			if (startTime != m_SignalStartTime)
				Debug.Log("Signal startTime adjusted from " + startTime + " to " + m_SignalStartTime);
			if (durationTime != m_SignalDuration)
				Debug.Log("Signal durationTime adjusted from " + durationTime + " to " + m_SignalDuration);
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
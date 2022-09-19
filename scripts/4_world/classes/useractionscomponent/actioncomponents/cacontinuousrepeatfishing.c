class CAContinuousRepeatFishing : CAContinuousRepeat
{
	void CAContinuousRepeatFishing( float time_to_complete_action )
	{
		m_DefaultTimeToComplete = time_to_complete_action;
	}
	
	override int Execute( ActionData action_data )
	{
		//Print("CAContinuousRepeatFishing | Execute");
		if ( !action_data.m_Player )
		{
			//Print("CAContinuousRepeatFishing | UA_ERROR");
			return UA_ERROR;
		}

		if ( m_TimeElpased < m_TimeToComplete )
		{
			m_TimeElpased += action_data.m_Player.GetDeltaT();
			m_TotalTimeElpased += action_data.m_Player.GetDeltaT();
			//Print("CAContinuousRepeatFishing | Execute | m_TimeElpased: " + m_TimeElpased);
			return UA_PROCESSING;
		}
		else
		{
			m_SpentUnits.param1 = m_TimeElpased;
			SetACData(m_SpentUnits);
			m_TimeElpased = m_TimeToComplete - m_TimeElpased;
			OnCompletePogress(action_data);
			
			FishingActionData fad = FishingActionData.Cast(action_data);
			if (fad && fad.m_FishingResult != -1)
			{
				//Print("CAContinuousRepeatFishing | UA_FINISHED");
				return UA_FINISHED;
			}
			else
			{
				//Print("CAContinuousRepeatFishing | UA_PROCESSING");
				return UA_PROCESSING;
			}
		}
	}
};
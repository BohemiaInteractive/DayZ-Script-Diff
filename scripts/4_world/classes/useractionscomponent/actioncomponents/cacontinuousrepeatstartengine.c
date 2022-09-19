class CAContinuousRepeatStartEngine : CAContinuousRepeat
{
	void CAContinuousRepeatStartEngine( float time_to_complete_action )
	{
		m_DefaultTimeToComplete = time_to_complete_action;
	}
	
	override int Execute( ActionData action_data )
	{
		if ( !action_data.m_Player )
		{
			return UA_ERROR;
		}
		
		HumanCommandVehicle vehCommand = action_data.m_Player.GetCommand_Vehicle();
		if( vehCommand )
		{
			Transport trans = vehCommand.GetTransport();
			
			if ( trans )
			{
				Car car;
				if ( Class.CastTo(car, trans) && car.EngineIsOn() )
				{
					return UA_FINISHED;
				}
			}
		}

		if ( m_TimeElpased < m_TimeToComplete )
		{
			m_TimeElpased += action_data.m_Player.GetDeltaT();
			m_TotalTimeElpased += action_data.m_Player.GetDeltaT();
			return UA_PROCESSING;
		}
		else
		{
			m_SpentUnits.param1 = m_TimeElpased;
			SetACData(m_SpentUnits);
			m_TimeElpased = 0;
			OnCompletePogress(action_data);
			return UA_PROCESSING;
		}
	}
};
class CAContinuousTime : CAContinuousBase
{
	protected float 			m_LocalTimeElpased;
	protected float				m_TimeElpased;
	protected float				m_AdjustedTimeToComplete;
	protected float				m_DefaultTimeToComplete;	
	protected ref Param1<float>	m_SpentUnits;
	
	void CAContinuousTime( float time_to_complete_action )
	{
		m_DefaultTimeToComplete = time_to_complete_action;
	}
	
	override void Setup( ActionData action_data )
	{
		m_TimeElpased = 0;
		if ( !m_SpentUnits )
		{ 
			m_SpentUnits = new Param1<float>(0);
		}
		else
		{	
			m_SpentUnits.param1 = 0;
		}
		
		m_AdjustedTimeToComplete = action_data.m_Player.GetSoftSkillsManager().SubtractSpecialtyBonus( m_DefaultTimeToComplete, m_Action.GetSpecialtyWeight(), true);
	}
	
	override int Execute( ActionData action_data )
	{
		if ( m_DefaultTimeToComplete < 0 ) //for infinite actions
		{
			return UA_PROCESSING;
		}
		
		if ( !action_data.m_Player )
		{
			return UA_ERROR;
		}
		
		if ( m_TimeElpased < m_AdjustedTimeToComplete )
		{
			m_TimeElpased += action_data.m_Player.GetDeltaT();
			return UA_PROCESSING;
		}
		else
		{
			if ( m_SpentUnits )
			{
				m_SpentUnits.param1 = m_TimeElpased;
				SetACData(m_SpentUnits);
			}
			OnCompletePogress(action_data);
			return UA_FINISHED;
		}
	}
	
	override int Cancel(ActionData action_data )
	{
		if ( m_SpentUnits )
		{
			m_SpentUnits.param1 = m_TimeElpased;
			SetACData(m_SpentUnits);
		}
		if ( m_DefaultTimeToComplete < 0 ) //for infinite actions
		{
			return UA_FINISHED;
		}
		
		return UA_CANCEL;
	}

	override float GetProgress()
	{	
		if ( m_AdjustedTimeToComplete != 0 )
		{
			//progress = m_TimeElpased/m_AdjustedTimeToComplete;
			return m_TimeElpased/m_AdjustedTimeToComplete;
		}
		else
		{
			Print("USER ACTION COMPONENT CONTINUOUS TIME - WRONG TIME TO COMPLETE CALCULATION - Incorrect data for progress bar!");
		}
		return 1;
	}

};
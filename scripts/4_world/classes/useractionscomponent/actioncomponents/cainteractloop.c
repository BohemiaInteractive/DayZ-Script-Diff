class CAInteractLoop : CABase
{	
	protected float 			m_LocalTimeElpased;
	protected float				m_TimeElpased;
	protected float				m_TimeToComplete;
	protected float				m_DefaultTimeToComplete;	
	protected ref Param1<float>	m_SpentUnits;
	
	void CAInteractLoop( float time_to_complete_action )
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
		
		m_TimeToComplete = action_data.m_Player.GetSoftSkillsManager().SubtractSpecialtyBonus( m_DefaultTimeToComplete, m_Action.GetSpecialtyWeight(), true);
	}
	
	override int Execute( ActionData action_data )
	{
		if ( !action_data.m_Player )
		{
			return UA_ERROR;
		}
		
		if ( m_TimeElpased < m_TimeToComplete )
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
			return UA_FINISHED;
		}
	}

};
class CABase 
{	
	protected ActionBase				m_Action;
	protected float 					m_LastTick;
	protected ref Param					m_ACData;
	protected ref Param2<float,float>	m_ProgressParam;
	
	void Init( ActionData action_data )
	{
		m_ProgressParam = new Param2<float,float>(0,0);
		m_Action = action_data.m_Action;
		if ( !m_ACData ) 
		{
			m_ACData = new Param;
		}
		m_LastTick = GetGame().GetTime();
		Setup(action_data);
	}
	
	void Setup( ActionData action_data  )
	{
	}

	int Execute( ActionData action_data  )
	{
		return UA_ERROR;
	}
	
	int Cancel( ActionData action_data )
	{
		return UA_CANCEL;
	}

	int Interrupt( ActionData action_data )
	{
		Cancel( action_data );
		return UA_INTERRUPT;
	}
		
	void SetACData(Param units) //overload this method if you want to send more than one parameter out of the action component
	{
		if ( m_ACData )
		{
			m_ACData = units;
		}
	}
	
	Param GetACData() 
	{
		if ( m_ACData ) 
		{
			return m_ACData;
		}
		return NULL;
	}
	
	bool IsContinuousAction() //informs command callback whether action is looped or oneshot
	{
		return false;
	}
	
	float GetProgress()
	{
		return 0;
	}
};
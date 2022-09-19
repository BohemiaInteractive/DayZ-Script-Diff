class SymptomCB extends HumanCommandActionCallback
{
	//int m_SymptomUID;
	float m_RunTime;
	float m_StartingTime;
	PlayerBase m_Player;
	
	override void OnFinish(bool pCanceled)
	{
		if( m_Player && m_Player.GetSymptomManager())
		{
			m_Player.GetSymptomManager().OnAnimationFinished();
		}
	}
	
	
	void Init(float run_time, PlayerBase player)
	{
		EnableCancelCondition(true);
		m_RunTime = run_time * 1000;
		m_StartingTime = GetGame().GetTime();
		m_Player = player;
		
		if( m_Player && m_Player.GetSymptomManager())
		{
			m_Player.GetSymptomManager().OnAnimationStarted();
		}
	}
	
	bool CancelCondition()
	{
		if(m_RunTime > 0 && (GetGame().GetTime() > m_StartingTime + m_RunTime))
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	override bool IsSymptomCallback()
	{
		return true;
	}
	
};

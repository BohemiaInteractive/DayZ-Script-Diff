/*
class AgentsNotfr: NotifierBase
{

	private const float 	DEC_TRESHOLD_LOW 	= 0;
	private const float 	DEC_TRESHOLD_MED 	= -0.5;
	private const float 	DEC_TRESHOLD_HIGH	= -1;
	private const float 	INC_TRESHOLD_LOW 	= 0;
	private const float 	INC_TRESHOLD_MED 	= 0.5;
	private const float 	INC_TRESHOLD_HIGH	= 1;
	
	void AgentsNotfr(NotifiersManager manager)
	{
		SetActive(true);
		m_TendencyBufferSize = 3;//for best results, this should be somewhat aligned with modifier frequency
	}

	override int GetNotifierType()
	{
		return eNotifiers.NTF_AGENT_INFECTION;
	}
	
	override void DisplayTendency(float delta)
	{
		int tendency = CalculateTendency(delta, INC_TRESHOLD_LOW, INC_TRESHOLD_MED, INC_TRESHOLD_HIGH, DEC_TRESHOLD_LOW, DEC_TRESHOLD_MED, DEC_TRESHOLD_HIGH);
		//GetVirtualHud().SetStatus(eDisplayElements.DELM_TDCY_BACTERIA,tendency);
		
		DisplayElementTendency dis_elm = DisplayElementTendency.Cast(GetVirtualHud().GetElement(eDisplayElements.DELM_TDCY_BACTERIA));
		
		if( dis_elm )
		{
			dis_elm.SetTendency(tendency);
		}
	}
	
	override float GetObservedValue()
	{
		float count = m_Player.GetTotalAgentCount();
		//Debug.Log( "GetObservedValue:" + count.ToString(),"Tendency");
		return count;
	}
};*/
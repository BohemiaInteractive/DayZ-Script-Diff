class UndergroundBunkerTriggerData
{
	vector m_Position;
	vector m_Size;
	EUndergroundTriggerType m_Type;
	string m_LinkedDoorSelection;
};

class UndergroundBunkerTrigger : UndergroundTrigger
{
	string m_LinkedDoorSelection;
	
	void Init(UndergroundBunkerTriggerData data)
	{
		m_Type = data.m_Type;
		m_LinkedDoorSelection = data.m_LinkedDoorSelection;
	}

	override protected void OnEnterClientEvent(TriggerInsider insider) 
	{
		PlayerBase player = PlayerBase.Cast(insider.GetObject());
		if (player)
		{
			UndergroundBunkerHandlerClient handler = player.GetUndergroundBunkerHandler();
			if (handler)
			{
				handler.OnTriggerEnter(this);
			}
		}
	}
	
	override protected void OnLeaveClientEvent(TriggerInsider insider) 
	{
		PlayerBase player = PlayerBase.Cast(insider.GetObject());
		if (player)
		{
			UndergroundBunkerHandlerClient handler = player.GetUndergroundBunkerHandler();
			if (handler)
			{
				handler.OnTriggerLeave(this);
			}
		}
	}
}
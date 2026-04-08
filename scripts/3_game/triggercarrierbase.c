class UndergroundTriggerCarrierBase : ScriptedEntity
{
	int m_TriggerIndex = -1;
	
	protected int m_ParentObjectNetIdLow 	= -1;
	protected int m_ParentObjectNetIdHigh 	= -1;

	protected Object m_ParentObject;
	
	void SetIndex(int index)
	{
		m_TriggerIndex = index;
		SetSynchDirty();
	}
	
	void SetParent(Object parent)
	{
		int low, high;
		parent.GetNetworkID(low, high);
		
		m_ParentObjectNetIdLow 	= low;
		m_ParentObjectNetIdHigh = high;

		m_ParentObject = parent;

		SetSynchDirty();
	}
}
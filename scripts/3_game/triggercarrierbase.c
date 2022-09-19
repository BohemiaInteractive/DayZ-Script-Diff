class UndergroundTriggerCarrierBase extends ScriptedEntity
{
	int m_TriggerIndex = -1;
	
	void SetIndex(int index)
	{
		m_TriggerIndex = index;
		SetSynchDirty();
	}
}
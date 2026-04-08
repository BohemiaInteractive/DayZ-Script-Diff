class UndergroundBunkerTriggerData
{
	vector m_Position;
	vector m_Size;
	EUndergroundTriggerType m_Type;
	string m_LinkedDoorSelection;
};

[Obsolete("replaced by the JsonUndergroundTriggers")]
class UndergroundBunkerTrigger : UndergroundTrigger
{
	string m_LinkedDoorSelection;
	
	void Init(UndergroundBunkerTriggerData data)
	{
		m_Type = data.m_Type;
		m_LinkedDoorSelection = data.m_LinkedDoorSelection;
	}
}

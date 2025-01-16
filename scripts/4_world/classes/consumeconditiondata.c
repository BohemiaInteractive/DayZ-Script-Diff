class ConsumeConditionData
{
	EntityAI m_Consumer;
	ItemBase m_ConsumedItem;
	//int m_ConsumptionType = -1;
	
	void ConsumeConditionData(EntityAI consumer, ItemBase item)
	{
		m_Consumer = consumer;
		m_ConsumedItem = item;
	}
}

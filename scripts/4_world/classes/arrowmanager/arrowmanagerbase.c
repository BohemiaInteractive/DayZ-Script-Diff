class ArrowManagerBase
{
	const int VERSION = 1;
	protected ref array<EntityAI> m_Arrows;
	
	void ArrowManagerBase()
	{
		m_Arrows = new array<EntityAI>();
	}

	void AddArrow(EntityAI arrow)
	{
		m_Arrows.Insert(arrow);
	}
	
	void RemoveArrow(EntityAI arrow)
	{
		m_Arrows.RemoveItem(arrow);
	}
		
	void ClearArrows()
	{
		foreach (EntityAI arrow : m_Arrows)
			arrow.DeleteSafe();

		m_Arrows.Clear();
	}
	
	EntityAI GetArrow(int index)
	{
		if (m_Arrows.Count() > index)
		{
			return m_Arrows.Get(index);
		}

		return null;
	}
	
}
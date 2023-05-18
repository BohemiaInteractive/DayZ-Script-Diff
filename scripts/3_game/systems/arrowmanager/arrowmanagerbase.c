class ArrowManagerBase
{
	protected const int VERSION = 2;
	protected ref array<EntityAI> m_Arrows;
	protected EntityAI m_Owner;
	
	void ArrowManagerBase(EntityAI owner)
	{
		m_Owner = owner;
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
		{
			if (arrow)
				arrow.DeleteSafe();
		}

		m_Arrows.Clear();
	}
	
	int GetArrowsCount()
	{
		return m_Arrows.Count();
	}
	
	void DropArrow(int index)
	{
		if (m_Arrows.Count() > index)
		{
			EntityAI arrow = m_Arrows.Get(index);
			vector pos = arrow.GetPosition();
			
			m_Owner.RemoveChild(arrow);
			
			vector m4[4];
			Math3D.MatrixIdentity4(m4);
			m4[3] = pos;
	
			arrow.PlaceOnSurfaceRotated(m4, pos);
			
			arrow.SetTransform(m4);
			arrow.PlaceOnSurface();
		}
	}
	
	void DropFirstArrow()
	{
		DropArrow(0);
	}
	
	void DropAllArrows()
	{
		for (int i = m_Arrows.Count() - 1; i >= 0 ; i--)
		{
			DropArrow(i);
		}
	}
	
	EntityAI GetArrow(int index)
	{
		if (m_Arrows.Count() > index)
		{
			return m_Arrows.Get(index);
		}

		return null;
	}
	
	EntityAI GetFirstArrow()
	{
		if (m_Arrows.Count())
		{
			return m_Arrows.Get(0);
		}

		return null;
	}

	EntityAI AcquireFirstArrow(bool keepTransform = false)
	{
		EntityAI entity = GetFirstArrow();
		m_Owner.RemoveChild(entity, keepTransform);
		return entity;
	}
	
}
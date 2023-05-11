class ArrowManagerPlayer : ArrowManagerBase
{	
	private static ref map<int,typename> m_TypeHashTable;
	
	void ArrowManagerPlayer(PlayerBase player)
	{
		if (!m_TypeHashTable)
		{
			InitializeHash();
		}
	}
	
	private static void InitializeHash()
	{
		m_TypeHashTable = new map<int,typename>();
		
		AddArrowTypeToHash("Ammo_HuntingBolt");
		AddArrowTypeToHash("Ammo_ImprovisedBolt_1");
		AddArrowTypeToHash("Ammo_ImprovisedBolt_2");
		AddArrowTypeToHash("Ammo_ImprovisedBolt_3");
	}
	
	private static void AddArrowTypeToHash(string ArrowType)
	{
		m_TypeHashTable.Insert(ArrowType.Hash(), ArrowType.ToType());
	}
	
	private static typename GetArrowTypeFromHash(int hash)
	{
		return m_TypeHashTable.Get(hash);
	}
	
	bool Save(ParamsWriteContext ctx)
	{
		ctx.Write(VERSION);
		int count = m_Arrows.Count();
		int i;
		//TODO MW Delete after find why sometimes arrow missing - most likely Life span
		for (i = count - 1; i >= 0; i--)
		{
			if (!m_Arrows.Get(i))
			{
				m_Arrows.Remove(i);
			}
		}
		count = m_Arrows.Count();
		
		ctx.Write(count);
		
		for (i = 0; i < count; i++)
		{
			EntityAI arrow = m_Arrows.Get(i);
			
			string type = arrow.GetType();
			ctx.Write(type.Hash());
			
			vector angle = arrow.GetLocalYawPitchRoll();
			vector pos = arrow.GetLocalPosition();
			
			ctx.Write(angle[0]);
			ctx.Write(angle[1]);
			ctx.Write(angle[2]);
			ctx.Write(pos[0]);
			ctx.Write(pos[1]);
			ctx.Write(pos[2]);
			
			int pivot = arrow.GetHierarchyPivot();
			ctx.Write(pivot);
		}

		return true;
	}

	
	bool Load(ParamsReadContext ctx)
	{
		int version;
		if (!ctx.Read(version))
		{
			return false;
		}
		
		int count;
		if (!ctx.Read(count))
		{
			return false;
		}
		
		for (int i = 0; i < count; i++)
		{
		
			if (version >= 1)
			{
				int hash;
				if (!ctx.Read(hash))
				{
					return false;
				}

				float angleF[3];
				float posF[3];
				float value;
				
				if (!ctx.Read(value))
				{
					return false;
				}
				angleF[0] = value;
				
				if (!ctx.Read(value))
				{
					return false;
				}
				angleF[1] = value;
				
				if (!ctx.Read(value))
				{
					return false;
				}
				angleF[2] = value;
				
				if (!ctx.Read(value))
				{
					return false;
				}
				posF[0] = value;
				
				if (!ctx.Read(value))
				{
					return false;
				}
				posF[1] = value;
				
				if (!ctx.Read(value))
				{
					return false;
				}
				posF[2] = value;
				
				vector angle = "0 0 0";
				vector pos = "0 0 0";

				angle = vector.ArrayToVec(angleF);
				pos = vector.ArrayToVec(posF);
				
				int pivot;
				if (!ctx.Read(pivot))
				{
					return false;
				}
				
				if (version >= 2)
				{
					#ifdef SERVER
					int spawnFlags = ECE_KEEPHEIGHT|ECE_DYNAMIC_PERSISTENCY;
					#else
					int spawnFlags = ECE_LOCAL|ECE_KEEPHEIGHT|ECE_DYNAMIC_PERSISTENCY;
					#endif
					
					typename arrowType = GetArrowTypeFromHash(hash);
					EntityAI arrow = EntityAI.Cast(GetGame().CreateObjectEx(arrowType.ToString(), pos, spawnFlags));
					if (arrow)
					{
						arrow.SetQuantityToMinimum();
						arrow.SetYawPitchRoll(angle);
						m_Owner.AddChild(arrow, pivot);
					}
				}
			}
		
		}

		return true;
	}
}

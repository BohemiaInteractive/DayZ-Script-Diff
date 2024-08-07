class PowerGeneratorStatic : PowerGeneratorBase
{
	protected static ref set<PowerGeneratorStatic> m_PowerGenerators = new set<PowerGeneratorStatic>();
	protected Land_WarheadStorage_PowerStation m_Parent;
	
	protected int m_ParentID1;
	protected int m_ParentID2;
	protected int m_ParentID3;
	protected int m_ParentID4;
	
	void PowerGeneratorStatic()
	{
		RegisterPersistentObject(this);
	}
	
	void ~PowerGeneratorStatic()
	{
		UnregisterPersistentObject(this);
	}

	static void RegisterPersistentObject(PowerGeneratorStatic obj)
	{
		m_PowerGenerators.Insert(obj);
	}
	
	static void UnregisterPersistentObject(PowerGeneratorStatic obj)
	{
		if (!m_PowerGenerators)
			return;
		int index = m_PowerGenerators.Find(obj);
		if (index != -1)
		{
			m_PowerGenerators.Remove(index);
		}
	}
	
	static PowerGeneratorStatic GetClosestGenerator(vector position, float tolerance)
	{
		float toleranceSq = tolerance * tolerance;
		float smallestDist = float.MAX;
		PowerGeneratorStatic closest;
		
		foreach (PowerGeneratorStatic obj:m_PowerGenerators)
		{
			float distSq = vector.DistanceSq(position,obj.GetPosition());
			if (distSq < toleranceSq)
			{
				return obj;
			}
		}
		return null;
	}
	
	void SetParent(Land_WarheadStorage_PowerStation parent)
	{
		m_Parent = parent;
	}
	
	// Generator is working
	override void OnWorkStart()
	{
		super.OnWorkStart();
		if (m_Parent)
		{
			m_Parent.OnGeneratorStart();
		}
	}
	
	// Turn off when this runs out of fuel
	override void OnWorkStop()
	{
		super.OnWorkStop();
		if (m_Parent)
		{
			m_Parent.OnGeneratorStop();
		}
	}

	// Checks sparkplug
	override bool HasSparkplug()
	{
		int slot = InventorySlots.GetSlotIdFromString("GlowPlug");
		EntityAI ent = GetInventory().FindAttachment(slot);

		return ent && !ent.IsRuined();
	}
	
	// Taking item into inventory
	override bool CanPutInCargo( EntityAI parent )
	{
		return false;
	}

	// Taking item into inventory
	override bool CanPutIntoHands(EntityAI player)
	{
		return false;
	}

	override void OnStoreSave(ParamsWriteContext ctx)
	{
		super.OnStoreSave(ctx);
		
		if (m_Parent)
			m_Parent.GetPersistentID(m_ParentID1,m_ParentID2, m_ParentID3, m_ParentID4);
		
		ctx.Write(m_ParentID1);
		ctx.Write(m_ParentID2);
		ctx.Write(m_ParentID3);
		ctx.Write(m_ParentID4);
	}
	
	override bool OnStoreLoad( ParamsReadContext ctx, int version )
	{
		if ( !super.OnStoreLoad( ctx, version ) )
			return false;

		if ( !ctx.Read(m_ParentID1))
		{
			return false;
		}
		if ( !ctx.Read(m_ParentID2))
		{
			return false;
		}
		if ( !ctx.Read(m_ParentID3))
		{
			return false;
		}
		if ( !ctx.Read( m_ParentID4))
		{
			return false;
		}
		
		return true;
	}
	
	override void EEOnAfterLoad()
	{
		Land_WarheadStorage_PowerStation powerStation = Land_WarheadStorage_PowerStation.Cast(GetGame().GetEntityByPersitentID(m_ParentID1, m_ParentID2, m_ParentID3, m_ParentID4));
		if (powerStation)
		{
			PowerGeneratorStatic otherGenerator = powerStation.GetPowerGenerator();
			if (otherGenerator)
			{
				otherGenerator.SetFuel(GetFuel());
				Delete();
			}
		}
	}
	
	override bool CanReleaseAttachment(EntityAI attachment)
	{
		if (!super.CanReleaseAttachment(attachment))
			return false;
		return !GetCompEM().IsWorking();
	}
	
	override bool IsTakeable()
	{
		return false;
	}

	override bool DisableVicinityIcon()
    {
        return true;
    }
}
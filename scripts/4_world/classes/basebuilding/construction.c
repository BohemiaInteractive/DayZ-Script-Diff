//! Used in BBB objects
class Construction : ConstructionBase
{
	protected BaseBuildingBase 	m_Parent;
	
	void Construction( EntityAI parent )
	{
		m_ConstructionParts = new map<string, ref ConstructionPart>;
		
		//set parent object
		SetParent(parent);
	}
	
	//legacy constructor (unused)
	void Construction( BaseBuildingBase parent )
	{
		m_ConstructionParts = new map<string, ref ConstructionPart>;
		SetParent(parent);
	}
	
	override void Init()
	{
		UpdateConstructionParts();
	}
	
	override void InitBaseState()
	{
		if (LogManager.IsBaseBuildingLogEnable()) bsbDebugPrint("[bsb] Construction " + Object.GetDebugName(m_EntityParent) + " InitBaseState");
		
		m_Parent.InitBaseState();
		UpdatePhysics();
	}
	
	override protected typename GetDefaultPartInstanceTypename()
	{
		return ConstructionPart;
	}
	
	//BB parent
	protected BaseBuildingBase GetParent()
	{
		return m_Parent;
	}
	override protected void SetParent( EntityAI parent )
	{
		super.SetParent(parent);
		
		m_Parent = BaseBuildingBase.Cast(parent);
	}
	
	//BuildPart
	override void BuildPartServerEx(Man player, string part_name, int action_id)
	{
		super.BuildPartServerEx(player,part_name,action_id);
		
		//call events, 'special' BBB handling, TODO: some parameter into BBB to allow the new handling?
		if (m_Parent)
		{
			//reset DamageZone health
			string damageZone;
			if (DamageSystem.GetDamageZoneFromComponentName(m_EntityParent,part_name,damageZone))
			{
				m_EntityParent.SetAllowDamage(true);
				m_EntityParent.SetHealthMax(damageZone);
				m_EntityParent.ProcessInvulnerabilityCheck(m_EntityParent.GetInvulnerabilityTypeString());
			}
			
			m_Parent.OnPartBuiltServer(player, part_name, action_id);
		}
		else
		{
			ProcessOnPartBuiltServer(player, part_name, action_id);
		}
	}
	
	//DismantlePart
	override void DismantlePartServerEx(Man player, string part_name, int action_id)
	{
		super.DismantlePartServerEx(player,part_name,action_id);
		
		//call events, 'special' BBB handling, TODO: some parameter into BBB to allow the new handling?
		if (m_Parent)
		{
			m_Parent.OnPartDismantledServer(player, part_name, action_id);
		}
		else
		{
			ProcessOnPartDismantledServer(player, part_name, action_id);
		}
	}
	
	//DestroyPart
	override void DestroyPartServer( Man player, string part_name, int action_id, bool destroyed_by_connected_part = false )
	{
		super.DestroyPartServer(player,part_name,action_id,destroyed_by_connected_part);
		
		//call events, 'special' BBB handling, TODO: some parameter into BBB to allow the new handling?
		if (m_Parent)
		{
			m_Parent.OnPartDestroyedServer(player, part_name, action_id);
			DestroyConnectedParts(part_name);
		}
		else
		{
			ProcessOnPartDestroyedServer(player, part_name, action_id, destroyed_by_connected_part);
		}
		
		//set DamageZone health to zero (redundant?)
		string damageZone;
		if (DamageSystem.GetDamageZoneFromComponentName(m_EntityParent,part_name,damageZone) && m_EntityParent.GetHealth(damageZone,"Health") > 0)
		{
			m_EntityParent.SetHealth(damageZone,"Health",0);
		}
	}
	
	override void OnHealthLevelChanged(int oldLevel, int newLevel, string zone)
	{
		super.OnHealthLevelChanged(oldLevel, newLevel, zone);
		
		string partName = zone;
		partName.ToLower();
		
		if (newLevel == GameConstants.STATE_RUINED)
		{
			//barbed wire handling (hack-ish)
			if (partName.Contains("barbed"))
			{
				BarbedWire barbedWire = BarbedWire.Cast(m_Parent.FindAttachmentBySlotName(zone));
				if (barbedWire)
					barbedWire.SetMountedState( false );
			}
		}
	}
	
	//!Exceptions from 'dependent parts' hierarchy are handled here
	override bool ExceptionCheck(string part_name)
	{
		if (super.ExceptionCheck(part_name))
			return true;
		
		//gate hack
		ConstructionPart part = GetConstructionPart(part_name);
		if (part.IsGate())
		{
			if( GetConstructionPart("wall_base_down").IsBuilt() || GetConstructionPart("wall_base_up").IsBuilt() )
				return true;
		}
		return false;
	}
	
	//Returns (first found) gate construction part
	ConstructionPart GetGateConstructionPart()
	{
		foreach (ConstructionPart part : m_ConstructionPartArray)
		{
			if (part.IsGate())
				return part;
		}
		
		return NULL;
	}
}

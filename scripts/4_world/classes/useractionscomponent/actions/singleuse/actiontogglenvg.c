class ActionToggleNVG : ActionBase
{
	override bool IsInstant()
	{
		return true;
	}
	
	override void CreateConditionComponents()  
	{	
		m_ConditionItem 	= new CCINone();
		m_ConditionTarget 	= new CCTNonRuined(UAMaxDistances.DEFAULT);
	}
	
	override typename GetInputType()
	{
		return ToggleNVGActionInput;
	}
	
	override bool HasTarget()
	{
		return true;
	}
	
	override bool UseMainItem()
	{
		return false;
	}
	
	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		if (target.GetObject() != null)
			return true;
		
		return false;
	}

	override void Start(ActionData action_data)
	{
		super.Start(action_data);

		CachedEquipmentStorageQuery query = new CachedEquipmentStorageQuery();
		query.m_Category = ECachedEquipmentItemCategory.NVG;
		query.m_Placement = ECachedEquipmentPlacement.ATTACHMENT;
		query.m_MaximumDepth = 2;
		
		array<Entity> nvgs = action_data.m_Player.GetCachedEquipment().GetEntitiesByCategory(query);
		foreach (Entity nvg : nvgs)
		{
			NVGoggles goggles;
			if (NVGoggles.CastTo(goggles, nvg))
			{
				goggles.RotateGoggles(goggles.m_IsLowered);
				break;
			}
		}
	}
}

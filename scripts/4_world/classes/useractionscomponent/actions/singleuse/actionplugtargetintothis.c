class ActionPlugTargetIntoThis: ActionSingleUseBase
{
	bool m_Retoggle;
	
	void ActionPlugTargetIntoThis()
	{
		m_CommandUID 	= DayZPlayerConstants.CMD_ACTIONMOD_INTERACTONCE;
		m_Retoggle 		= false;
		m_Text 			= "#connect_together";
	}
	
	override void CreateConditionComponents()  
	{
		m_ConditionItem 	= new CCINonRuined();
		m_ConditionTarget 	= new CCTNonRuined(UAMaxDistances.DEFAULT);
	}
	
	override bool Can(PlayerBase player, ActionTarget target, ItemBase item)
	{				
		if (super.Can(player, target, item))
		{		
			if (player.IsPlacingLocal())
			{				
				m_Retoggle = true;
				
				player.GetHologramLocal().SetIsHidden(true);
				player.GetHologramLocal().GetProjectionEntity().HideAllSelections();	
			}
		}
		
		if (!super.Can(player, target, item))
		{
			if (m_Retoggle)
			{				
				m_Retoggle = false;
				
				player.GetHologramLocal().SetIsHidden(false);
				player.GetHologramLocal().GetProjectionEntity().ShowAllSelections();	
			}
		}
		
		return super.Can(player, target, item);
	}
	
	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{		
		ItemBase targetIB = ItemBase.Cast(target.GetObject());
		
		//Prevent plugging to items in inventory
		if (targetIB && targetIB.GetHierarchyRoot() == targetIB && item && targetIB.HasEnergyManager() && item.HasEnergyManager())
		{
			if (!targetIB.GetCompEM().IsPlugged() && item.GetCompEM().CanReceivePlugFrom(targetIB))
			{
				//hotfix, re-work entire action!
				return targetIB.IsElectricAppliance();
			}
			
			// Special case for vehicle batteries
			if (item.IsInherited(VehicleBattery))
			{
				MetalWire metalWire = MetalWire.Cast(item.GetCompEM().GetPluggedDevice());
				
				if (metalWire && metalWire.GetCompEM().CanReceivePlugFrom(targetIB))
				{
					return true; // We can power the action_data.m_Target from vehicle battery because it has metal wire attached.
				}
			}
		}
		
		return false;
	}
	
	override bool IsDeploymentAction()
	{
		return true;
	}
	
	override void OnExecuteServer(ActionData action_data)
	{
		Process(action_data);
	}
	
	override void OnExecuteClient(ActionData action_data)
	{
		m_Retoggle = false;
	}

	void Process(ActionData action_data)
	{
		ItemBase targetIB = ItemBase.Cast(action_data.m_Target.GetObject());

		if (action_data.m_MainItem.IsInherited(VehicleBattery))
		{
			// Car/truck batteries can have a metal wire attached through which they can power common electric appliances
			MetalWire metalWire = MetalWire.Cast(action_data.m_MainItem.GetCompEM().GetPluggedDevice());
			if (metalWire)
				targetIB.GetCompEM().PlugThisInto(metalWire);
		}
		else
		{
			// Everything else in general
			targetIB.GetCompEM().PlugThisInto(action_data.m_MainItem);
		}
		
		Spotlight spotlight;
		if (Spotlight.CastTo(spotlight, targetIB))
			spotlight.Unfold();
		
		targetIB.GetInventory().TakeEntityAsAttachment(InventoryMode.LOCAL, action_data.m_MainItem);
		action_data.m_Player.ServerDropEntity(action_data.m_MainItem);
	}
}

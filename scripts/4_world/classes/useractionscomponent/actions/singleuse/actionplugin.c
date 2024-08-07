class ActionPlugIn: ActionSingleUseBase
{
	void ActionPlugIn()
	{
		m_CommandUID 	= DayZPlayerConstants.CMD_ACTIONMOD_INTERACTONCE;
		m_Text 			= "#plug_in";
	}
	
	override void CreateConditionComponents()  
	{	
		m_ConditionItem 	= new CCINonRuined();
		m_ConditionTarget 	= new CCTNonRuined(UAMaxDistances.DEFAULT);
	}

	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		if (player.IsPlacingLocal())
			return false;
		
		ItemBase targetIB = ItemBase.Cast(target.GetObject());
		
		//Prevent plugging to items in inventory
		if (targetIB && targetIB.GetHierarchyRoot() == targetIB && item)
		{
			if (item.HasEnergyManager() && !item.GetCompEM().IsPlugged() && targetIB.HasEnergyManager() && targetIB.GetCompEM().CanReceivePlugFrom(item))
			{
				return true;
			}
			
			ItemBase attachedDevice = GetAttachedDevice(targetIB);
			//Will only ever affect batteries
			if (attachedDevice)
			{
				return attachedDevice.GetCompEM().HasFreeSocket();
			}
		}
		
		return false;
	}

	override void OnExecuteServer(ActionData action_data)
	{
		ItemBase targetIB = ItemBase.Cast(action_data.m_Target.GetObject());
		
		if (targetIB.HasEnergyManager())
		{
			ItemBase attachedDevice = GetAttachedDevice(targetIB);
			if (attachedDevice)
				targetIB = attachedDevice;
			
			action_data.m_MainItem.GetCompEM().PlugThisInto(targetIB);
		
			if (!g_Game.IsDedicatedServer())
			{
				if (!action_data.m_Player.IsPlacingLocal())
				{
					action_data.m_Player.PlacingStartLocal(action_data.m_MainItem);
					Process(action_data);
				}
			}
			else
			{
				if (!action_data.m_Player.IsPlacingServer())
				{
					action_data.m_Player.PlacingStartServer(action_data.m_MainItem);
					Process(action_data);
				}
			}
		}
	}
	
	void Process(ActionData action_data)
	{
		ItemBase targetIB = ItemBase.Cast(action_data.m_Target.GetObject());

		targetIB.GetInventory().TakeEntityAsAttachment(InventoryMode.LOCAL, action_data.m_MainItem);
	}
	
	override void OnExecuteClient(ActionData action_data)
	{	
		if (!action_data.m_Player.IsPlacingLocal())
			action_data.m_Player.PlacingStartLocal(action_data.m_MainItem);
		else
			Process(action_data);
	}
	
	ItemBase GetAttachedDevice(ItemBase parent)
	{
		if (parent.IsInherited(CarBattery) || parent.IsInherited(TruckBattery))
		{
			ItemBase parentAttachment = ItemBase.Cast(parent.GetAttachmentByType(MetalWire));
			if (!parentAttachment)
				parentAttachment = ItemBase.Cast(parent.GetAttachmentByType(BarbedWire));

			return parentAttachment;
		}
		
		return null;
	}
};
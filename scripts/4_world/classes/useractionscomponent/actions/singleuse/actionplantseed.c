class ActionPlantSeed : ActionSingleUseBase
{
	void ActionPlantSeed()
	{
		m_CommandUID	= DayZPlayerConstants.CMD_ACTIONMOD_DROPITEM_HANDS;
		m_FullBody 		= false;

		m_Text = "#plant_seed";
	}
	
	override void CreateConditionComponents()  
	{		
		m_ConditionItem 	= new CCINonRuined();
		m_ConditionTarget 	= new CCTDummy();
	}

	override bool HasTarget()
	{
		return true;
	}

	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		GardenBase gardenBase = GardenBase.Cast(target.GetObject());
		
		if (gardenBase && (!gardenBase.IsHologram() || !gardenBase.IsBeingPlaced()))
		{
			array<string> selections = new array<string>();
			gardenBase.GetActionComponentNameList(target.GetComponentIndex(), selections);

			foreach (string selection : selections)
			{
				Slot slot = gardenBase.GetSlotBySelection(selection);
				if (slot)
				{
					if (item != null && item.GetQuantity() > 0 && gardenBase.CanPlantSeed(selection))
						return true;
				}
			}
		}
		
		return false;
	}

	override void OnExecuteServer(ActionData action_data)
	{
		super.OnExecuteServer(action_data);

		Process(action_data);
	}

	void Process(ActionData action_data)
	{
		GardenBase gardenBase = GardenBase.Cast(action_data.m_Target.GetObject());
		
		if (gardenBase)
		{
			int slotID;
			array<string> selections = new array<string>();
			gardenBase.GetActionComponentNameList(action_data.m_Target.GetComponentIndex(), selections);

			foreach (string selection : selections)
			{
				Slot slot = gardenBase.GetSlotBySelection(selection);
				if (slot)
				{
					slotID = slot.GetSlotId();
					break;
				}
			}
	
			ItemBase seed = ItemBase.Cast(action_data.m_MainItem);
			if (seed.GetQuantity() > 1)
				seed.SplitIntoStackMax(gardenBase, slotID, action_data.m_Player);
			else
				gardenBase.ServerTakeEntityToTargetAttachmentEx(gardenBase, seed, slotID);
		}
	}
}

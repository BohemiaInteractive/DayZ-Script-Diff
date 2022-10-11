class ActionLightItemOnFireWithBlowtorchCB : ActionLightItemOnFireCB
{
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousTime(UATimeSpent.FIREPLACE_IGNITE);
	}
}

class ActionLightItemOnFireWithBlowtorch : ActionLightItemOnFire
{
	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		Blowtorch bt = Blowtorch.Cast(item);
		if (!bt.HasEnoughEnergyForRepair(UATimeSpent.BASEBUILDING_REPAIR_FAST))
		{
			return false;
		}
		
		ItemBase target_item = ItemBase.Cast(target.GetObject());	
		if (target_item && item)
		{
			// when igniting item on the ground with igniter in hands
			if (!target_item.IsIgnited() && !IsItemInCargoOfSomething(target_item) && target_item.CanBeIgnitedBy(item))
			{
				// oven stage of standard fireplace
				if (target_item.IsKindOf("Fireplace"))
				{
					if (Fireplace.Cast(target_item).IsOven())
					{
						return true;
					}
					
					if (Fireplace.CanIgniteEntityAsFireplace(target_item))
					{
						return true;
					}

					return false;
				}
				
				return true;
			}
			// when igniting item in hands from something on ground
			else if (!item.IsIgnited() && !IsItemInCargoOfSomething(item) && target_item.CanIgniteItem(item) && item.CanBeIgnitedBy(target_item))
			{
				return true;
			}
		}
		
		return false;
	}
	
	override void OnUpdate(ActionData action_data)
	{
		super.OnUpdate(action_data);

		if (action_data.m_State == UA_PROCESSING && !action_data.m_MainItem.GetCompEM().IsWorking())
		{
			Interrupt(action_data);
		}
	}

	override void OnExecuteServer(ActionData action_data)
	{
		super.OnExecuteServer(action_data);

		action_data.m_MainItem.GetCompEM().SwitchOn();
	}
	
	override void OnFinishProgressServer(ActionData action_data)
	{
		super.OnFinishProgressServer(action_data);
		
		action_data.m_MainItem.GetCompEM().SwitchOff();
	}
	
	override void OnEndServer(ActionData action_data)
	{
		super.OnEndServer(action_data);
		
		action_data.m_MainItem.GetCompEM().SwitchOff();
	}
}
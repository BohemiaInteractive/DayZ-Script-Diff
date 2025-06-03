class ActionWaterGardenSlotCB : ActionContinuousBaseCB
{
	private const float QUANTITY_USED_PER_SEC = 150;
	
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousWaterSlot(QUANTITY_USED_PER_SEC);
	}
};

class ActionWaterGardenSlot: ActionContinuousBase
{
	void ActionWaterGardenSlot()
	{
		m_CallbackClass = ActionWaterGardenSlotCB;
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONFB_EMPTY_VESSEL;
		m_FullBody = true;
		m_SpecialtyWeight = UASoftSkillsWeight.ROUGH_LOW;
		m_Text = "#water_slot";
	}
	
	override void CreateConditionComponents()  
	{
		m_ConditionTarget = new CCTDummy;
		m_ConditionItem = new CCINonRuined;
	}
	
	override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
	{
		if (item.GetQuantity() == 0)
			return false;
		
		// Get the liquid
		int liquidType	= item.GetLiquidType();
		if (liquidType != LIQUID_WATER)
		{
			return false; //  Forbid watering of plants with gasoline and other fluids
		}
		
		if (item.GetIsFrozen())
		{
			return false;
		}
		
		Object targetObject = target.GetObject();
		if (targetObject && targetObject.IsInherited(GardenBase))
		{
			GardenBase gardenBase = GardenBase.Cast(targetObject);			
			array<string> selections = new array<string>;
			targetObject.GetActionComponentNameList(target.GetComponentIndex(), selections);
			
			Slot slot;
			for (int s = 0; s < selections.Count(); s++)
			{
				string selection = selections[s];
				slot = gardenBase.GetSlotBySelection(selection);
				if (slot)
					break;
			}
					
			if ( slot && !slot.GetPlant() && slot.GetWateredState() == eWateredState.DRY )
			{
				return true;
			}
			else
			{
				return false;
			}
		}
		
		return false;
	}
};
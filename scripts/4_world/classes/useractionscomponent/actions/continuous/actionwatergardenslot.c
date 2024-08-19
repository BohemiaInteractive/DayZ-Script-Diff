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
	
	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{
		Object targetObject = target.GetObject();
		
		if (item.GetQuantity() == 0)
			return false;
		
		// Get the liquid
		int liquid_type	= item.GetLiquidType();

		if (liquid_type != LIQUID_WATER)
		{
			return false; //  Forbid watering of plants with gasoline and other fluids
		}
		
		if ( targetObject.IsInherited(GardenBase) )
		{
			GardenBase garden_base = GardenBase.Cast( targetObject );
			
			Slot slot;
			
			array<string> selections = new array<string>;
			targetObject.GetActionComponentNameList(target.GetComponentIndex(), selections);

			for (int s = 0; s < selections.Count(); s++)
			{
				string selection = selections[s];
				slot = garden_base.GetSlotBySelection( selection );
				if (slot)
					break;
			}
		
			if ( slot && !slot.GetPlant() && slot.CanBeWatered() && slot.GetWateredState() == 0 )
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
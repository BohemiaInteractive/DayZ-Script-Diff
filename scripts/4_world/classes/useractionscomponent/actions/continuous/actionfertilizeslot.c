class ActionFertilizeSlotCB : ActionContinuousBaseCB
{
	private const float QUANTITY_USED_PER_SEC = 150;
	
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousFertilizeGardenSlot(QUANTITY_USED_PER_SEC);
	}
};

class ActionFertilizeSlot: ActionContinuousBase
{
	void ActionFertilizeSlot()
	{
		m_CallbackClass = ActionFertilizeSlotCB;
		m_SpecialtyWeight = UASoftSkillsWeight.ROUGH_MEDIUM;
		
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONFB_INTERACT;
		m_FullBody = true;
		m_StanceMask = DayZPlayerConstants.STANCEMASK_CROUCH;
		m_Text = "#fertilize_slot";
	}
	
	override void CreateConditionComponents()  
	{
		m_ConditionTarget = new CCTDummy;
		m_ConditionItem = new CCINonRuined;
	}


	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{
		if (item.GetQuantity() == 0)
			return false;
		
		Object targetObject = target.GetObject();
		if (targetObject.IsInherited(GardenBase))
		{
			GardenBase gardenBase = GardenBase.Cast(targetObject);
			Slot slot;
			
			array<string> selections = new array<string>;
			gardenBase.GetActionComponentNameList(target.GetComponentIndex(), selections);

			foreach(string selection: selections)
			{
				slot = gardenBase.GetSlotBySelection(selection);
				if (slot)
					break;
			}
			
			if (slot)
			{
				if (slot.GetPlant())
					return false;
								
				if (slot.GetFertilityState() == eFertlityState.NONE)
					return true;
			}
		}

		return false;
	}
	
	override void OnFinishProgressServer( ActionData action_data )
	{
		// The functionality is in the Execute event of this user action's component.
	}
};
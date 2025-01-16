class ActionHarvestCropsCB : ActionContinuousBaseCB
{
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousTime(UATimeSpent.DEFAULT_HARVEST);
	}
};

class ActionHarvestCrops: ActionContinuousBase
{
	PlantBase m_Plant;
	
	void ActionHarvestCrops()
	{
		m_CallbackClass = ActionHarvestCropsCB;
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONFB_INTERACT;
		m_FullBody = true;
		m_StanceMask = DayZPlayerConstants.STANCEMASK_ERECT | DayZPlayerConstants.STANCEMASK_CROUCH;
	}

	override typename GetInputType()
	{
		return ContinuousInteractActionInput;
	}
	
	Slot GetPlantSlot(ActionTarget target)
	{
		GardenBase garden_base;
		if ( Class.CastTo(garden_base, target.GetObject()))
		{
			Slot slot;
			
			array<string> selections = new array<string>;
			garden_base.GetActionComponentNameList(target.GetComponentIndex(), selections);
			string selection;

			for (int s = 0; s < selections.Count(); s++)
			{
				selection = selections[s];
				slot = garden_base.GetSlotBySelection( selection );
				if (slot)
					break;
			}
			
			if ( slot && slot.GetPlant() )
			{
				return slot;	
			}
		}
		return null;
	}
	
	override void OnActionInfoUpdate( PlayerBase player, ActionTarget target, ItemBase item )
	{
		m_Text =  "#harvest";
		
		Slot slot = GetPlantSlot(target);
		
		if (slot)
		{
			m_Plant = PlantBase.Cast(slot.GetPlant());
			m_Text+=  " " +  MiscGameplayFunctions.GetItemDisplayName(m_Plant.GetCropsType());
		}

	}

	override void CreateConditionComponents()  
	{
		m_ConditionItem = new CCINone;
		m_ConditionTarget = new CCTCursor(UAMaxDistances.SMALL);
	}
	
	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{
		GardenBase garden_base;
		if ( Class.CastTo(garden_base, target.GetObject()))
		{
			Slot slot = GetPlantSlot(target);
			
			if (slot)
			{
				m_Plant = PlantBase.Cast(slot.GetPlant());
				if ( m_Plant && m_Plant.IsHarvestable())
					return true;
			}
		}
		return false;

	}

	override void OnFinishProgressServer( ActionData action_data )
	{
		if ( m_Plant )
		{
			m_Plant.Harvest( action_data.m_Player );
		}
	}
};
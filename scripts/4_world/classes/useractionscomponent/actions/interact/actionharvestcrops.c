class ActionHarvestCrops: ActionInteractBase
{
	PlantBase m_Plant;
	
	void ActionHarvestCrops()
	{

		m_SpecialtyWeight = UASoftSkillsWeight.PRECISE_MEDIUM;
		/*m_CommandUID = DayZPlayerConstants.CMD_ACTIONFB_INTERACT;
		m_StanceMask = DayZPlayerConstants.STANCEMASK_CROUCH | DayZPlayerConstants.STANCEMASK_ERECT;
		m_FullBody = true;*/
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
				bool is_mature = m_Plant.IsMature();
				bool is_spoiled = m_Plant.IsSpoiled();
				bool has_crops = m_Plant.HasCrops();
				int plant_state = m_Plant.GetPlantState();

				if ( is_mature && has_crops )
				{
					return true;
				}
			}
		}
		return false;

	}

	override void OnExecuteServer( ActionData action_data )
	{
		if ( m_Plant )
		{
			m_Plant.Harvest( action_data.m_Player );

			action_data.m_Player.GetSoftSkillsManager().AddSpecialty( m_SpecialtyWeight );
		}
	}
};
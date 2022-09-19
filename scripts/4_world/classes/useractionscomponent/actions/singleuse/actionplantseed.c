class ActionPlantSeed: ActionSingleUseBase
{
	void ActionPlantSeed()
	{
		m_SpecialtyWeight = UASoftSkillsWeight.PRECISE_MEDIUM;
		
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONMOD_DROPITEM_HANDS;
		m_FullBody = false;
		m_Text = "#plant_seed";
	}
	
	override void CreateConditionComponents()  
	{		
		m_ConditionItem = new CCINonRuined;
		m_ConditionTarget = new CCTDummy;
	}

	override bool HasTarget()
	{
		return true;
	}

	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{
		GardenBase targetObject = GardenBase.Cast( target.GetObject() );
		
		if ( targetObject && ( !targetObject.IsHologram() || !targetObject.IsBeingPlaced() ) )
		{
			array<string> selections = new array<string>;
			targetObject.GetActionComponentNameList(target.GetComponentIndex(), selections);

			for (int s = 0; s < selections.Count(); s++)
			{
				string selection = selections[s];
				Slot slot = targetObject.GetSlotBySelection( selection );
				if (slot)
				{
					if ( item != NULL && item.GetQuantity() > 0 && targetObject.CanPlantSeed( selection ) )
					{
						return true;
					}
				}
			}
		}
		
		return false;
	}

	override void OnExecuteServer( ActionData action_data )
	{
		Process(action_data);
	}
	
	override void OnExecuteClient( ActionData action_data )
	{
		//Process(action_data);
	}

	void Process( ActionData action_data )
	{
		Object targetObject = action_data.m_Target.GetObject();
		int slot_ID;
		
		if ( targetObject != NULL && targetObject.IsInherited(GardenBase) )
		{
			GardenBase garden_base = GardenBase.Cast( targetObject );
			array<string> selections = new array<string>;
			targetObject.GetActionComponentNameList(action_data.m_Target.GetComponentIndex(), selections);

			for (int s = 0; s < selections.Count(); s++)
			{
				string selection = selections[s];
				Slot slot = garden_base.GetSlotBySelection( selection );
				if (slot)
				{
					slot_ID = slot.GetSlotId();
					break;
				}
			}
			
			//int slot_ID = slot.GetSlotId();
			
			ItemBase seed_IB = ItemBase.Cast( action_data.m_MainItem );
			
			seed_IB.SplitIntoStackMax( garden_base, slot_ID, action_data.m_Player );
		}

		action_data.m_Player.GetSoftSkillsManager().AddSpecialty( m_SpecialtyWeight );
	}
};
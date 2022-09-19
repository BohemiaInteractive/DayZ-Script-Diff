class ActionBuildOvenCB : ActionContinuousBaseCB
{
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousTime( UATimeSpent.DEFAULT_CONSTRUCT );
	}
}

class ActionBuildOven: ActionContinuousBase
{
	void ActionBuildOven()
	{
		m_CallbackClass = ActionBuildOvenCB;
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONFB_CRAFTING;
		m_FullBody = true;
		m_StanceMask = DayZPlayerConstants.STANCEMASK_CROUCH;
		m_SpecialtyWeight = UASoftSkillsWeight.ROUGH_HIGH;
		m_Text = "#build_oven";
	}
	
	override void CreateConditionComponents()  
	{	
		
		m_ConditionTarget = new CCTNonRuined( UAMaxDistances.DEFAULT );
		m_ConditionItem = new CCINotPresent;
	}
	
	override typename GetInputType()
	{
		return ContinuousInteractActionInput;
	}

	override bool HasProgress()
	{
		return true;
	}

	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{	
		//Action not allowed if player has broken legs
		if (player.GetBrokenLegs() == eBrokenLegs.BROKEN_LEGS)
			return false;
		
		Object target_object = target.GetObject();
			
		if ( target_object && target_object.IsFireplace() )
		{
			FireplaceBase fireplace_target = FireplaceBase.Cast( target_object );
			
			if ( fireplace_target.IsBaseFireplace() && fireplace_target.CanBuildOven() )
			{
				return true;
			}
		}
		
		return false;
	}
		
	override void OnFinishProgressServer( ActionData action_data )
	{	
		Object target_object = action_data.m_Target.GetObject();
		FireplaceBase fireplace_target = FireplaceBase.Cast( target_object );
		
		if ( fireplace_target.CanBuildOven() )
		{
			ItemBase attached_item = ItemBase.Cast( fireplace_target.GetAttachmentByType( fireplace_target.ATTACHMENT_STONES ) );
			
			/*InventoryLocation inventory_location = new InventoryLocation;
			attached_item.GetInventory().GetCurrentInventoryLocation( inventory_location );
			fireplace_target.GetInventory().SetSlotLock( inventory_location.GetSlot(), true );*/
			
			//set oven state
			fireplace_target.SetOvenState( true );
			fireplace_target.SetOrientation( action_data.m_Player.GetOrientation() - "180 0 0" );
			
			// extend lifetime
			fireplace_target.SetLifetimeMax( FireplaceBase.LIFETIME_FIREPLACE_STONE_OVEN );

			//add specialty to soft skills
			action_data.m_Player.GetSoftSkillsManager().AddSpecialty( m_SpecialtyWeight );
		}
	}
}
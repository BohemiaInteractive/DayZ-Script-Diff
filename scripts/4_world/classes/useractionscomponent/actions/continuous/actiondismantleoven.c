class ActionDismantleOvenCB : ActionContinuousBaseCB
{
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousTime( UATimeSpent.DEFAULT_DECONSTRUCT );
	}
}

class ActionDismantleOven: ActionContinuousBase
{	
	void ActionDismantleOven()
	{
		m_CallbackClass = ActionDismantleOvenCB;
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONFB_CRAFTING;
		m_FullBody = true;
		m_StanceMask = DayZPlayerConstants.STANCEMASK_CROUCH;
		m_SpecialtyWeight = UASoftSkillsWeight.ROUGH_HIGH;
		
		m_Text = "#dismantle_oven";
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

	/*override bool HasProgress()
	{
		return false;
	}*/

	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{	
		//Action not allowed if player has broken legs
		if (player.GetBrokenLegs() == eBrokenLegs.BROKEN_LEGS)
			return false;
		
		Object target_object = target.GetObject();
		
		if ( target_object && target_object.IsFireplace() )
		{
			FireplaceBase fireplace_target = FireplaceBase.Cast( target_object );
			
			if ( fireplace_target.IsBaseFireplace() && fireplace_target.CanDismantleOven() )
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
		
		if ( fireplace_target.CanDismantleOven() )
		{
			ItemBase attached_item = ItemBase.Cast( fireplace_target.GetAttachmentByType( fireplace_target.ATTACHMENT_STONES ) );
			
			// for backward compatibility - for the cases built before slot locking was commented out for the stone att slot
			InventoryLocation inventory_location = new InventoryLocation;
			attached_item.GetInventory().GetCurrentInventoryLocation( inventory_location );			
			fireplace_target.GetInventory().SetSlotLock( inventory_location.GetSlot(), false );

			//set oven state
			fireplace_target.SetOvenState( false );
			
			// extend lifetime (either back to stone circle lifetime or standard fireplace one)
			if ( fireplace_target.HasStoneCircle() )
			{
				fireplace_target.SetLifetimeMax( FireplaceBase.LIFETIME_FIREPLACE_STONE_CIRCLE );
			}
			else
			{
				fireplace_target.SetLifetimeMax( 10800 );
			}
			
			//add specialty to soft skills
			action_data.m_Player.GetSoftSkillsManager().AddSpecialty( m_SpecialtyWeight );
		}
	}
}
class ActionBuildStoneCircleCB : ActionContinuousBaseCB
{
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousTime( UATimeSpent.DEFAULT_CONSTRUCT );
	}
}

class ActionBuildStoneCircle: ActionContinuousBase
{
	void ActionBuildStoneCircle()
	{
		m_CallbackClass = ActionBuildStoneCircleCB;
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONFB_CRAFTING;
		m_FullBody = true;
		m_StanceMask = DayZPlayerConstants.STANCEMASK_CROUCH;
		m_SpecialtyWeight = UASoftSkillsWeight.ROUGH_HIGH;
		m_Text = "#build_stonecircle";
		
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
		Object target_object = target.GetObject();
			
		if ( target_object && target_object.IsFireplace() )
		{
			FireplaceBase fireplace_target = FireplaceBase.Cast( target_object );
			
			if ( fireplace_target.IsBaseFireplace() && fireplace_target.CanBuildStoneCircle() )
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
		
		if ( fireplace_target.CanBuildStoneCircle() )
		{
			ItemBase attached_item = ItemBase.Cast( fireplace_target.GetAttachmentByType( fireplace_target.ATTACHMENT_STONES ) );
			
			/*InventoryLocation inventory_location = new InventoryLocation;
			attached_item.GetInventory().GetCurrentInventoryLocation( inventory_location );
			fireplace_target.GetInventory().SetSlotLock( inventory_location.GetSlot(), true );*/
			
			//set oven state
			fireplace_target.SetStoneCircleState( true );
			
			// extend lifetime
			fireplace_target.SetLifetimeMax( FireplaceBase.LIFETIME_FIREPLACE_STONE_CIRCLE );

			//add specialty to soft skills
			action_data.m_Player.GetSoftSkillsManager().AddSpecialty( m_SpecialtyWeight );
		}
	}
}
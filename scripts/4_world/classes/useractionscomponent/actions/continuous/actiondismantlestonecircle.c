class ActionDismantleStoneCircleCB : ActionContinuousBaseCB
{
	override void CreateActionComponent()
	{
		m_ActionData.m_ActionComponent = new CAContinuousTime( UATimeSpent.DEFAULT_DECONSTRUCT );
	}
}

class ActionDismantleStoneCircle: ActionContinuousBase
{	
	void ActionDismantleStoneCircle()
	{
		m_CallbackClass = ActionDismantleStoneCircleCB;
		m_CommandUID = DayZPlayerConstants.CMD_ACTIONFB_CRAFTING;
		m_FullBody = true;
		m_StanceMask = DayZPlayerConstants.STANCEMASK_CROUCH;
		m_SpecialtyWeight = UASoftSkillsWeight.ROUGH_HIGH;
		m_Text = "#dismantle_stonecircle";
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
		Object target_object = target.GetObject();
		
		if ( target_object && target_object.IsFireplace() )
		{
			FireplaceBase fireplace_target = FireplaceBase.Cast( target_object );
			
			if ( fireplace_target.IsBaseFireplace() && fireplace_target.CanDismantleStoneCircle() )
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
		
		if ( fireplace_target.CanDismantleStoneCircle() )
		{
			ItemBase attached_item = ItemBase.Cast( fireplace_target.GetAttachmentByType( fireplace_target.ATTACHMENT_STONES ) );

			//set oven state
			fireplace_target.SetStoneCircleState( false );
			
			// extend lifetime (but back to default of CE)
			fireplace_target.SetLifetimeMax( 10800 );
			
			//add specialty to soft skills
			action_data.m_Player.GetSoftSkillsManager().AddSpecialty( m_SpecialtyWeight );
		}
	}
}

class ActionDetachPowerSourceFromPanel: ActionInteractBase
{
	void ActionDetachPowerSourceFromPanel()
	{
		m_Text = "#detach_power_source";
	}
	
	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{
		Object target_object = target.GetObject();
		
		if ( player && target_object )
		{
			EntityAI target_entity = EntityAI.Cast( target.GetObject() );
			string selection = target_object.GetActionComponentName( target.GetComponentIndex() );
			
			
			if ( target_entity && target_entity.GetInventory() && target_entity.GetInventory().AttachmentCount() > 0 && selection == "power_source" )	//has any power source attachment attached
			{
				return true;
			}
		}
		
		return false;
	}
	
	override void OnExecuteClient( ActionData action_data )
	{
		Process(action_data);
	}
	
	override void OnExecuteServer( ActionData action_data )
	{
		Process(action_data);
	}

	void Process( ActionData action_data )
	{
		EntityAI target_entity = EntityAI.Cast( action_data.m_Target.GetObject() );
		
		EntityAI attachment;
		int attachments_count = target_entity.GetInventory().AttachmentCount();
		for ( int j = 0; j < attachments_count; j++ )				//find any attached power source
		{
			// @TODO: vezme prvni att?
			attachment = target_entity.GetInventory().GetAttachmentFromIndex( j );
			if ( attachment )
				break;
		}
		
		if ( attachment )
		{
			action_data.m_Player.PredictiveTakeEntityToHands(attachment );
		}
	}
}
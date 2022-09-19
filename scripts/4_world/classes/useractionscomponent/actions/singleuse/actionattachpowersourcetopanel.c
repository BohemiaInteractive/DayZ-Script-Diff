//!DEPRECATED
class ActionAttachPowerSourceToPanel: ActionSingleUseBase
{
	void ActionAttachPowerSourceToPanel()
	{
		m_Text = "#attach";
	}

	override void CreateConditionComponents() 
	{
		m_ConditionItem = new CCINonRuined;
		m_ConditionTarget = new CCTNonRuined(UAMaxDistances.DEFAULT);
	}
	
	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{
		EntityAI target_entity = EntityAI.Cast( target.GetObject() );
		if ( player && target_entity && item )
		{			
			if ( target_entity.IsStaticTransmitter() && target_entity.GetInventory().AttachmentCount() == 0 )	//has any power source attachment attached
			{
				return true;
			}
		}
		
		return false;
	}

	protected void OnExecuteImpl( ActionData action_data )
	{
		EntityAI target_entity = EntityAI.Cast( action_data.m_Target.GetObject() );
		EntityAI item_entity = action_data.m_MainItem;
		
		//find inventory location for attachment
		InventoryLocation target_location = new InventoryLocation;
		if ( target_entity.GetInventory().FindFirstFreeLocationForNewEntity( item_entity.GetType(), FindInventoryLocationType.ATTACHMENT, target_location ) )
		{
			action_data.m_Player.PredictiveTakeEntityToTargetAttachmentEx( target_entity, item_entity, target_location.GetSlot() );
		}
	}

	override void OnExecuteClient( ActionData action_data )
	{
		ClearInventoryReservationEx(action_data);
		OnExecuteImpl(action_data);
	}
	
	override void OnExecuteServer( ActionData action_data )
	{
		if (GetGame().IsMultiplayer())
			return; // multiplayer handled on client side via OnExecuteClient
		else
			OnExecuteImpl(action_data); // single player
	}
	
	override void OnEndClient( ActionData action_data )
	{
		// Probably not needed since attaching is done server side.
		/*
		EntityAI target_entity = EntityAI.Cast( action_data.m_Target.GetObject() );
		EntityAI item_entity = EntityAI.Cast( action_data.m_MainItem );
		
		//find inventory location for attachment
		InventoryLocation target_location = new InventoryLocation;
		
		if( target_entity.GetInventory().FindFirstFreeLocationForNewEntity( item_entity.GetType(), FindInventoryLocationType.ATTACHMENT, target_location ) )
		{
			//target_entity.PredictiveTakeEntityToTargetAttachmentEx( target_entity, item_entity, target_location.GetSlot() );
			target_entity.LocalTakeEntityAsAttachmentEx( item_entity, target_location.GetSlot() );
		}
		*/
	}
}
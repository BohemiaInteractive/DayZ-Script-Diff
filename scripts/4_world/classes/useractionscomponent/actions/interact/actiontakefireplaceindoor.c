class ActionTakeFireplaceIndoor: ActionInteractBase
{
	string m_NewItemTypeName = "Fireplace";

	void ActionTakeFireplaceIndoor()
	{
		m_CommandUID        = DayZPlayerConstants.CMD_ACTIONMOD_PICKUP_HANDS;
		m_StanceMask        = DayZPlayerConstants.STANCEMASK_CROUCH | DayZPlayerConstants.STANCEMASK_ERECT;
		m_Text = "#take_fireplace";
	}
	
	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{
		Object target_object = target.GetObject();
		
		//empty hands	--interact action base condition
		if ( target_object && target_object.IsFireplace() )
		{
			FireplaceIndoor fireplace_indoor = FireplaceIndoor.Cast( target_object );
	
			if ( fireplace_indoor && !fireplace_indoor.HasAshes() && !fireplace_indoor.IsBurning() && fireplace_indoor.IsCargoEmpty() && !fireplace_indoor.DirectCookingSlotsInUse() )
			{
				InventoryLocation targetIL = new InventoryLocation;
				bool found = player.GetInventory().FindFirstFreeLocationForNewEntity( m_NewItemTypeName, FindInventoryLocationType.ANY, targetIL );
				return found;
			}
		}

		return false;
	}

	override void OnExecuteServer( ActionData action_data )
	{
		Object target_object = action_data.m_Target.GetObject();
		FireplaceIndoor fireplace_indoor = FireplaceIndoor.Cast( target_object );
		
		TakeFireplaceFromIndoorLambda lambda( fireplace_indoor, m_NewItemTypeName, action_data.m_Player );
		InventoryLocation targetIL = new InventoryLocation;
		bool found = action_data.m_Player.GetInventory().FindFirstFreeLocationForNewEntity( m_NewItemTypeName, FindInventoryLocationType.ANY, targetIL );
		if ( found )
		{
			// allow action only if there is place in inventory
			lambda.OverrideNewLocation( targetIL );
			action_data.m_Player.ServerReplaceItemWithNew( lambda );
		}
	}
}

class TakeFireplaceFromIndoorLambda : ReplaceItemWithNewLambdaBase
{
	PlayerBase m_Player;

	void TakeFireplaceFromIndoorLambda ( EntityAI old_item, string new_item_type, PlayerBase player )
	{
		m_Player = player;
	}

	override void CopyOldPropertiesToNew( notnull EntityAI old_item, EntityAI new_item )
	{
		super.CopyOldPropertiesToNew( old_item, new_item );

		MiscGameplayFunctions.TransferInventory(old_item, new_item, m_Player);		
	}
}
class ActionTakeFireplaceFromBarrel: ActionInteractBase
{
	void ActionTakeFireplaceFromBarrel()
	{
		m_CommandUID        = DayZPlayerConstants.CMD_ACTIONMOD_PICKUP_HANDS;
		m_StanceMask        = DayZPlayerConstants.STANCEMASK_CROUCH | DayZPlayerConstants.STANCEMASK_ERECT;
		m_Text = "#take_fireplace";
	}
	
	override bool ActionCondition( PlayerBase player, ActionTarget target, ItemBase item )
	{
		Object target_object = target.GetObject();
		
		if ( target_object && target_object.IsFireplace() )
		{
			BarrelHoles_ColorBase fireplace_barrel = BarrelHoles_ColorBase.Cast( target_object );
			
			//check barrel fireplace state
			if ( fireplace_barrel && fireplace_barrel.IsOpen() && !fireplace_barrel.HasAshes() && !fireplace_barrel.IsBurning() )
			{
				//check cargo and attachments
				if ( fireplace_barrel.IsCargoEmpty() && fireplace_barrel.GetInventory().AttachmentCount() > 0 )
				{
					return true;
				}
			}
		}

		return false;
	}

	override void OnExecuteServer( ActionData action_data )
	{
		Object target_object = action_data.m_Target.GetObject();
		BarrelHoles_ColorBase fireplace_barrel = BarrelHoles_ColorBase.Cast( target_object );

		string newTypeName = "Fireplace";		
		InventoryLocation targetIL = new InventoryLocation;
		bool found = action_data.m_Player.GetInventory().FindFirstFreeLocationForNewEntity( newTypeName, FindInventoryLocationType.ANY, targetIL );
		if ( found )
		{
			// allow action only if there is place in inventory
			auto lambda = TakeFireplaceFromBarrelLambda( fireplace_barrel, newTypeName, action_data.m_Player );
			lambda.OverrideNewLocation( targetIL );
			action_data.m_Player.ServerReplaceItemWithNew( lambda );
		}
	}
}

class TakeFireplaceFromBarrelLambda : ReplaceItemWithNewLambdaBase
{
	PlayerBase m_Player;

	void TakeFireplaceFromBarrelLambda ( EntityAI old_item, string new_item_type, PlayerBase player )
	{
		m_Player = player;
	}
	
	override protected void RemoveOldItemFromLocation ()
	{
		// intentional no-operation
		m_RemoveFromLocationPassed = true; // but indicate the operation to be success
	}
	override protected void UndoRemoveOldItemFromLocation ()
	{
		// undo nothing
	}

	override void CopyOldPropertiesToNew( notnull EntityAI old_item, EntityAI new_item )
	{
		super.CopyOldPropertiesToNew( old_item, new_item );

		MiscGameplayFunctions.TransferInventory(old_item, new_item, m_Player);		
	}
	
	override protected void DeleteOldEntity ()
	{
		// intentional no-operation
	}
	
	override protected void CreateNetworkObjectInfo (EntityAI new_item)
	{
		super.CreateNetworkObjectInfo(new_item);
		GetGame().RemoteObjectTreeCreate(m_OldItem); // re-create network for old item 
	}
}
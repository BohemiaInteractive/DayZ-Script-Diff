class HandDrillKit extends ItemBase
{
	//! Override this method and and check if the given item can be ignited. It is not necesarry to check here if the item is not in cargo of something.
	override bool CanIgniteItem( EntityAI ignite_target = NULL )
	{
		return true;
	}
	
	override void OnIgnitedTarget( EntityAI ignited_item )
	{
		if ( GetGame().IsServer() )
		{
			DecreaseHealth( 20 );
		}
	}	
	
	override void OnIgnitedTargetFailed( EntityAI target_item )
	{
		if ( GetGame().IsServer() )
		{
			DecreaseHealth( 20 );
		}
	}
	
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionLightItemOnFire);
	}
	
	/*
	override bool IsTargetIgnitionSuccessful( EntityAI item_target = NULL )
	{
	}
	*/	
}
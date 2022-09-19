class PetrolLighter extends ItemBase
{
	override bool CanIgniteItem( EntityAI ignite_target = NULL )
	{
		if ( GetQuantity() > 0 )
			return true;
		else
			return false;
	}
	
	override void OnIgnitedTarget( EntityAI ignited_item )
	{
		if ( GetGame().IsServer() )
		{
			AddQuantity( -0.5 );
		}
	}
	
	override void OnIgnitedTargetFailed( EntityAI target_item )
	{
		if ( GetGame().IsServer() )
		{
			AddQuantity( -0.5 );
		}
	}

	/*
	override bool IsTargetIgnitionSuccessful( EntityAI item_target = NULL )
	{
	}
	*/
	
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionLightItemOnFire);
	}
}
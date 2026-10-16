class PileOfBricks extends ItemBase
{	
	void PileOfBricks()
	{
		if ( GetGame().IsServer() )
		{
			SetAllowDamage(false);
		}
	}
	
	// Shows/Hides selections. Call this in init or after every quantity change.
	void UpdateSelections()
	{
		RemoveProxyPhysics( "stage_1" );
		RemoveProxyPhysics( "stage_2" );
		RemoveProxyPhysics( "stage_3" );
		
		// Show/Hide selections according to the current quantity
		if ( this )
		{
			SetAnimationPhase("stages", 1 - GetQuantityNormalized() );
			
			if ( GetQuantityNormalized() > 0.67 )
			{	
				AddProxyPhysics( "stage_1" );
				AddProxyPhysics( "stage_2" );			
				AddProxyPhysics( "stage_3" );
			}			
			else if ( GetQuantityNormalized() > 0.34  &&  GetQuantityNormalized() <= 0.67 )
			{				
				AddProxyPhysics( "stage_2" );
				AddProxyPhysics( "stage_3" );
			}			
			else
			{
				AddProxyPhysics( "stage_3" );
			}
		}
	}
	
	override void EEItemLocationChanged(notnull InventoryLocation oldLoc, notnull InventoryLocation newLoc)
	{
		super.EEItemLocationChanged(oldLoc, newLoc);
		UpdateSelections();
	}
	
	// Removes a number of bricks
	// Returns the number of bricks
	int RemoveBricks( int needed_brickds )
	{
		int available_bricks = needed_brickds;
		if ( available_bricks > GetQuantity() )
		{
			available_bricks = GetQuantity();
		}
		
		// Remove long planks from this object
		AddQuantity( -available_bricks, true ); // Autodelete enabled
		
		// Return the number of removed long planks
		return available_bricks;
	}
	
	override bool SetQuantity(float value, bool destroy_config = true, bool destroy_forced = false, bool allow_client = false, bool clamp_to_stack_max = true)
	{
		bool result = super.SetQuantity(value, destroy_config, destroy_forced, allow_client, clamp_to_stack_max);
		UpdateSelections();
		
		return result;
	}
	
	override bool CanPutIntoHands( EntityAI parent )
	{
		return false;
	}
	
	override bool CanPutInCargo (EntityAI parent)
	{
		return false;
	}
}
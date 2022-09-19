class PlateCarrierVest: Clothing
{
	override void OnWasAttached( EntityAI parent, int slot_id )
	{
		super.OnWasAttached( parent, slot_id );
		
		if ( GetGame().IsServer() && parent.IsInherited( DayZInfected ) )
		{
			float coef = Math.RandomFloatInclusive( 0.2, 0.4 );
			SetHealth01( "", "", coef);
		}
	}
	
	//Debug menu Spawn Ground Special
	override void OnDebugSpawn()
	{
		EntityAI entity;
		if ( Class.CastTo(entity, this) )
		{
			entity.GetInventory().CreateInInventory( "PlateCarrierPouches" );
			entity.GetInventory().CreateInInventory( "PlateCarrierHolster" );
		}
	}
};
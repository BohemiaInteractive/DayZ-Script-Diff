class PlateCarrierVest: Vest_Base
{
	override void OnWasAttached( EntityAI parent, int slot_id )
	{
		super.OnWasAttached( parent, slot_id );
		
		if ( g_Game.IsServer() && parent.IsInherited( DayZInfected ) )
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
class PlateCarrierVest_Black extends PlateCarrierVest {};
class PlateCarrierVest_Camo extends PlateCarrierVest {};
class PlateCarrierVest_Green extends PlateCarrierVest {};
class PlateCarrierVest_Winter extends PlateCarrierVest {};
class CivilianBelt: Clothing
{
	override bool CanPutInCargo( EntityAI parent )
	{
		if( !super.CanPutInCargo( parent ) )
		{
			return false;
		}
		
		return IsEmpty();
	}
	
	override bool CanReceiveAttachment( EntityAI attachment,int slotId )
	{
		if( !super.CanReceiveAttachment( attachment, slotId ) )
		{
			return false;
		}
		
		return !GetInventory().IsInCargo();
	}
		
			
	//Debug menu Spawn Ground Special
	override void OnDebugSpawn()
	{
		EntityAI entity;
		if ( Class.CastTo(entity, this) )
		{
			entity.GetInventory().CreateInInventory( "NylonKnifeSheath" );
			entity.GetInventory().CreateInInventory( "PlateCarrierHolster" );
			entity.GetInventory().CreateInInventory( "Canteen" );
		}
	}
};

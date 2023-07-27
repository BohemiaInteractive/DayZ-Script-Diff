class HipPack_ColorBase : Belt_Base 
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
			entity.GetInventory().CreateInInventory( "Canteen" );
		}
	}
};
class HipPack_Black : HipPack_ColorBase {};
class HipPack_Green : HipPack_ColorBase {};
class HipPack_Medical : HipPack_ColorBase {};
class HipPack_Party : HipPack_ColorBase {};
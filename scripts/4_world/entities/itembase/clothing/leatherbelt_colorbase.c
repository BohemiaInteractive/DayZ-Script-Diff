class LeatherBelt_ColorBase extends Clothing
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
}
class LeatherBelt_Beige extends LeatherBelt_ColorBase {};
class LeatherBelt_Natural extends LeatherBelt_ColorBase {};
class LeatherBelt_Brown extends LeatherBelt_ColorBase {};
class LeatherBelt_Black extends LeatherBelt_ColorBase {};
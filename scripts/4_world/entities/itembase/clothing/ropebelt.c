class RopeBelt: Clothing
{		
	override void SetActions()
	{
		super.SetActions();
		
		AddAction(ActionDeCraftRopeBelt);
	}
	
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
};

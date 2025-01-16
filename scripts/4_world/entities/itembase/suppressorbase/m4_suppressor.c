class M4_Suppressor extends SuppressorBase
{
	override bool CanPutAsAttachment( EntityAI parent )
	{
		if (!super.CanPutAsAttachment(parent)) 
			return false;
		
		if (parent.IsSlotReserved(InventorySlots.GetSlotIdFromString("suppressorImpro")) || parent.IsSlotReserved(InventorySlots.GetSlotIdFromString("weaponBayonet")))
			return false;
		
		if ( parent.FindAttachmentBySlotName("suppressorImpro") == NULL && parent.FindAttachmentBySlotName("weaponBayonet") == NULL )
		{
			return true;
		}
		return false;
	}
}
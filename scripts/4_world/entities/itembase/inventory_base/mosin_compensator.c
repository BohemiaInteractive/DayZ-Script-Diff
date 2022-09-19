class Mosin_Compensator extends ItemSuppressor
{
	override bool CanPutAsAttachment( EntityAI parent )
	{
		if(!super.CanPutAsAttachment(parent)) {return false;}
		if ( parent.FindAttachmentBySlotName("suppressorImpro") == NULL && parent.FindAttachmentBySlotName("pistolMuzzle") == NULL )
		{
			return true;
		}
		return false;
	}
}
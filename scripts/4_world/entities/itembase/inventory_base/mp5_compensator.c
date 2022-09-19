class MP5_Compensator extends ItemSuppressor
{
	override bool CanPutAsAttachment( EntityAI parent )
	{
		if(!super.CanPutAsAttachment(parent)) {return false;}
		if ( !parent.FindAttachmentBySlotName("pistolMuzzle") )
		{
			return true;
		}
		return false;
	}
}
class MakarovPBSuppressor extends ItemSuppressor
{
	override bool CanPutAsAttachment( EntityAI parent )
	{
		if(!super.CanPutAsAttachment(parent)) {return false;}
		if ( parent.FindAttachmentBySlotName("suppressorImpro") == NULL && parent.FindAttachmentBySlotName("weaponMuzzleMP5") == NULL )
		{
			return true;
		}
		return false;
	}
}
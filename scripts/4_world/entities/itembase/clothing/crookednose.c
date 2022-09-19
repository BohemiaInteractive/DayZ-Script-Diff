class CrookedNose extends Clothing
{
	override bool CanPutAsAttachment( EntityAI parent )
	{
		if (!super.CanPutAsAttachment(parent)) {return false;}
		
		Clothing headgear = Clothing.Cast(parent.FindAttachmentBySlotName("Headgear"));
		if ( headgear && headgear.ConfigGetBool("noMask") )
		{
			return false;
		}
		
		return true;
	}
}

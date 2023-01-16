class TacticalGoggles extends Clothing 
{
	override int GetGlassesEffectID()
	{
		return PPERequesterBank.REQ_GLASSESTACTICAL;
	}
	
	override bool CanPutAsAttachment(EntityAI parent)
	{
		if (!super.CanPutAsAttachment(parent))
			return false;
		
		Clothing mask = Clothing.Cast(parent.FindAttachmentBySlotName("Mask"));
		if (mask && mask.ConfigGetBool("noEyewear"))
		{
			return false;
		}
		
		return true;
	}
};
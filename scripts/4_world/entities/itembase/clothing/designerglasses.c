class DesignerGlasses extends Clothing 
{
	override int GetGlassesEffectID()
	{
		return PPERequesterBank.REQ_GLASSESDESIGNER;
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